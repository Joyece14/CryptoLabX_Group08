import os

from shift_cipher import encrypt
from brute_force_dictionary import (
    load_dictionary,
    brute_force,
    dictionary_attack
)
from chi_square_attack import chi_square_attack


def main():
    print("=" * 60)
    print("       SHIFT CIPHER CRYPTANALYSIS")
    print("=" * 60)

    # Input

    plaintext = input("\nEnter plaintext: ")
    actual_key = int(input("Enter encryption key (0-25): "))

    if not 0 <= actual_key <= 25:
        print("Error: Key must be between 0 and 25.")
        return

    # Encryption

    ciphertext = encrypt(plaintext, actual_key)

    print("\n--- ENCRYPTION ---")
    print("Plaintext  :", plaintext)
    print("Actual Key :", actual_key)
    print("Ciphertext :", ciphertext)

    # Dictionary

    src_directory = os.path.dirname(os.path.abspath(__file__))
    project_directory = os.path.dirname(src_directory)

    dictionary_path = os.path.join(
        project_directory,
        "dictionary",
        "english_words.txt"
    )

    print("\nDictionary path:", dictionary_path)

    if not os.path.exists(dictionary_path):
        print("ERROR: english_words.txt not found!")
        print("Expected location:")
        print(dictionary_path)
        return

    dictionary = load_dictionary(dictionary_path)

    print("Dictionary loaded successfully.")
    print("Number of words:", len(dictionary))

    # Brute Force

    print("\n--- BRUTE FORCE ATTACK ---")

    brute_results = brute_force(ciphertext)

    for key, candidate in brute_results:
        print(f"Key {key:2d}: {candidate}")

    # Dictionary Attack

    print("\n--- DICTIONARY ATTACK ---")

    dictionary_key, dictionary_plaintext, dictionary_score_value = (
        dictionary_attack(
            ciphertext,
            dictionary
        )
    )

    print("Predicted Key :", dictionary_key)
    print("Plaintext     :", dictionary_plaintext)
    print("Score         :", dictionary_score_value)

    # Chi-Square Attack

    print("\n--- CHI-SQUARE ATTACK ---")

    chi_key, chi_plaintext, chi_score, chi_results = (
        chi_square_attack(ciphertext)
    )

    print("Predicted Key :", chi_key)
    print("Plaintext     :", chi_plaintext)
    print("Chi-Square    :", chi_score)

    # Comparison

    dictionary_correct = (
        dictionary_key == actual_key
    )

    chi_square_correct = (
        chi_key == actual_key
    )

    print("\n--- FINAL COMPARISON ---")

    print(
        f"Actual Key          : {actual_key}"
    )

    print(
        f"Dictionary Key      : {dictionary_key}"
    )

    print(
        f"Chi-Square Key      : {chi_key}"
    )

    print(
        f"Dictionary Correct? : "
        f"{'Yes' if dictionary_correct else 'No'}"
    )

    print(
        f"Chi-Square Correct?  : "
        f"{'Yes' if chi_square_correct else 'No'}"
    )

    print("=" * 60)


if __name__ == "__main__":
    main()
