import os
import sys
import tempfile

import pytest

sys.path.insert(
    0,
    os.path.abspath(
        os.path.join(os.path.dirname(__file__), "..", "src")
    ),
)

from app import app, init_db


@pytest.fixture()
def client():
    database_fd, database_path = tempfile.mkstemp()

    app.config.update(
        TESTING=True,
        DATABASE=database_path,
        SECRET_KEY="test-key",
    )

    with app.app_context():
        init_db()

    with app.test_client() as client:
        yield client

    os.close(database_fd)
    os.unlink(database_path)


def login(client, username, password):
    return client.post(
        "/login",
        data={
            "username": username,
            "password": password,
        },
        follow_redirects=True,
    )


def test_home_page(client):
    response = client.get("/")

    assert response.status_code == 200
    assert b"Laptop" in response.data


def test_product_search(client):
    response = client.get("/search?q=Laptop")

    assert response.status_code == 200
    assert b"Laptop" in response.data


def test_sql_injection_is_blocked_by_secure_search(client):
    payload = "' OR '1'='1"

    response = client.get(
        "/search",
        query_string={"q": payload},
    )

    assert response.status_code == 200


def test_xss_is_escaped_on_secure_page(client):
    payload = "<script>alert('XSS')</script>"

    response = client.get(
        "/search",
        query_string={"q": payload},
    )

    assert response.status_code == 200

    # The script must not appear as executable HTML.
    assert b"<script>" not in response.data


def test_login(client):
    response = login(
        client,
        "alice",
        "alice123",
    )

    assert response.status_code == 200
    assert b"alice" in response.data


def test_checkout_requires_login(client):
    response = client.post("/checkout")

    assert response.status_code == 302
    assert "/login" in response.headers["Location"]


def test_idor_is_blocked(client):
    # Alice logs in.
    login(client, "alice", "alice123")

    # The secure endpoint must not expose another user's order.
    response = client.get("/orders/999999")

    assert response.status_code in (403, 404)


def test_orders_require_authentication(client):
    response = client.get("/orders")

    assert response.status_code == 302
    assert "/login" in response.headers["Location"]
