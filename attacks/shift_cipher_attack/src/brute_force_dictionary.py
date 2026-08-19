import os
import re
from shift_cipher import decrypt


def load_dictionary(dictionary_path):
    """
    Load English words from dictionary file.
    """
    words = set()

    if not os.path.exists(dictionary_path):
        print("Dictionary file not found:", dictionary_path)
        return words

    with open(dictionary_path, "r", encoding="utf-8") as file:
        for line in file:
            word = line.strip().lower()

            if word:
                words.add(word)

    return words


def dictionary_score(text, dictionary):
    """
    Count how many words in the text occur in the dictionary.
    """
    words = re.findall(r"[a-zA-Z]+", text.lower())

    score = 0

    for word in words:
        if word in dictionary:
            score += 1

    return score


def brute_force(ciphertext):
    """
    Try all 26 possible Shift Cipher keys.
    """
    results = []

    for key in range(26):
        plaintext = decrypt(ciphertext, key)
        results.append((key, plaintext))

    return results


def dictionary_attack(ciphertext, dictionary):
    """
    Predict the key using dictionary scoring.
    """
    best_key = None
    best_plaintext = None
    best_score = -1

    for key in range(26):
        plaintext = decrypt(ciphertext, key)
        score = dictionary_score(plaintext, dictionary)

        if score > best_score:
            best_score = score
            best_key = key
            best_plaintext = plaintext

    return best_key, best_plaintext, best_score


if __name__ == "__main__":
    ciphertext = input("Enter ciphertext: ")

    dictionary_path = os.path.join(
        os.path.dirname(os.path.dirname(__file__)),
        "dictionary",
        "english_words.txt"
    )

    dictionary = load_dictionary(dictionary_path)

    print("\n--- BRUTE FORCE ---")

    for key, plaintext in brute_force(ciphertext):
        print(f"Key {key:2d}: {plaintext}")

    print("\n--- DICTIONARY ATTACK ---")

    key, plaintext, score = dictionary_attack(
        ciphertext,
        dictionary
    )

    print("Predicted Key :", key)
    print("Plaintext     :", plaintext)
    print("Dictionary Score:", score)
