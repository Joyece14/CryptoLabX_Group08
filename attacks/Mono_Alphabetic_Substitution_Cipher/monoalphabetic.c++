#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <cctype>

using namespace std;

// ============================================================
// FILE FUNCTIONS
// ============================================================

string readTextFile(const string& filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "ERROR: Cannot open file: "
             << filename << endl;
        exit(1);
    }

    string text;
    string line;

    while (getline(file, line))
    {
        text += line;
        text += '\n';
    }

    file.close();

    return text;
}


string readKeyFile(const string& filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "ERROR: Cannot open key file: "
             << filename << endl;
        exit(1);
    }

    string key;
    string line;

    while (getline(file, line))
    {
        for (char c : line)
        {
            if (isalpha(static_cast<unsigned char>(c)))
            {
                key += toupper(
                    static_cast<unsigned char>(c));
            }
        }
    }

    file.close();

    // Check key length
    if (key.length() != 26)
    {
        cerr << "ERROR: Key must contain 26 letters."
             << endl;
        exit(1);
    }

    // Check that all letters are unique
    set<char> letters(key.begin(), key.end());

    if (letters.size() != 26)
    {
        cerr << "ERROR: Key must contain each "
                "letter A-Z exactly once."
             << endl;
        exit(1);
    }

    return key;
}


vector<string> readSubstitutions(
    const string& filename)
{
    ifstream file(filename);

    vector<string> substitutions;

    if (!file.is_open())
    {
        cout << "WARNING: "
             << filename
             << " not found.\n";

        return substitutions;
    }

    string line;

    while (getline(file, line))
    {
        // Remove spaces
        string cleaned;

        for (char c : line)
        {
            if (!isspace(
                    static_cast<unsigned char>(c)))
            {
                cleaned += c;
            }
        }

        if (cleaned.length() == 3 &&
            cleaned[1] == '=')
        {
            substitutions.push_back(cleaned);
        }
    }

    file.close();

    return substitutions;
}


void writeTextFile(
    const string& filename,
    const string& text)
{
    ofstream file(filename);

    if (!file.is_open())
    {
        cerr << "ERROR: Cannot write to "
             << filename << endl;
        return;
    }

    file << text;

    file.close();
}


// ============================================================
// UTILITY
// ============================================================

string toUpperCase(const string& text)
{
    string result = text;

    for (char& c : result)
    {
        c = toupper(
            static_cast<unsigned char>(c));
    }

    return result;
}


vector<string> getWords(const string& text)
{
    vector<string> words;

    string current;

    for (char c : text)
    {
        if (isalpha(
                static_cast<unsigned char>(c)))
        {
            current += toupper(
                static_cast<unsigned char>(c));
        }
        else
        {
            if (!current.empty())
            {
                words.push_back(current);
                current.clear();
            }
        }
    }

    if (!current.empty())
    {
        words.push_back(current);
    }

    return words;
}


// ============================================================
// MONOALPHABETIC ENCRYPTION
// ============================================================

string encrypt_text(
    const string& plaintext,
    const string& key)
{
    string ciphertext = plaintext;

    for (char& c : ciphertext)
    {
        if (isalpha(
                static_cast<unsigned char>(c)))
        {
            bool lowercase =
                islower(
                    static_cast<unsigned char>(c));

            char upper =
                toupper(
                    static_cast<unsigned char>(c));

            int index = upper - 'A';

            char encrypted = key[index];

            if (lowercase)
            {
                c = tolower(
                    static_cast<unsigned char>(
                        encrypted));
            }
            else
            {
                c = encrypted;
            }
        }
    }

    return ciphertext;
}


// ============================================================
// 1. FREQUENCY ANALYSIS
// ============================================================

void frequency_analysis(
    const string& ciphertext)
{
    ofstream out(
        "output/frequency_analysis.txt");

    cout << "\n=============================================\n";
    cout << "          LETTER FREQUENCY ANALYSIS\n";
    cout << "=============================================\n";

    map<char, int> frequency;

    int totalLetters = 0;

    for (char c : ciphertext)
    {
        if (isalpha(
                static_cast<unsigned char>(c)))
        {
            char letter =
                toupper(
                    static_cast<unsigned char>(c));

            frequency[letter]++;
            totalLetters++;
        }
    }

    vector<pair<char, int>> sorted;

    for (char c = 'A'; c <= 'Z'; c++)
    {
        sorted.push_back(
            {c, frequency[c]});
    }

    sort(
        sorted.begin(),
        sorted.end(),
        [](const pair<char, int>& a,
           const pair<char, int>& b)
        {
            return a.second > b.second;
        });

    cout << left
         << setw(10) << "Letter"
         << setw(10) << "Count"
         << setw(15) << "Percentage"
         << endl;

    out << left
        << setw(10) << "Letter"
        << setw(10) << "Count"
        << setw(15) << "Percentage"
        << endl;

    cout << "----------------------------------------\n";
    out << "----------------------------------------\n";

    for (auto& item : sorted)
    {
        double percentage = 0;

        if (totalLetters > 0)
        {
            percentage =
                item.second * 100.0 /
                totalLetters;
        }

        cout << left
             << setw(10) << item.first
             << setw(10) << item.second
             << fixed << setprecision(2)
             << percentage << "%\n";

        out << left
            << setw(10) << item.first
            << setw(10) << item.second
            << fixed << setprecision(2)
            << percentage << "%\n";
    }

    cout << "\nDescending frequency:\n";

    out << "\nDescending frequency:\n";

    for (auto& item : sorted)
    {
        if (item.second > 0)
        {
            cout << item.first
                 << "("
                 << item.second
                 << ") ";

            out << item.first
                << "("
                << item.second
                << ") ";
        }
    }

    cout << endl;
    out << endl;

    if (!sorted.empty())
    {
        cout << "\nMost frequent ciphertext letter: "
             << sorted[0].first << endl;

        out << "\nMost frequent ciphertext letter: "
            << sorted[0].first << endl;
    }

    out.close();
}


// ============================================================
// 2. WORD FREQUENCY ANALYSIS
// ============================================================

void word_frequency_analysis(
    const string& ciphertext)
{
    ofstream out(
        "output/word_analysis.txt");

    cout << "\n=============================================\n";
    cout << "           WORD FREQUENCY ANALYSIS\n";
    cout << "=============================================\n";

    map<string, int> frequency;

    vector<string> words =
        getWords(ciphertext);

    for (const string& word : words)
    {
        frequency[word]++;
    }

    vector<pair<string, int>> sorted(
        frequency.begin(),
        frequency.end());

    sort(
        sorted.begin(),
        sorted.end(),
        [](const pair<string, int>& a,
           const pair<string, int>& b)
        {
            return a.second > b.second;
        });

    // One-letter words
    cout << "\nOne-letter words:\n";
    out << "One-letter words:\n";

    for (auto& item : sorted)
    {
        if (item.first.length() == 1)
        {
            cout << item.first
                 << " -> "
                 << item.second << endl;

            out << item.first
                << " -> "
                << item.second << endl;
        }
    }

    // Two-letter words
    cout << "\nTwo-letter words:\n";
    out << "\nTwo-letter words:\n";

    for (auto& item : sorted)
    {
        if (item.first.length() == 2)
        {
            cout << item.first
                 << " -> "
                 << item.second << endl;

            out << item.first
                << " -> "
                << item.second << endl;
        }
    }

    // Three-letter words
    cout << "\nThree-letter words:\n";
    out << "\nThree-letter words:\n";

    for (auto& item : sorted)
    {
        if (item.first.length() == 3)
        {
            cout << item.first
                 << " -> "
                 << item.second << endl;

            out << item.first
                << " -> "
                << item.second << endl;
        }
    }

    // Repeated words
    cout << "\nRepeated words:\n";
    out << "\nRepeated words:\n";

    for (auto& item : sorted)
    {
        if (item.second > 1)
        {
            cout << item.first
                 << " -> "
                 << item.second
                 << " times\n";

            out << item.first
                << " -> "
                << item.second
                << " times\n";
        }
    }

    out.close();
}


// ============================================================
// WORD PATTERN
// ============================================================

string get_pattern(
    const string& word)
{
    map<char, int> patternMap;

    int nextNumber = 0;

    string pattern;

    for (char c : word)
    {
        if (patternMap.find(c) ==
            patternMap.end())
        {
            patternMap[c] = nextNumber++;
        }

        pattern +=
            to_string(patternMap[c]);
    }

    return pattern;
}


// ============================================================
// 3. PATTERN ANALYSIS
// ============================================================

void pattern_analysis(
    const string& ciphertext)
{
    ofstream out(
        "output/pattern_analysis.txt");

    cout << "\n=============================================\n";
    cout << "              PATTERN ANALYSIS\n";
    cout << "=============================================\n";

    vector<string> words =
        getWords(ciphertext);

    map<string, vector<string>> patterns;

    for (const string& word : words)
    {
        patterns[
            get_pattern(word)
        ].push_back(word);
    }

    cout << "\nWord patterns:\n";
    out << "Word patterns:\n";

    for (auto& item : patterns)
    {
        cout << item.first << " : ";

        out << item.first << " : ";

        for (const string& word :
             item.second)
        {
            cout << word << " ";
            out << word << " ";
        }

        cout << endl;
        out << endl;
    }

    // Repeated-letter words
    cout << "\nRepeated-letter patterns:\n";
    out << "\nRepeated-letter patterns:\n";

    set<string> displayed;

    for (const string& word : words)
    {
        set<char> letters(
            word.begin(),
            word.end());

        if (letters.size() < word.length())
        {
            string pattern =
                get_pattern(word);

            string identifier =
                word + ":" + pattern;

            if (displayed.find(identifier)
                == displayed.end())
            {
                cout << word
                     << " -> "
                     << pattern
                     << endl;

                out << word
                    << " -> "
                    << pattern
                    << endl;

                displayed.insert(identifier);
            }
        }
    }

    out.close();
}


// ============================================================
// 4. APPLY SUBSTITUTION
// ============================================================

string apply_substitution(
    const string& ciphertext,
    const map<char, char>& cipherToPlain)
{
    string plaintext = ciphertext;

    for (char& c : plaintext)
    {
        if (isalpha(
                static_cast<unsigned char>(c)))
        {
            bool lowercase =
                islower(
                    static_cast<unsigned char>(c));

            char upper =
                toupper(
                    static_cast<unsigned char>(c));

            if (cipherToPlain.find(upper)
                != cipherToPlain.end())
            {
                char replacement =
                    cipherToPlain.at(upper);

                if (lowercase)
                {
                    c = tolower(
                        static_cast<unsigned char>(
                            replacement));
                }
                else
                {
                    c = replacement;
                }
            }
            else
            {
                c = '?';
            }
        }
    }

    return plaintext;
}


// ============================================================
// 5. DISPLAY PARTIAL PLAINTEXT
// ============================================================

void display_partial_plaintext(
    const string& ciphertext,
    const map<char, char>& cipherToPlain,
    ofstream& log)
{
    string partial =
        apply_substitution(
            ciphertext,
            cipherToPlain);

    cout << "\nPartial plaintext:\n";
    cout << partial << endl;

    log << "\nPartial plaintext:\n";
    log << partial << endl;
}


// ============================================================
// CHECK SUBSTITUTION
// ============================================================

bool add_mapping(
    map<char, char>& cipherToPlain,
    map<char, char>& plainToCipher,
    char cipher,
    char plain)
{
    cipher =
        toupper(
            static_cast<unsigned char>(
                cipher));

    plain =
        toupper(
            static_cast<unsigned char>(
                plain));

    // Check existing cipher mapping
    if (cipherToPlain.find(cipher)
        != cipherToPlain.end())
    {
        return
            cipherToPlain[cipher] == plain;
    }

    // Check whether plaintext letter
    // is already assigned
    if (plainToCipher.find(plain)
        != plainToCipher.end())
    {
        return false;
    }

    cipherToPlain[cipher] = plain;
    plainToCipher[plain] = cipher;

    return true;
}


// ============================================================
// AUTOMATIC ITERATIVE CRYPTANALYSIS
// ============================================================

void iterative_cryptanalysis(
    const string& ciphertext)
{
    cout << "\n=============================================\n";
    cout << "         ITERATIVE CRYPTANALYSIS\n";
    cout << "=============================================\n";

    ofstream log(
        "output/cryptanalysis_log.txt");

    vector<string> substitutions =
        readSubstitutions(
            "substitutions.txt");

    map<char, char> cipherToPlain;
    map<char, char> plainToCipher;

    int step = 1;

    log << "MONOALPHABETIC CIPHER "
            "CRYPTANALYSIS LOG\n\n";

    log << left
        << setw(6) << "Step"
        << setw(25) << "Observation"
        << setw(25) << "Possible"
        << setw(25) << "Tested"
        << setw(20) << "Result"
        << setw(15) << "Decision"
        << endl;

    log << string(116, '-') << endl;

    // --------------------------------------------------------
    // Apply substitutions one by one
    // --------------------------------------------------------

    for (const string& substitution :
         substitutions)
    {
        char cipher =
            toupper(substitution[0]);

        char plain =
            toupper(substitution[2]);

        bool accepted =
            add_mapping(
                cipherToPlain,
                plainToCipher,
                cipher,
                plain);

        string partial =
            apply_substitution(
                ciphertext,
                cipherToPlain);

        cout << "\n---------------------------------------------\n";

        cout << "Step " << step << endl;

        cout << "Testing: "
             << cipher
             << " -> "
             << plain
             << endl;

        if (accepted)
        {
            cout << "Decision: ACCEPTED\n";
        }
        else
        {
            cout << "Decision: REJECTED\n";
        }

        cout << "\nPartial plaintext:\n";
        cout << partial << endl;

        // Write table
        log << left
            << setw(6) << step;

        string observation =
            "Tested ciphertext " +
            string(1, cipher);

        string possible =
            string(1, cipher) +
            " -> " +
            string(1, plain);

        string tested =
            string(1, cipher) +
            " -> " +
            string(1, plain);

        string result;

        string decision;

        if (accepted)
        {
            result =
                "Partial text updated";

            decision =
                "ACCEPT";
        }
        else
        {
            result =
                "Mapping conflict";

            decision =
                "REJECT";
        }

        log << setw(25)
            << observation;

        log << setw(25)
            << possible;

        log << setw(25)
            << tested;

        log << setw(20)
            << result;

        log << setw(15)
            << decision;

        log << endl;

        display_partial_plaintext(
            ciphertext,
            cipherToPlain,
            log);

        step++;
    }

    // --------------------------------------------------------
    // Save recovered plaintext
    // --------------------------------------------------------

    string recovered =
        apply_substitution(
            ciphertext,
            cipherToPlain);

    writeTextFile(
        "output/recovered_plaintext.txt",
        recovered);

    // --------------------------------------------------------
    // Save partial key
    // --------------------------------------------------------

    string recoveredKey;

    for (char c = 'A'; c <= 'Z'; c++)
    {
        if (cipherToPlain.find(c)
            != cipherToPlain.end())
        {
            recoveredKey +=
                cipherToPlain[c];
        }
        else
        {
            recoveredKey += '?';
        }
    }

    writeTextFile(
        "output/recovered_key.txt",
        recoveredKey);

    cout << "\n=============================================\n";
    cout << "        CRYPTANALYSIS COMPLETED\n";
    cout << "=============================================\n";

    cout << "\nRecovered plaintext:\n";
    cout << recovered << endl;

    cout << "\nPartial recovered key:\n";
    cout << recoveredKey << endl;

    log.close();
}


// ============================================================
// 6. VERIFY SOLUTION
// ============================================================

bool verify_solution(
    const string& plaintext,
    const string& ciphertext,
    const string& key)
{
    cout << "\n=============================================\n";
    cout << "                 VERIFICATION\n";
    cout << "=============================================\n";

    string regenerated =
        encrypt_text(
            plaintext,
            key);

    if (toUpperCase(regenerated) ==
        toUpperCase(ciphertext))
    {
        cout << "Verification SUCCESSFUL!\n";
        cout << "Re-encrypted plaintext matches "
                "the ciphertext.\n";

        return true;
    }

    cout << "Verification FAILED!\n";

    return false;
}


// ============================================================
// RECOVER ORIGINAL KEY
// ============================================================

string recover_key(
    const string& plaintext,
    const string& ciphertext)
{
    string key(26, '?');

    for (size_t i = 0;
         i < plaintext.length() &&
         i < ciphertext.length();
         i++)
    {
        if (isalpha(
                static_cast<unsigned char>(
                    plaintext[i])) &&
            isalpha(
                static_cast<unsigned char>(
                    ciphertext[i])))
        {
            char p =
                toupper(
                    static_cast<unsigned char>(
                        plaintext[i]));

            char c =
                toupper(
                    static_cast<unsigned char>(
                        ciphertext[i]));

            key[p - 'A'] = c;
        }
    }

    return key;
}


// ============================================================
// DISPLAY KEY
// ============================================================

void display_encryption_key(
    const string& key)
{
    cout << "\nRecovered encryption key:\n";

    cout << "Plain : ";

    for (char c = 'A'; c <= 'Z'; c++)
    {
        cout << c << " ";
    }

    cout << "\nCipher: ";

    for (char c : key)
    {
        cout << c << " ";
    }

    cout << endl;
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    cout << "====================================================\n";
    cout << " MONOALPHABETIC SUBSTITUTION CIPHER\n";
    cout << " FREQUENCY AND PATTERN ANALYSIS\n";
    cout << "====================================================\n";

    // --------------------------------------------------------
    // Input files
    // --------------------------------------------------------

    const string plaintextFile =
        "plaintext.txt";

    const string keyFile =
        "key.txt";

    // --------------------------------------------------------
    // Read plaintext
    // --------------------------------------------------------

    string plaintext =
        readTextFile(
            plaintextFile);

    // --------------------------------------------------------
    // Read key
    // --------------------------------------------------------

    string key =
        readKeyFile(
            keyFile);

    cout << "\nFiles loaded successfully.\n";

    // --------------------------------------------------------
    // Generate ciphertext
    // --------------------------------------------------------

    string ciphertext =
        encrypt_text(
            plaintext,
            key);

    cout << "\nCiphertext generated.\n";

    writeTextFile(
        "output/ciphertext.txt",
        ciphertext);

    // --------------------------------------------------------
    // Display ciphertext
    // --------------------------------------------------------

    cout << "\n=============================================\n";
    cout << "                 CIPHERTEXT\n";
    cout << "=============================================\n";

    cout << ciphertext << endl;

    // --------------------------------------------------------
    // 1. Frequency analysis
    // --------------------------------------------------------

    frequency_analysis(
        ciphertext);

    // --------------------------------------------------------
    // 2. Word frequency analysis
    // --------------------------------------------------------

    word_frequency_analysis(
        ciphertext);

    // --------------------------------------------------------
    // 3. Pattern analysis
    // --------------------------------------------------------

    pattern_analysis(
        ciphertext);

    // --------------------------------------------------------
    // 4,5. Iterative cryptanalysis
    // --------------------------------------------------------

    iterative_cryptanalysis(
        ciphertext);

    // --------------------------------------------------------
    // Recover actual key
    //
    // This is ONLY used for validation because the original
    // plaintext is known to us during the encryption experiment.
    // It is NOT used for cryptanalysis.
    // --------------------------------------------------------

    string recoveredKey =
        recover_key(
            plaintext,
            ciphertext);

    display_encryption_key(
        recoveredKey);

    writeTextFile(
        "output/recovered_key.txt",
        recoveredKey);

    // --------------------------------------------------------
    // 6. Verify
    // --------------------------------------------------------

    verify_solution(
        plaintext,
        ciphertext,
        recoveredKey);

    cout << "\n====================================================\n";
    cout << "                 PROGRAM FINISHED\n";
    cout << "====================================================\n";

    cout << "\nCheck the output folder for:\n";
    cout << "1. ciphertext.txt\n";
    cout << "2. frequency_analysis.txt\n";
    cout << "3. word_analysis.txt\n";
    cout << "4. pattern_analysis.txt\n";
    cout << "5. cryptanalysis_log.txt\n";
    cout << "6. recovered_plaintext.txt\n";
    cout << "7. recovered_key.txt\n";

    return 0;
}

