# Group 08 - E-Commerce Website Security Laboratory

## Application

E-Commerce Website

## Group

Group 08

## Technology

- Python 3
- Flask
- SQLite
- HTML/CSS
- pytest
- Bandit

## Core Functionalities

1. Product browsing
2. Shopping cart
3. Checkout
4. Order history

## Security Vulnerabilities Studied

### 1. SQL Injection

The vulnerable search endpoint constructs SQL using user-controlled
input.

Vulnerable endpoint:

/vulnerable/search

Secure endpoint:

/search

The secure implementation uses parameterized SQL queries.

---

### 2. Cross-Site Scripting (XSS)

The vulnerable XSS demonstration directly marks user input as safe
using Jinja's `safe` filter.

Vulnerable endpoint:

/vulnerable/xss

The secure implementation uses normal Jinja escaping.

---

### 3. IDOR / Broken Access Control

The vulnerable order endpoint retrieves an order using only its ID.

Vulnerable endpoint:

/vulnerable/order/<order_id>

The secure implementation verifies both:

- requested order ID
- authenticated user's ID

Secure endpoint:

/orders/<order_id>

## Demo Accounts

alice / alice123

bob / bob123

These credentials are for the local laboratory only.

## Installation

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
