#pragma once
#include <string>
#include <vector>
#include <utility>
#include "BPlusTree.h"
#include <iomanip> 

class Table {
private:
    std::string name;
    std::vector<std::string> attributes;   
    BPlusTree tree;

public:
    Table(std::string name, std::vector<std::string> attributes);

    bool addRow(std::vector<std::string> row);
    bool addRow(int id, std::vector<std::string> row);

    void printTable(int n = 10) const;
    void addAttribute(std::string attribute);           
    std::vector<std::string> search(int id) const;
    bool update(int id, std::vector<std::string> newRow);
    bool deleteRow(int id);
};
