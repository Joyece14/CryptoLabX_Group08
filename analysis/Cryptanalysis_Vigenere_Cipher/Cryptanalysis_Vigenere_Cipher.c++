#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

// English letter frequencies
const double ENGLISH_FREQ[26] = {
    0.08167, 0.01492, 0.02782, 0.04253, 0.12702,
    0.02228, 0.02015, 0.06094, 0.06966, 0.00153,
    0.00772, 0.04025, 0.02406, 0.06749, 0.07507,
    0.01929, 0.00095, 0.05987, 0.06327, 0.09056,
    0.02758, 0.00978, 0.02360, 0.00150, 0.01974,
    0.00074
};

// ============================================================
// 1. clean_ciphertext()
// Remove spaces, special characters and convert to uppercase
// ============================================================

string clean_ciphertext(const string& ciphertext) {
    string cleaned;

    for (char c : ciphertext) {
        if (isalpha(static_cast<unsigned char>(c))) {
            cleaned += toupper(static_cast<unsigned char>(c));
        }
    }

    return cleaned;
}

// ============================================================
// 2. find_repeated_patterns()
// Find repeated sequences of length 3 to 5
// ============================================================

map<string, vector<int>> find_repeated_patterns(
    const string& ciphertext,
    int minLen = 3,
    int maxLen = 5
) {
    map<string, vector<int>> patterns;

    for (int len = minLen; len <= maxLen; len++) {
        for (int i = 0; i <= (int)ciphertext.length() - len; i++) {
            string pattern = ciphertext.substr(i, len);
            patterns[pattern].push_back(i);
        }
    }

    // Remove patterns that occur only once
    map<string, vector<int>> repeated;

    for (auto& entry : patterns) {
        if (entry.second.size() >= 2) {
            repeated[entry.first] = entry.second;
        }
    }

    return repeated;
}

// ============================================================
// 3. calculate_distances()
// Calculate distances between repeated occurrences
// ============================================================

map<string, vector<int>> calculate_distances(
    const map<string, vector<int>>& repeatedPatterns
) {
    map<string, vector<int>> distances;

    for (auto& entry : repeatedPatterns) {

        const string& pattern = entry.first;
        const vector<int>& positions = entry.second;

        vector<int> d;

        for (int i = 0; i < (int)positions.size() - 1; i++) {
            d.push_back(positions[i + 1] - positions[i]);
        }

        distances[pattern] = d;
    }

    return distances;
}

// ============================================================
// 4. find_factors()
// Find factors of a distance
// ============================================================

vector<int> find_factors(int number) {
    vector<int> factors;

    for (int i = 2; i <= number && i <= 30; i++) {
        if (number % i == 0) {
            factors.push_back(i);
        }
    }

    return factors;
}

// ============================================================
// 5. kasiski_analysis()
// Use repeated patterns and distances to suggest key lengths
// ============================================================

vector<int> kasiski_analysis(const string& ciphertext) {

    auto repeated = find_repeated_patterns(ciphertext);
    auto distances = calculate_distances(repeated);

    map<int, int> factorCount;

    cout << "\n========== KASISKI ANALYSIS ==========\n";

    cout << "\nRepeated Patterns:\n";

    for (auto& entry : repeated) {
        cout << entry.first << " -> ";

        for (int position : entry.second) {
            cout << position << " ";
        }

        cout << "\n";
    }

    cout << "\nDistances and Factors:\n";

    for (auto& entry : distances) {

        cout << entry.first << " -> ";

        for (int distance : entry.second) {

            cout << distance << " (";

            vector<int> factors = find_factors(distance);

            for (int factor : factors) {
                cout << factor << " ";
                factorCount[factor]++;
            }

            cout << ") ";
        }

        cout << "\n";
    }

    vector<pair<int, int>> sortedFactors(
        factorCount.begin(),
        factorCount.end()
    );

    sort(
        sortedFactors.begin(),
        sortedFactors.end(),
        [](const pair<int, int>& a,
           const pair<int, int>& b) {
            return a.second > b.second;
        }
    );

    cout << "\nCandidate Key Lengths:\n";

    vector<int> candidates;

    for (auto& entry : sortedFactors) {

        if (entry.first >= 2 && entry.first <= 20) {

            cout << "Length "
                 << entry.first
                 << " : "
                 << entry.second
                 << " occurrences\n";

            candidates.push_back(entry.first);
        }
    }

    return candidates;
}

// ============================================================
// 6. calculate_ic()
// Calculate Index of Coincidence
// ============================================================

double calculate_ic(const string& text) {

    int n = text.length();

    if (n <= 1)
        return 0.0;

    int frequency[26] = {0};

    for (char c : text) {
        frequency[c - 'A']++;
    }

    double numerator = 0;

    for (int i = 0; i < 26; i++) {
        numerator += frequency[i] * (frequency[i] - 1);
    }

    return numerator / (n * (n - 1));
}

// ============================================================
// 7. split_into_groups()
// Divide ciphertext according to key length
// ============================================================

vector<string> split_into_groups(
    const string& ciphertext,
    int keyLength
) {
    vector<string> groups(keyLength);

    for (int i = 0; i < (int)ciphertext.length(); i++) {
        groups[i % keyLength] += ciphertext[i];
    }

    return groups;
}

// ============================================================
// 8. frequency_analysis()
// Calculate A-Z frequency for each group
// ============================================================

vector<int> frequency_analysis(const string& group) {

    vector<int> frequency(26, 0);

    for (char c : group) {
        frequency[c - 'A']++;
    }

    return frequency;
}

// ============================================================
// Display frequency tables
// ============================================================

void display_frequency_tables(
    const vector<string>& groups
) {

    cout << "\n========== FREQUENCY ANALYSIS ==========\n";

    for (int i = 0; i < (int)groups.size(); i++) {

        vector<int> frequency =
            frequency_analysis(groups[i]);

        cout << "\nGroup " << i + 1 << "\n";
        cout << "Text: " << groups[i] << "\n";

        cout << "-----------------------------------\n";
        cout << "Letter\tCount\tPercentage\n";
        cout << "-----------------------------------\n";

        for (int j = 0; j < 26; j++) {

            double percentage =
                (double)frequency[j] /
                groups[i].length() * 100;

            cout << char('A' + j)
                 << "\t"
                 << frequency[j]
                 << "\t"
                 << fixed
                 << setprecision(2)
                 << percentage
                 << "%\n";
        }
    }
}

// ============================================================
// 9. find_shift()
// Estimate Caesar shift using chi-square
// ============================================================

pair<int, double> find_shift(
    const string& group
) {

    int n = group.length();

    int bestShift = 0;
    double bestScore = 1e100;

    for (int shift = 0; shift < 26; shift++) {

        int frequency[26] = {0};

        // Decrypt using this possible shift
        for (char c : group) {

            int value =
                (c - 'A' - shift + 26) % 26;

            frequency[value]++;
        }

        double chiSquare = 0.0;

        for (int i = 0; i < 26; i++) {

            double expected =
                ENGLISH_FREQ[i] * n;

            if (expected > 0) {

                chiSquare +=
                    ((frequency[i] - expected) *
                     (frequency[i] - expected))
                    / expected;
            }
        }

        if (chiSquare < bestScore) {

            bestScore = chiSquare;
            bestShift = shift;
        }
    }

    return {bestShift, bestScore};
}

// ============================================================
// 10. find_key()
// Combine shifts to obtain probable Vigenere key
// ============================================================

string find_key(
    const vector<string>& groups
) {

    string key;

    cout << "\n========== KEY RECOVERY ==========\n";

    cout << "Group\tShift\tKey Letter\tChi-Square\n";
    cout << "---------------------------------------------\n";

    for (int i = 0; i < (int)groups.size(); i++) {

        pair<int, double> result =
            find_shift(groups[i]);

        int shift = result.first;
        double score = result.second;

        char keyLetter = 'A' + shift;

        key += keyLetter;

        cout << i + 1
             << "\t"
             << shift
             << "\t"
             << keyLetter
             << "\t\t"
             << fixed
             << setprecision(3)
             << score
             << "\n";
    }

    return key;
}

// ============================================================
// 11. vigenere_decrypt()
// ============================================================

string vigenere_decrypt(
    const string& ciphertext,
    const string& key
) {

    string plaintext;

    for (int i = 0; i < (int)ciphertext.length(); i++) {

        int cipherValue =
            ciphertext[i] - 'A';

        int keyValue =
            key[i % key.length()] - 'A';

        int plainValue =
            (cipherValue - keyValue + 26) % 26;

        plaintext +=
            char('A' + plainValue);
    }

    return plaintext;
}

// ============================================================
// 12. vigenere_encrypt()
// ============================================================

string vigenere_encrypt(
    const string& plaintext,
    const string& key
) {

    string ciphertext;

    for (int i = 0; i < (int)plaintext.length(); i++) {

        int plainValue =
            plaintext[i] - 'A';

        int keyValue =
            key[i % key.length()] - 'A';

        int cipherValue =
            (plainValue + keyValue) % 26;

        ciphertext +=
            char('A' + cipherValue);
    }

    return ciphertext;
}

// ============================================================
// 13. verify()
// ============================================================

bool verify(
    const string& originalCiphertext,
    const string& encryptedCiphertext
) {

    return originalCiphertext ==
           encryptedCiphertext;
}

// ============================================================
// Estimate key length using Index of Coincidence
// ============================================================

pair<int, double> estimate_key_length(
    const string& ciphertext
) {

    cout << "\n========== INDEX OF COINCIDENCE ==========\n";

    cout << "Length\tAverage IC\n";
    cout << "-------------------------\n";

    int bestLength = 2;
    double bestIC = 0.0;

    for (int length = 2; length <= 20; length++) {

        vector<string> groups =
            split_into_groups(ciphertext, length);

        double totalIC = 0.0;
        int count = 0;

        for (const string& group : groups) {

            if (group.length() > 1) {

                totalIC +=
                    calculate_ic(group);

                count++;
            }
        }

        double averageIC =
            (count > 0) ? totalIC / count : 0;

        cout << length
             << "\t"
             << fixed
             << setprecision(6)
             << averageIC
             << "\n";

        if (averageIC > bestIC) {

            bestIC = averageIC;
            bestLength = length;
        }
    }

    return {bestLength, bestIC};
}

// ============================================================
// MAIN
// ============================================================

int main() {

    string ciphertext = R"(
QRBAI UWYOK ILBRZ XTUWL EGXSN VDXWR XMHXY FCGMW
WWSME LSXUZ
MKMFS BNZIF YEIEG RFZRX WKUFA XQEDX DTTHY NTBRJ
LHTAI KOCZX
QHBND ZIGZG PXARJ EDYSJ NUMKI FLBTN HWISW NVLFM
EGXAI AAWSL
FMHXR SGRIG HEQTU MLGLV BRSIL AEZSG XCMHT OWHFM
LWMRK HPRFB
ELWGF RUGPB HNBEM KBNVW HHUEA KILBN BMLHK XUGML
YQKHP RFBEL
EJYNV WSIJB GAXGO TPMXR TXFKI WUALB RGWIE GHWHG
AMEWW LTAEL
NUMRE UWTBL SDPRL YVRET LEEDF ROBEQ UXTHX ZYOZB
XLKAC KSOHN
VWXKS MAEPH IYQMM FSECH RFYPB BSQTX TPIWH GPXQD
FWTAI KNNBX
SIYKE TXTLV BTMQA LAGHG OTPMX RTXTH XSFYG WMVKH
LOIVU ALMLD
LTSYV WYNVW MQVXP XRVYA BLXDL XSMLW SUIOI IMELI
SOYEB HPHNR
WTVUI AKEYG WIETG WWBVM VDUMA EPAUA KXWHK MAUPA
MUKHQ PWKCX
EFXGW WSDDE OMLWL NKMWD FWTAM FAFEA MFZBN WIHYA
LXRWK MAMIK
GNGHJ UAZHM HGUAL YSULA ELYHJ BZMSI LAILH WWYIK
EWAHN PMLBN
NBVPJ XLBEF WRWGX KWIRH XWWGQ HRRXW IOMFY CZHZL
VXNVI OYZCM
YDDEY IPWXT MMSHS VHHXZ YEWNV OAOEL SMLSW KXXFX
STRVI HZLEF
JXDAS FIE
)";

    // --------------------------------------------------------
    // Step 1: Preprocess
    // --------------------------------------------------------

    ciphertext = clean_ciphertext(ciphertext);

    cout << "============================================\n";
    cout << " VIGENERE CIPHER CRYPTANALYSIS\n";
    cout << "============================================\n";

    cout << "\nCleaned ciphertext:\n";
    cout << ciphertext << "\n";

    cout << "\nCiphertext length: "
         << ciphertext.length()
         << "\n";

    // --------------------------------------------------------
    // Step 2: Kasiski Test
    // --------------------------------------------------------

    vector<int> candidates =
        kasiski_analysis(ciphertext);

    // --------------------------------------------------------
    // Step 3: IC
    // --------------------------------------------------------

    pair<int, double> icResult =
        estimate_key_length(ciphertext);

    int keyLength = icResult.first;

    cout << "\n============================================\n";
    cout << "ESTIMATED KEY LENGTH\n";
    cout << "============================================\n";

    cout << "Estimated key length: "
         << keyLength << "\n";

    cout << "Average IC: "
         << fixed
         << setprecision(6)
         << icResult.second
         << "\n";

    // --------------------------------------------------------
    // Step 4: Split into groups
    // --------------------------------------------------------

    vector<string> groups =
        split_into_groups(ciphertext, keyLength);

    cout << "\n========== CIPHERTEXT GROUPS ==========\n";

    for (int i = 0; i < (int)groups.size(); i++) {

        cout << "Group "
             << i + 1
             << ": "
             << groups[i]
             << "\n";
    }

    // --------------------------------------------------------
    // Step 5: Frequency Analysis
    // --------------------------------------------------------

    display_frequency_tables(groups);

    // --------------------------------------------------------
    // Step 6: Recover Key
    // --------------------------------------------------------

    string recoveredKey =
        find_key(groups);

    cout << "\n============================================\n";
    cout << "RECOVERED KEY\n";
    cout << "============================================\n";

    cout << "Key: "
         << recoveredKey
         << "\n";

    // --------------------------------------------------------
    // Step 7: Decrypt
    // --------------------------------------------------------

    string plaintext =
        vigenere_decrypt(
            ciphertext,
            recoveredKey
        );

    cout << "\n============================================\n";
    cout << "RECOVERED PLAINTEXT\n";
    cout << "============================================\n";

    for (int i = 0;
         i < (int)plaintext.length();
         i += 80) {

        cout << plaintext.substr(i, 80)
             << "\n";
    }

    // --------------------------------------------------------
    // Step 8: Re-encrypt
    // --------------------------------------------------------

    string reEncrypted =
        vigenere_encrypt(
            plaintext,
            recoveredKey
        );

    // --------------------------------------------------------
    // Step 9: Verify
    // --------------------------------------------------------

    bool correct =
        verify(
            ciphertext,
            reEncrypted
        );

    cout << "\n============================================\n";
    cout << "VERIFICATION\n";
    cout << "============================================\n";

    cout << "Re-encrypted ciphertext matches "
            "original: ";

    if (correct) {
        cout << "TRUE\n";
        cout << "Verification successful!\n";
    }
    else {
        cout << "FALSE\n";
        cout << "Verification failed!\n";
    }

    return 0;
}

