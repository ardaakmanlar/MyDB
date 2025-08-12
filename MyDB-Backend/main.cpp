#include "./index/BPlusTree.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    BPlusTree tree;

    // Test verileri (split ve ba�lant� zincirini test etmek i�in kar���k s�ra)
    vector<pair<int, vector<string>>> testData = {
        {10, {"A"}},
        {20, {"B"}},
        {5, {"C"}},
        {15, {"D"}},
        {25, {"E"}},
        {30, {"F"}},
        {1, {"G"}},
        {12, {"H"}},
        {18, {"I"}},
        {35, {"J"}}
    };

    for (auto& data : testData) {
        cout << "\n>>> Inserting key: " << data.first << endl;
        tree.insert(data);
        tree.print(); // Yaprak zincirini g�ster
    }

    return 0;
}

