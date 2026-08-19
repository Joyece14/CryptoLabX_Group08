def encrypt(plaintext, key):
    """
    Encrypt plaintext using Shift Cipher.
    key must be between 0 and 25.
    """
    result = ""

    for char in plaintext:
        if char.isalpha():
            base = ord('A') if char.isupper() else ord('a')
            result += chr((ord(char) - base + key) % 26 + base)
        else:
            result += char

    return result


def decrypt(ciphertext, key):
    """
    Decrypt ciphertext using Shift Cipher.
    """
    return encrypt(ciphertext, -key)


if __name__ == "__main__":
    plaintext = input("Enter plaintext: ")
    key = int(input("Enter key (0-25): "))

    ciphertext = encrypt(plaintext, key)

    print("Plaintext :", plaintext)
    print("Key       :", key)
    print("Ciphertext:", ciphertext)

    decrypted = decrypt(ciphertext, key)
    print("Decrypted :", decrypted)
