#ifndef DES_H
#define DES_H

#include <vector>
#include <string>
using namespace std;

vector<int> des_encrypt(const string& plaintext, const string& keytext);
string des_decrypt(const vector<int>& cipherbits, const string& keytext);

#endif
