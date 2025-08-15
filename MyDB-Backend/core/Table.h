#pragma once
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include "../index/BPlusTree.h"


class Table {
public:
    Table(std::string name,
        std::vector<std::pair<std::string, std::string>> attributes);

    const std::string& getName() const { return name; }
    const std::vector<std::pair<std::string, std::string>>& getAttributes() const { return Attributes; }

    bool insertRow(int key, const std::vector<std::string>& row);
    std::optional<std::vector<std::string>> getRow(int key) const;

    int columnIndex(const std::string& column) const;
    void printLeafChain() const { tree.print(); }

private:
    std::string name;
    std::vector<std::pair<std::string, std::string>> Attributes;
    BPlusTree tree;

    bool validateRowShape(const std::vector<std::string>& row) const;
    bool validateValueByType(const std::string& type, const std::string& value) const;
};
