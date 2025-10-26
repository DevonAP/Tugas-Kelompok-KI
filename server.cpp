#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include "DES.h"   // pastikan file ini sudah kamu punya dan berisi fungsi des_encrypt() & des_decrypt()

using namespace std;

// Konversi dari bit (vector<int>) ke byte (vector<uint8_t>)
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
    for (uint8_t byte : bytes) {
        for (int i = 7; i >= 0; --i)
            bits.push_back((byte >> i) & 1);
    }
    return bits;
}

int main() {
    const string key = "81384935"; // Kunci DES 8 karakter

    // --- Inisialisasi Winsock ---
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cerr << "Gagal inisialisasi Winsock!" << endl;
        return 1;
    }

    // --- Buat socket ---
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Gagal membuat socket!" << endl;
        WSACleanup();
        return 1;
    }

    // --- Siapkan alamat server ---
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(9999);

    // --- Bind ---
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind gagal!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // --- Listen ---
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        cerr << "Listen gagal!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Menunggu koneksi di port 9999..." << endl;

    // --- Terima koneksi dari client ---
    sockaddr_in clientAddr{};
    int clientSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Gagal menerima koneksi!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Client terhubung!" << endl;

    // --- Terima pesan dari client ---
    vector<uint8_t> buffer(4096);
    int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0);

    if (bytesReceived > 0) {
        buffer.resize(bytesReceived);

        cout << "Pesan diterima (terenkripsi, " << bytesReceived << " byte):" << endl;
        for (int i = 0; i < bytesReceived; ++i)
            cout << hex << (int)buffer[i] << " ";
        cout << dec << endl;

        // Ambil data yang diterima (byte)
        vector<uint8_t> receivedBytes(buffer.begin(), buffer.begin() + bytesReceived);

        // Konversi byte → bit
        vector<int> cipherbits = bytes_to_bits(receivedBytes);

        // Baru decrypt
        string decrypted = des_decrypt(cipherbits, key);
        cout << "Setelah dekripsi: " << decrypted << endl;
    } else {
        cerr << "Gagal menerima data dari client!" << endl;
    }

    // --- Kirim balasan terenkripsi ---
    string reply = "HI JUGAA";
    vector<int> encryptedReply = des_encrypt(reply, key);
    vector<uint8_t> replyBytes = bits_to_bytes(encryptedReply);
    cout << "Size:" << replyBytes.size() << endl;

    ssize_t sent = send(
        clientSocket,
        reinterpret_cast<const char*>(replyBytes.data()),
        replyBytes.size(),  // total bytes
        0
    );

    if (sent == SOCKET_ERROR)
        cerr << "Gagal mengirim balasan!" << endl;
    else
        cout << "Balasan terenkripsi dikirim ke client (" << sent << " byte)." << endl;

    // --- Tutup koneksi ---
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
