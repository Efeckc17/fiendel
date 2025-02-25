#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <string>
#include <random>
#include <sstream>
#include <cerrno>
#include <sys/stat.h>
#include <chrono>
#include <thread>
#ifdef _WIN32
#include <windows.h>
#define PATH_SEPARATOR "\\"
#else
#include <unistd.h>
#define PATH_SEPARATOR "/"
#endif

using namespace std;

long fSize(const string &file) {
    struct stat st;
    if (stat(file.c_str(), &st) != 0)
        return -1;
    return st.st_size;
}

string randStr(size_t len) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string res;
    default_random_engine rng(random_device{}());
    uniform_int_distribution<> dist(0, sizeof(charset) - 2);
    for (size_t i = 0; i < len; ++i)
        res += charset[dist(rng)];
    return res;
}

bool overPattern(const string &file, char pat, int passes) {
    long size = fSize(file);
    if (size <= 0) {
        cerr << "File size error: " << file << endl;
        return false;
    }
    fstream f(file, ios::binary | ios::in | ios::out);
    if (!f.is_open()) {
        cerr << "Cannot open file: " << file << endl;
        return false;
    }
    for (int p = 1; p <= passes; ++p) {
        f.seekp(0, ios::beg);
        for (long i = 0; i < size; ++i)
            f.put(pat);
        f.flush();
        cout << "Pattern pass " << p << " (0x" 
             << hex << (int)(unsigned char)pat << dec 
             << ") done." << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
    f.close();
    return true;
}

bool overRandom(const string &file, int passes) {
    long size = fSize(file);
    if (size <= 0) {
        cerr << "File size error: " << file << endl;
        return false;
    }
    fstream f(file, ios::binary | ios::in | ios::out);
    if (!f.is_open()) {
        cerr << "Cannot open file: " << file << endl;
        return false;
    }
    srand(static_cast<unsigned>(time(nullptr)));
    for (int p = 1; p <= passes; ++p) {
        f.seekp(0, ios::beg);
        for (long i = 0; i < size; ++i)
            f.put(static_cast<char>(rand() % 256));
        f.flush();
        cout << "Random pass " << p << " done." << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
    f.close();
    return true;
}

bool chgExt(string &file, int times) {
    for (int i = 1; i <= times; ++i) {
        string dir, base;
        size_t pos = file.find_last_of("/\\");
        if (pos != string::npos) {
            dir = file.substr(0, pos + 1);
            base = file.substr(pos + 1);
        } else {
            dir = "";
            base = file;
        }
        size_t dotPos = base.find_last_of(".");
        if (dotPos != string::npos)
            base = base.substr(0, dotPos);
        string newExt = "." + randStr(3);
        string newName = dir + base + newExt;
        if (rename(file.c_str(), newName.c_str()) != 0) {
            cerr << "Extension change failed: " << strerror(errno) << endl;
            return false;
        }
        file = newName;
        cout << "Extension change " << i << " done: " << newName << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
    return true;
}

bool rmFile(const string &file) {
    if (remove(file.c_str()) != 0) {
        cerr << "Deletion failed: " << strerror(errno) << endl;
        return false;
    }
    cout << "File permanently removed." << endl;
    return true;
}

int main() {
    string file;
    cout << "WARNING: This program will PERMANENTLY DELETE the specified file." << endl;
    cout << "All data will be irrecoverable. Use at your own risk." << endl;
    cout << "Enter file path: ";
    getline(cin, file);
    cout << "\nAre you sure? (Y/N): ";
    char ch;
    cin >> ch;
    if (ch != 'Y' && ch != 'y') {
        cout << "Operation canceled." << endl;
        return 0;
    }
    cout << "Starting deletion process..." << endl;
    this_thread::sleep_for(chrono::seconds(1));
    if (!overPattern(file, 0xFF, 1)) return 1;
    if (!overPattern(file, 0x00, 1)) return 1;
    if (!overRandom(file, 5)) return 1;
    if (!overPattern(file, 0xAA, 2)) return 1;
    if (!chgExt(file, 4)) return 1;
    if (!rmFile(file)) return 1;
    cout << "Deletion completed successfully." << endl;
    return 0;
}
