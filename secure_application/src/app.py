import os
import sqlite3
from functools import wraps

from flask import (
    Flask,
    abort,
    flash,
    g,
    redirect,
    render_template,
    request,
    session,
    url_for,
)
from werkzeug.security import check_password_hash, generate_password_hash


BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DATABASE = os.path.join(BASE_DIR, "ecommerce.db")

app = Flask(__name__)
app.config.update(
    SECRET_KEY=os.environ.get("SECRET_KEY", "group08-development-key"),
    DATABASE=DATABASE,
    SESSION_COOKIE_HTTPONLY=True,
    SESSION_COOKIE_SAMESITE="Lax",
)


def get_db():
    """Open one SQLite connection per request."""
    if "db" not in g:
        g.db = sqlite3.connect(app.config["DATABASE"])
        g.db.row_factory = sqlite3.Row
    return g.db


@app.teardown_appcontext
def close_db(exception=None):
    db = g.pop("db", None)

    if db is not None:
        db.close()


def init_db():
    """Create database and demonstration data."""
    db = get_db()

    with open(os.path.join(BASE_DIR, "schema.sql"), "r", encoding="utf-8") as file:
        db.executescript(file.read())

    users = [
        ("alice", generate_password_hash("alice123")),
        ("bob", generate_password_hash("bob123")),
    ]

    db.executemany(
        "INSERT INTO users (username, password_hash) VALUES (?, ?)",
        users,
    )

    products = [
        (
            "Laptop",
            "15-inch development laptop",
            65000.00,
        ),
        (
            "Keyboard",
            "Mechanical USB keyboard",
            3500.00,
        ),
        (
            "Mouse",
            "Wireless optical mouse",
            1200.00,
        ),
        (
            "Headphones",
            "Noise-isolating headphones",
            4500.00,
        ),
    ]

    db.executemany(
        """
        INSERT INTO products (name, description, price)
        VALUES (?, ?, ?)
        """,
        products,
    )

    db.commit()


@app.before_request
def load_user():
    user_id = session.get("user_id")

    if user_id is None:
        g.user = None
    else:
        g.user = get_db().execute(
            "SELECT id, username FROM users WHERE id = ?",
            (user_id,),
        ).fetchone()


def login_required(view):
    @wraps(view)
    def wrapped_view(*args, **kwargs):
        if g.user is None:
            return redirect(url_for("login"))

        return view(*args, **kwargs)

    return wrapped_view


@app.route("/")
def index():
    products = get_db().execute(
        "SELECT * FROM products ORDER BY id"
    ).fetchall()

    return render_template("index.html", products=products)


# ---------------------------------------------------------
# Authentication
# ---------------------------------------------------------

@app.route("/login", methods=("GET", "POST"))
def login():
    if request.method == "POST":
        username = request.form.get("username", "").strip()
        password = request.form.get("password", "")

        user = get_db().execute(
            "SELECT * FROM users WHERE username = ?",
            (username,),
        ).fetchone()

        if user is None or not check_password_hash(
            user["password_hash"], password
        ):
            flash("Invalid username or password.")
            return render_template("login.html"), 401

        session.clear()
        session["user_id"] = user["id"]

        return redirect(url_for("index"))

    return render_template("login.html")


@app.route("/logout")
def logout():
    session.clear()
    return redirect(url_for("index"))


# ---------------------------------------------------------
# Shopping cart
# ---------------------------------------------------------

def get_cart():
    return session.setdefault("cart", {})


@app.post("/cart/add/<int:product_id>")
def add_to_cart(product_id):
    product = get_db().execute(
        "SELECT id FROM products WHERE id = ?",
        (product_id,),
    ).fetchone()

    if product is None:
        abort(404)

    cart = get_cart()
    key = str(product_id)
    cart[key] = cart.get(key, 0) + 1

    session.modified = True
    flash("Product added to cart.")

    return redirect(url_for("index"))


@app.route("/cart")
def cart():
    cart_data = get_cart()

    products = []

    for product_id, quantity in cart_data.items():
        product = get_db().execute(
            "SELECT * FROM products WHERE id = ?",
            (product_id,),
        ).fetchone()

        if product:
            products.append(
                {
                    "id": product["id"],
                    "name": product["name"],
                    "price": product["price"],
                    "quantity": quantity,
                    "subtotal": product["price"] * quantity,
                }
            )

    total = sum(item["subtotal"] for item in products)

    return render_template(
        "cart.html",
        products=products,
        total=total,
    )


@app.post("/cart/clear")
def clear_cart():
    session["cart"] = {}
    return redirect(url_for("cart"))


# ---------------------------------------------------------
# Checkout
# ---------------------------------------------------------

@app.post("/checkout")
@login_required
def checkout():
    cart_data = get_cart()

    if not cart_data:
        flash("Your cart is empty.")
        return redirect(url_for("cart"))

    db = get_db()
    total = 0
    valid_items = []

    for product_id, quantity in cart_data.items():
        product = db.execute(
            "SELECT * FROM products WHERE id = ?",
            (product_id,),
        ).fetchone()

        if product is None:
            continue

        quantity = int(quantity)

        if quantity < 1 or quantity > 20:
            abort(400)

        subtotal = product["price"] * quantity
        total += subtotal

        valid_items.append(
            (
                product["id"],
                quantity,
                product["price"],
            )
        )

    if not valid_items:
        abort(400)

    cursor = db.execute(
        "INSERT INTO orders (user_id, total) VALUES (?, ?)",
        (g.user["id"], total),
    )

    order_id = cursor.lastrowid

    for product_id, quantity, price in valid_items:
        db.execute(
            """
            INSERT INTO order_items
            (order_id, product_id, quantity, price)
            VALUES (?, ?, ?, ?)
            """,
            (order_id, product_id, quantity, price),
        )

    db.commit()

    session["cart"] = {}

    flash(f"Order #{order_id} placed successfully.")

    return redirect(url_for("orders"))


# ---------------------------------------------------------
# Order history
# ---------------------------------------------------------

@app.route("/orders")
@login_required
def orders():
    orders_data = get_db().execute(
        """
        SELECT id, total, created_at
        FROM orders
        WHERE user_id = ?
        ORDER BY id DESC
        """,
        (g.user["id"],),
    ).fetchall()

    return render_template(
        "orders.html",
        orders=orders_data,
    )


@app.route("/orders/<int:order_id>")
@login_required
def order_detail(order_id):
    """
    SECURE VERSION OF IDOR.

    The order must belong to the currently authenticated user.
    """
    order = get_db().execute(
        """
        SELECT *
        FROM orders
        WHERE id = ? AND user_id = ?
        """,
        (order_id, g.user["id"]),
    ).fetchone()

    if order is None:
        abort(403)

    items = get_db().execute(
        """
        SELECT
            order_items.quantity,
            order_items.price,
            products.name
        FROM order_items
        JOIN products ON products.id = order_items.product_id
        WHERE order_items.order_id = ?
        """,
        (order_id,),
    ).fetchall()

    return render_template(
        "order.html",
        order=order,
        items=items,
    )


# ---------------------------------------------------------
# SECURE SEARCH
# ---------------------------------------------------------

@app.route("/search")
def search():
    query = request.args.get("q", "").strip()

    # Secure against SQL injection:
    # user input is passed as a query parameter.
    products = get_db().execute(
        """
        SELECT *
        FROM products
        WHERE name LIKE ? OR description LIKE ?
        """,
        (f"%{query}%", f"%{query}%"),
    ).fetchall()

    return render_template(
        "search.html",
        products=products,
        query=query,
        vulnerable=False,
    )


# ---------------------------------------------------------
# VULNERABILITY DEMONSTRATIONS
# ---------------------------------------------------------

@app.route("/vulnerable/search")
def vulnerable_search():
    """
    INTENTIONALLY VULNERABLE SQL INJECTION DEMONSTRATION.

    DO NOT use this pattern in production.
    This endpoint exists only for the security laboratory.
    """
    query = request.args.get("q", "")

    unsafe_sql = (
        "SELECT * FROM products "
        f"WHERE name LIKE '%{query}%' "
        f"OR description LIKE '%{query}%'"
    )

    products = get_db().execute(unsafe_sql).fetchall()

    return render_template(
        "search.html",
        products=products,
        query=query,
        vulnerable=True,
    )

@app.route("/vulnerable/xss")
def vulnerable_xss():
    query = request.args.get("q", "")

    return render_template(
        "xss_vulnerable.html",
        query=query,
    )



@app.route("/vulnerable/order/<int:order_id>")
@login_required
def vulnerable_order(order_id):
    """
    INTENTIONALLY VULNERABLE IDOR DEMONSTRATION.

    The application checks that the user is logged in,
    but DOES NOT check that the order belongs to that user.
    """
    order = get_db().execute(
        """
        SELECT *
        FROM orders
        WHERE id = ?
        """,
        (order_id,),
    ).fetchone()

    if order is None:
        abort(404)

    items = get_db().execute(
        """
        SELECT
            order_items.quantity,
            order_items.price,
            products.name
        FROM order_items
        JOIN products ON products.id = order_items.product_id
        WHERE order_items.order_id = ?
        """,
        (order_id,),
    ).fetchall()

    return render_template(
        "order.html",
        order=order,
        items=items,
        vulnerable=True,
    )


# ---------------------------------------------------------
# Error pages
# ---------------------------------------------------------

@app.errorhandler(403)
def forbidden(error):
    return "<h1>403 Forbidden</h1><p>Access denied.</p>", 403


@app.errorhandler(404)
def not_found(error):
    return "<h1>404 Not Found</h1>", 404


# ---------------------------------------------------------
# Development entry point
# ---------------------------------------------------------

if __name__ == "__main__":
    with app.app_context():
        if not os.path.exists(DATABASE):
            init_db()

    app.run(host="127.0.0.1", port=5000, debug=False)
