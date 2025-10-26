#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <bitset>
#include <cstdint>
#include <errno.h>

using namespace std;

// =========================
// 1️⃣ Tabel Permutasi dan S-Box
// =========================
int IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

int FP[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

int E[48] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
};

int P[32] = {
    16, 7, 20, 21, 29, 12, 28, 17,
    1, 15, 23, 26, 5, 18, 31, 10,
    2, 8, 24, 14, 32, 27, 3, 9,
    19, 13, 30, 6, 22, 11, 4, 25
};

int PC1[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
};

int PC2[48] = {
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

int SHIFT[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

int S_BOX[8][4][16] = {
    // S1
    {{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
     {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
     {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
     {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},
    // S2
    {{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
     {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
     {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
     {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},
    // S3
    {{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
     {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
     {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
     {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},
    // S4
    {{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
     {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
     {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
     {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},
    // S5
    {{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
     {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
     {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
     {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},
    // S6
    {{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
     {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
     {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
     {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},
    // S7
    {{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
     {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
     {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
     {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},
    // S8
    {{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
     {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
     {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
     {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}
};

// =========================
// 2️⃣ Fungsi Utility
// =========================
vector<int> permute(const vector<int>& block, int table[], int size) {
    vector<int> out(size);
    for (int i = 0; i < size; ++i) {
        out[i] = block[table[i]-1];
    }
    return out;
}

vector<int> shift_left(const vector<int>& block, int n) {
    vector<int> out(block.size());
    for (size_t i = 0; i < block.size(); ++i) {
        out[i] = block[(i+n)%block.size()];
    }
    return out;
}

vector<int> xor_bits(const vector<int>& a, const vector<int>& b) {
    vector<int> out(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        out[i] = a[i] ^ b[i];
    }
    return out;
}

vector<int> sbox_substitution(const vector<int>& block) {
    vector<int> output;
    for (int i = 0; i < 8; ++i) {
        vector<int> chunk(block.begin()+i*6, block.begin()+(i+1)*6);
        int row = (chunk[0]<<1) + chunk[5];
        int col = (chunk[1]<<3) + (chunk[2]<<2) + (chunk[3]<<1) + chunk[4];
        int val = S_BOX[i][row][col];
        for (int j = 3; j >= 0; --j) {
            output.push_back((val >> j) & 1);
        }
    }
    return output;
}

// =========================
// 3️⃣ Feistel Function
// =========================
vector<int> feistel(const vector<int>& right, const vector<int>& subkey) {
    vector<int> expanded = permute(right, E, 48);
    vector<int> xored = xor_bits(expanded, subkey);
    vector<int> substituted = sbox_substitution(xored);
    vector<int> permuted = permute(substituted, P, 32);
    return permuted;
}

// =========================
// 4️⃣ Key Schedule
// =========================
vector< vector<int> > generate_keys(const vector<int>& key64) {
    vector<int> key56 = permute(key64, PC1, 56);
    vector<int> C(key56.begin(), key56.begin()+28);
    vector<int> D(key56.begin()+28, key56.end());
    vector< vector<int> > subkeys;
    for (int shift : SHIFT) {
        C = shift_left(C, shift);
        D = shift_left(D, shift);
        vector<int> CD = C;
        CD.insert(CD.end(), D.begin(), D.end());
        vector<int> subkey48 = permute(CD, PC2, 48);
        subkeys.push_back(subkey48);
    }
    return subkeys;
}

// =========================
// 5️⃣ DES Core Functions
// =========================
vector<int> des_encrypt_block(const vector<int>& block64, const vector< vector<int> >& subkeys) {
    vector<int> block = permute(block64, IP, 64);
    vector<int> L(block.begin(), block.begin()+32);
    vector<int> R(block.begin()+32, block.end());
    for (const auto& k : subkeys) {
        vector<int> tempR = R;
        R = xor_bits(L, feistel(R, k));
        L = tempR;
    }
    vector<int> combined;
    combined.insert(combined.end(), R.begin(), R.end());
    combined.insert(combined.end(), L.begin(), L.end());
    return permute(combined, FP, 64);
}

vector<int> des_decrypt_block(const vector<int>& block64, const vector< vector<int> >& subkeys) {
    vector< vector<int> > rev_subkeys(subkeys.rbegin(), subkeys.rend());
    return des_encrypt_block(block64, rev_subkeys);
}

// =========================
// 6️⃣ Helper (Text <-> Bits) and Bits<->Bytes
// =========================
vector<int> text_to_bits(const string& text) {
    vector<int> bits;
    for (unsigned char c : text) {
        bitset<8> b(c);
        for (int i = 7; i >= 0; --i) bits.push_back(b[i]);
    }
    return bits;
}

string bits_to_text(const vector<int>& bits) {
    string text;
    for (size_t i = 0; i < bits.size(); i+=8) {
        int val = 0;
        for (int j = 0; j < 8; ++j) {
            val = (val << 1) | bits[i+j];
        }
        text += char(val);
    }
    return text;
}

vector<uint8_t> bits_to_bytes(const vector<int>& bits) {
    vector<uint8_t> bytes;
    size_t n = bits.size();
    if (n % 8 != 0) return bytes; // harus kelipatan 8
    for (size_t i = 0; i < n; i += 8) {
        int val = 0;
        for (int j = 0; j < 8; ++j) {
            val = (val << 1) | bits[i + j];
        }
        bytes.push_back(static_cast<uint8_t>(val));
    }
    return bytes;
}

vector<int> bytes_to_bits(const vector<uint8_t>& bytes) {
    vector<int> bits;
    for (uint8_t b : bytes) {
        for (int i = 7; i >= 0; --i) {
            bits.push_back((b >> i) & 1);
        }
    }
    return bits;
}

// =========================
// 7️⃣ Main DES Encrypt/Decrypt (operasi pada string <-> bits -> bytes untuk kirim)
// =========================
vector<int> des_encrypt(const string& plaintext, const string& keytext) {
    vector<int> keybits = text_to_bits(keytext.substr(0,8));
    vector< vector<int> > subkeys = generate_keys(keybits);

    int pad_len = 8 - (plaintext.size() % 8);
    string padded = plaintext + string(pad_len, char(pad_len));

    vector<int> plainbits = text_to_bits(padded);
    vector<int> cipherbits;
    for (size_t i = 0; i < plainbits.size(); i += 64) {
        vector<int> block(plainbits.begin()+i, plainbits.begin()+i+64);
        vector<int> encrypted = des_encrypt_block(block, subkeys);
        cipherbits.insert(cipherbits.end(), encrypted.begin(), encrypted.end());
    }
    return cipherbits;
}

string des_decrypt(const vector<int>& cipherbits, const string& keytext) {
    vector<int> keybits = text_to_bits(keytext.substr(0,8));
    vector< vector<int> > subkeys = generate_keys(keybits);
    vector<int> plainbits;
    for (size_t i = 0; i < cipherbits.size(); i += 64) {
        vector<int> block(cipherbits.begin()+i, cipherbits.begin()+i+64);
        vector<int> decrypted = des_decrypt_block(block, subkeys);
        plainbits.insert(plainbits.end(), decrypted.begin(), decrypted.end());
    }
    string plaintext = bits_to_text(plainbits);
    if (plaintext.empty()) return string();
    int pad_len = int((unsigned char)plaintext.back());
    if (pad_len <= 0 || pad_len > 8) return plaintext; // fallback safety
    return plaintext.substr(0, plaintext.size()-pad_len);
}

int main() {
    int sock = 0;
    struct sockaddr_in serverAddr;
    const string key = "81384935"; // Kunci DES 8 karakter

    // 1️⃣ Buat socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket gagal dibuat");
        return -1;
    }

    // 2️⃣ Siapkan alamat server (pastikan IP benar dan reachable)
    const char* server_ip = "192.168.137.1"; // ubah sesuai IP laptop/server kamu
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);

    if (inet_pton(AF_INET, server_ip, &serverAddr.sin_addr) <= 0) {
        perror("IP tidak valid");
        close(sock);
        return -1;
    }

    // 3️⃣ Coba koneksi ke server
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Koneksi gagal");
        close(sock);
        return -1;
    }

    cout << "Terhubung ke server.\n";

    // 4️⃣ Enkripsi pesan
    const char* msg = "HI TEMAN";
    vector<int> encrypted_bits = des_encrypt(string(msg), key);
    vector<uint8_t> encrypted_bytes = bits_to_bytes(encrypted_bits);

    // 5️⃣ Kirim ke server
    ssize_t sent = send(sock, encrypted_bytes.data(), encrypted_bytes.size(), 0);
    if (sent < 0) {
        perror("Gagal mengirim");
        close(sock);
        return -1;
    }
    cout << "Terkirim " << sent << " byte terenkripsi.\n";

    // 6️⃣ Terima balasan dari server
    vector<uint8_t> recvbuf(4096);
    ssize_t received = recv(sock, recvbuf.data(), recvbuf.size(), 0);
    if (received < 0) {
        perror("Gagal menerima");
        close(sock);
        return -1;
    }
    recvbuf.resize(received);
    cout << "Menerima " << received << " byte dari server.\n";

    // 7️⃣ Dekripsi balasan
    vector<int> recv_bits = bytes_to_bits(recvbuf);
    string decrypted = des_decrypt(recv_bits, key);
    cout << "Balasan server (setelah dekripsi): " << decrypted << endl;

    // 8️⃣ Tutup koneksi
    close(sock);
    return 0;
}