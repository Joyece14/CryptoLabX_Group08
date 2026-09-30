#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cctype>

using namespace std;

// --------------------------------------------------
// Generate 5x5 Key Matrix
// --------------------------------------------------
vector<vector<char>> generate_key_matrix(string keyword) {

    vector<vector<char>> matrix(5, vector<char>(5));
    string key = "";

    // Process keyword
    for (char ch : keyword) {

        if (isalpha(ch)) {
            ch = toupper(ch);

            // Combine I and J
            if (ch == 'J')
                ch = 'I';

            // Remove duplicate letters
            if (key.find(ch) == string::npos)
                key += ch;
        }
    }

    // Add remaining alphabet
    string alphabet = "ABCDEFGHIKLMNOPQRSTUVWXYZ";

    for (char ch : alphabet) {
        if (key.find(ch) == string::npos)
            key += ch;
    }

    // Fill 5x5 matrix
    int k = 0;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            matrix[i][j] = key[k++];
        }
    }

    return matrix;
}

// --------------------------------------------------
// Display Matrix
// --------------------------------------------------
void display_matrix(const vector<vector<char>>& matrix) {

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

// --------------------------------------------------
// Prepare Plaintext
// --------------------------------------------------
string prepare_plaintext(string plaintext) {

    string text = "";

    for (char ch : plaintext) {

        if (isalpha(ch)) {

            ch = toupper(ch);

            // Combine J with I
            if (ch == 'J')
                ch = 'I';

            text += ch;
        }
    }

    return text;
}

// --------------------------------------------------
// Create Digraphs
// --------------------------------------------------
vector<string> create_digraphs(string text) {

    vector<string> digraphs;

    int i = 0;

    while (i < text.length()) {

        char first = text[i];

        // Last character
        if (i + 1 >= text.length()) {

            digraphs.push_back(string(1, first) + "X");
            i++;
        }

        // Repeated letters
        else if (text[i] == text[i + 1]) {

            digraphs.push_back(string(1, first) + "X");
            i++;
        }

        // Normal pair
        else {

            string pair = "";

            pair += text[i];
            pair += text[i + 1];

            digraphs.push_back(pair);

            i += 2;
        }
    }

    return digraphs;
}

// --------------------------------------------------
// Find Position of Character
// --------------------------------------------------
void find_position(
    const vector<vector<char>>& matrix,
    char ch,
    int& row,
    int& col
) {

    for (int i = 0; i < 5; i++) {

        for (int j = 0; j < 5; j++) {

            if (matrix[i][j] == ch) {
                row = i;
                col = j;
                return;
            }
        }
    }
}

// --------------------------------------------------
// Encrypt
// --------------------------------------------------
string playfair_encrypt(
    vector<string> digraphs,
    const vector<vector<char>>& matrix
) {

    string ciphertext = "";

    for (string pair : digraphs) {

        char a = pair[0];
        char b = pair[1];

        int r1, c1, r2, c2;

        find_position(matrix, a, r1, c1);
        find_position(matrix, b, r2, c2);

        // Same row
        if (r1 == r2) {

            ciphertext += matrix[r1][(c1 + 1) % 5];
            ciphertext += matrix[r2][(c2 + 1) % 5];
        }

        // Same column
        else if (c1 == c2) {

            ciphertext += matrix[(r1 + 1) % 5][c1];
            ciphertext += matrix[(r2 + 1) % 5][c2];
        }

        // Rectangle rule
        else {

            ciphertext += matrix[r1][c2];
            ciphertext += matrix[r2][c1];
        }
    }

    return ciphertext;
}

// --------------------------------------------------
// Decrypt
// --------------------------------------------------
string playfair_decrypt(
    string ciphertext,
    const vector<vector<char>>& matrix
) {

    string plaintext = "";

    for (int i = 0; i < ciphertext.length(); i += 2) {

        char a = ciphertext[i];
        char b = ciphertext[i + 1];

        int r1, c1, r2, c2;

        find_position(matrix, a, r1, c1);
        find_position(matrix, b, r2, c2);

        // Same row
        if (r1 == r2) {

            plaintext += matrix[r1][(c1 + 4) % 5];
            plaintext += matrix[r2][(c2 + 4) % 5];
        }

        // Same column
        else if (c1 == c2) {

            plaintext += matrix[(r1 + 4) % 5][c1];
            plaintext += matrix[(r2 + 4) % 5][c2];
        }

        // Rectangle rule
        else {

            plaintext += matrix[r1][c2];
            plaintext += matrix[r2][c1];
        }
    }

    return plaintext;
}

// --------------------------------------------------
// Digraph Frequency Analysis
// --------------------------------------------------
void digraph_frequency(string ciphertext) {

    map<string, int> frequency;

    for (int i = 0; i < ciphertext.length(); i += 2) {

        string pair = ciphertext.substr(i, 2);

        frequency[pair]++;
    }

    cout << "\nDigraph Frequency Analysis:\n";

    for (auto x : frequency) {

        cout << x.first << " : "
             << x.second << endl;
    }
}

// --------------------------------------------------
// Verification
// --------------------------------------------------
bool verify(
    string ciphertext,
    string decrypted,
    const vector<vector<char>>& matrix
) {

    vector<string> digraphs =
        create_digraphs(decrypted);

    string reencrypted =
        playfair_encrypt(digraphs, matrix);

    return reencrypted == ciphertext;
}

// --------------------------------------------------
// MAIN
// --------------------------------------------------
int main() {

    string keyword;
    string plaintext;

    // Take input
    cout << "Enter Keyword: ";
    getline(cin, keyword);

    cout << "Enter Plaintext: ";
    getline(cin, plaintext);

    // Generate key matrix
    vector<vector<char>> matrix =
        generate_key_matrix(keyword);

    // Display matrix
    cout << "\nKey Matrix:\n";
    display_matrix(matrix);

    // Prepare plaintext
    string prepared =
        prepare_plaintext(plaintext);

    // Create digraphs
    vector<string> digraphs =
        create_digraphs(prepared);

    // Display digraphs
    cout << "\nPrepared Digraphs:\n";

    for (string pair : digraphs) {
        cout << pair << " ";
    }

    // Encryption
    string ciphertext =
        playfair_encrypt(digraphs, matrix);

    cout << "\n\nCiphertext:\n";
    cout << ciphertext << endl;

    // Decryption
    string decrypted =
        playfair_decrypt(ciphertext, matrix);

    cout << "\nDecrypted Text:\n";
    cout << decrypted << endl;

    // Frequency analysis
    digraph_frequency(ciphertext);

    // Verification
    cout << "\nVerification:\n";

    if (verify(ciphertext, decrypted, matrix))
        cout << "SUCCESS" << endl;
    else
        cout << "FAILED" << endl;

    return 0;
}

