#include "./index/BPlusTree.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
using namespace std;

static pair<int, vector<string>> make_val(int k) {
    vector<string> v; v.push_back(to_string(k));
    return { k, v };
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    BPlusTree t;

    const int N1 = 50000;
    const int N2 = 50000;
    const int PRINT_EVERY = 10000;

    auto t0 = chrono::high_resolution_clock::now();

    for (int i = 1; i <= N1; ++i) {
        t.insert(make_val(i));
        if (i % PRINT_EVERY == 0) {
            cout << "Inserted (seq): " << i << "\n";
        }
    }

    vector<int> keys;
    keys.reserve(N2);
    for (int i = N1 + 1; i <= N1 + N2; ++i) keys.push_back(i);
    mt19937 rng(1234567u);
    shuffle(keys.begin(), keys.end(), rng);

    int c = 0;
    for (int k : keys) {
        t.insert(make_val(k));
        ++c;
        if (c % PRINT_EVERY == 0) {
            cout << "Inserted (rand): " << c << "\n";
        }
    }

    auto t1 = chrono::high_resolution_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count();


    cout << "\nTotal inserted: " << (N1 + N2) << "\n";
    cout << "Elapsed ms: " << ms << "\n";

    return 0;
}
