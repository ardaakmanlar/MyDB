#include "Table.h"
#include <iostream>
using namespace std;

Table::Table(std::string name, std::vector<std::string> attributes)
    : name(std::move(name)), attributes(std::move(attributes)), tree() {
}

bool Table::addRow(std::vector<std::string> row) {
    static int autoId = 0;
    int id = ++autoId;
    if (tree.search(id).first != -1) return false;
    return tree.insert({ id, std::move(row) });
}

bool Table::addRow(int id, std::vector<std::string> row) {
    static int autoId = 0;
    if (id <= 0) id = ++autoId; else if (id > autoId) autoId = id;
    if (tree.search(id).first != -1) return false;
    return tree.insert({ id, std::move(row) });
}
void Table::printTable(int n) const {
    auto rows = tree.collect(n);

    size_t colCount = 1 + attributes.size();
    vector<size_t> widths(colCount, 0);

    widths[0] = max<size_t>(2, string("id").size());
    for (auto& kv : rows) {
        widths[0] = max(widths[0], to_string(kv.first).size());
    }

    for (size_t c = 0; c < attributes.size(); ++c) {
        widths[c + 1] = attributes[c].size();
        for (auto& kv : rows) {
            if (c < kv.second.size()) {
                widths[c + 1] = max(widths[c + 1], kv.second[c].size());
            }
        }
    }

    cout << "| " << left << setw((int)widths[0]) << "id" << " ";
    for (size_t c = 0; c < attributes.size(); ++c) {
        cout << "| " << left << setw((int)widths[c + 1]) << attributes[c] << " ";
    }
    cout << "|\n";

    for (auto& kv : rows) {
        cout << "| " << left << setw((int)widths[0]) << kv.first << " ";
        for (size_t c = 0; c < attributes.size(); ++c) {
            string val = (c < kv.second.size() ? kv.second[c] : "");
            cout << "| " << left << setw((int)widths[c + 1]) << val << " ";
        }
        cout << "|\n";
    }
}

void Table::addAttribute(std::string attribute) {
    attributes.emplace_back(std::move(attribute));
}

std::vector<std::string> Table::search(int id) const {
    const auto& r = tree.search(id);
    if (r.first == -1) return {};
    return r.second;
}

bool Table::update(int id, std::vector<std::string> newRow) {
    return tree.update(id, newRow, -1);
}

bool Table::deleteRow(int id) {
    return tree.remove(id);
}
