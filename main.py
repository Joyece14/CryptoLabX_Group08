import os

def encrypt():
    print("\n[Encrypt]")
    print("Coming Soon...\n")


def decrypt():
    print("\n[Decrypt]")
    print("Coming Soon...\n")


def attack():
    print("\n[Attack]")
    print("Coming Soon...\n")

from collections import Counter
import string

def analyze_file():
    filename = input("Enter file name (example: sample1.txt): ")

    filepath = f"datasets/{filename}"

    try:
        with open(filepath, "r", encoding="utf-8") as file:
            text = file.read()

        # Number of characters
        characters = len(text)

        # Number of words
        words = len(text.split())

        # Number of lines
        lines = len(text.splitlines())

        # Unique characters
        unique_characters = len(set(text))

        # Letter frequency (case-insensitive)
        letters = [char.lower() for char in text if char.isalpha()]
        frequency = Counter(letters)

        print("\n========== File Analysis ==========")
        print(f"Characters        : {characters}")
        print(f"Words             : {words}")
        print(f"Lines             : {lines}")
        print(f"Unique Characters : {unique_characters}")

        print("\nLetter Frequency")
        print("-------------------------")

        for letter in string.ascii_lowercase:
            print(f"{letter} : {frequency.get(letter, 0)}")

        print("===================================")

    except FileNotFoundError:
        print("\nError: File not found.")

def analyze():
    print("\n[Analyze]")
    print("This feature will analyze text files in the datasets folder.")
    analyze_file()
    print("Coming Soon...\n")


def display_menu():
    print("=" * 40)
    print("        CryptoLabX Toolkit")
    print("=" * 40)
    print("1. Encrypt")
    print("2. Decrypt")
    print("3. Attack")
    print("4. Analyze")
    print("5. Exit")
    print("=" * 40)
from datetime import datetime

def write_log(option):
    now = datetime.now()
    timestamp = now.strftime("%Y-%m-%d %H:%M:%S")

    with open("outputs/log.txt", "a") as logfile:
        logfile.write(f"{timestamp} - {option}\n")

def main():
    while True:
        display_menu()

        choice = input("Enter your choice (1-5): ")

        if choice == '1':
            write_log("Encrypt")
            encrypt()

        elif choice == '2':
            write_log("Decrypt")
            decrypt()

        elif choice == '3':
            write_log("Attack")        
            attack()

        elif choice == '4':
            write_log("Analyze")        
            analyze()

        elif choice == '5':
            write_log("Exit")        
            print("\nThank you for using CryptoLabX.")
            print("Exiting...\n")
            break

        else:
            print("\nInvalid choice! Please enter a number between 1 and 5.\n")


if __name__ == "__main__":
    main()
