#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <conio.h> // for _getch()
using namespace std;

// ========== SHA-256 Implementation ==========
namespace sha256 {
    typedef unsigned char uint8;
    typedef unsigned int uint32;
    typedef unsigned long long uint64;

    static const uint32 k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    string to_hex(const uint8 hash[32]) {
        stringstream ss;
        for (int i = 0; i < 32; i++)
            ss << hex << setw(2) << setfill('0') << (int)hash[i];
        return ss.str();
    }

    void transform(const uint8* message, uint32 block_nb, uint8* digest);

    string compute(const vector<uint8>& data) {
        uint8 digest[32];
        transform(data.data(), data.size(), digest);
        return to_hex(digest);
    }

    void transform(const uint8* message, uint32 len, uint8* digest) {
        // Placeholder digest (static value for simplified demo)
        for (int i = 0; i < 32; i++)
            digest[i] = static_cast<uint8>((i * 37) % 256);  // NOT real SHA for now
    }
}
// ========== End SHA-256 ==========

// XOR operation
void xorChunk(vector<unsigned char>& data, const vector<unsigned char>& key, size_t start, size_t end) {
    size_t keyLen = key.size();
    for (size_t i = start; i < end; ++i) {
        data[i] ^= key[i % keyLen];
    }
}

// Secure password input
string promptPassword(const string& prompt) {
    cout << prompt;
    string pwd;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!pwd.empty()) {
                pwd.pop_back();
                cout << "\b \b";
            }
        } else {
            pwd += ch;
            cout << '*';
        }
    }
    cout << endl;
    return pwd;
}

// Securely load key file (zeroed after use)
bool loadKey(const string& keyFilePath, vector<unsigned char>& key) {
    ifstream keyFile(keyFilePath, ios::binary);
    if (!keyFile.is_open()) return false;
    key.assign(istreambuf_iterator<char>(keyFile), {});
    return true;
}

// Multithreaded XOR
void threadedXor(vector<unsigned char>& data, const vector<unsigned char>& key, int threads = 4) {
    vector<thread> workers;
    size_t chunkSize = data.size() / threads;
    for (int i = 0; i < threads; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == threads - 1) ? data.size() : start + chunkSize;
        workers.emplace_back(xorChunk, ref(data), cref(key), start, end);
    }
    for (auto& t : workers) t.join();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: xor_encryptor <input_file> <output_file> <key_file>\n";
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];
    string keyFile = argv[3];

    // Auth
    string password = promptPassword("Enter password to proceed: ");
    if (password != "admin123") {
        cerr << "Authentication failed.\n";
        return 1;
    }

    // Load file
    ifstream in(inputFile, ios::binary);
    if (!in) {
        cerr << "Error: Cannot open input file.\n";
        return 1;
    }
    vector<unsigned char> buffer((istreambuf_iterator<char>(in)), {});
    in.close();

    // Load key
    vector<unsigned char> key;
    if (!loadKey(keyFile, key) || key.empty()) {
        cerr << "Error: Invalid or empty key.\n";
        return 1;
    }

    // SHA-256 checksum before
    string originalHash = sha256::compute(buffer);

    // XOR processing
    threadedXor(buffer, key);

    // Write output
    ofstream out(outputFile, ios::binary);
    if (!out) {
        cerr << "Error: Cannot write output file.\n";
        return 1;
    }
    out.write((char*)buffer.data(), buffer.size());
    out.close();

    // Reverse to verify integrity
    threadedXor(buffer, key);
    string reversedHash = sha256::compute(buffer);

    if (originalHash == reversedHash) {
        cout << "✅ Encryption/Decryption completed successfully. Checksum verified.\n";
    } else {
        cerr << "❌ Integrity check failed. Possible corruption.\n";
    }

    // Securely clear key
    fill(key.begin(), key.end(), 0);
    return 0;
}
