# CryptoLabX

## Overview

CryptoLabX is a modular cryptography toolkit developed as part of the **Cryptography Laboratory (22CPP307)** course. The goal of this project is to build a reusable framework for implementing, testing, and analyzing various cryptographic algorithms and cryptanalysis techniques throughout the semester.

Week 1 focuses on establishing the project structure, creating a command-line interface, implementing basic file analysis, maintaining execution logs, and preparing the foundation for future cryptographic modules.

---

## Team Members

| Name          | Roll Number  |
| --------      | -----------  |
| Joyece Gajraj | 2024ucp1829  |
| Devansh Loya  | 2024ucp1898  |

---

## Features Implemented (Week 1)

* Professional project folder structure
* Menu-driven command-line interface
* File analysis utility
* Text statistics:

  * Number of characters
  * Number of words
  * Number of lines
  * Number of unique characters
  * Letter frequency analysis
* Execution logging with date and time
* Sample datasets for testing
* Git repository initialization

---

## Project Structure

```
CryptoLabX/
│
├── classical/
├── attacks/
├── math/
├── modern/
├── analysis/
├── datasets/
├── outputs/
├── docs/
├── tests/
├── utils/
│
├── main.py
├── README.md
└── requirements.txt
```

---

## Folder Description

**classical/**
Contains implementations of classical cryptographic algorithms such as Caesar, Vigenère, Playfair, Hill Cipher, and Rail Fence Cipher.

**modern/**
Contains implementations of modern cryptographic algorithms such as DES, AES, RSA, ECC, and Hashing algorithms.

**attacks/**
Contains cryptanalysis and attack techniques including brute-force attacks, frequency analysis, dictionary attacks, and ciphertext-only attacks.

**math/**
Contains mathematical utilities required by cryptographic algorithms, including modular arithmetic, matrices, GCD, multiplicative inverse, and prime number operations.

**analysis/**
Contains statistical and analytical tools for processing ciphertexts and plaintexts.

**datasets/**
Stores sample text files used for testing, analysis, and future assignments.

**outputs/**
Stores generated output files, logs, reports, and encrypted/decrypted files.

**docs/**
Contains project documentation, reports, and user guides.

**tests/**
Contains test programs and unit tests for validating project modules.

**utils/**
Contains helper functions and reusable utility modules such as logging and file handling.

---

## Future Modules

The following features will be added in future assignments:


---

## Conclusion

CryptoLabX provides a well-organized and scalable foundation for developing cryptographic algorithms and cryptanalysis tools. The modular design makes it easy to extend the project with new algorithms and analysis techniques in future assignments while maintaining clean, reusable, and well-documented code.

