#include "Table.h"
#include <cctype>

static bool isInteger(const std::string& s) {
    if (s.empty()) return false;
    size_t i = (s[0] == '+' || s[0] == '-') ? 1 : 0;
    if (i == s.size()) return false;
    for (; i < s.size(); ++i) if (!std::isdigit((unsigned char)s[i])) return false;
    return true;
}
static bool isFloating(const std::string& s) {
    if (s.empty()) return false;
    bool dot = false, digit = false;
    size_t i = (s[0] == '+' || s[0] == '-') ? 1 : 0;
    for (; i < s.size(); ++i) {
        if (std::isdigit((unsigned char)s[i])) { digit = true; continue; }
        if (s[i] == '.' && !dot) { dot = true; continue; }
        return false;
    }
    return digit;
}
static bool isBool(const std::string& s) {
    return s == "true" || s == "false" || s == "0" || s == "1";
}

Table::Table(std::string name_,
    std::vector<std::pair<std::string, std::string>> attributes)
    : name(std::move(name_)), Attributes(std::move(attributes)) {
}

bool Table::validateRowShape(const std::vector<std::string>& row) const {
    return row.size() == Attributes.size();
}

bool Table::validateValueByType(const std::string& type, const std::string& value) const {
    if (type == "string" || type == "STRING" || type == "text" || type == "TEXT") return true;
    if (type == "int" || type == "INT")    return isInteger(value);
    if (type == "float" || type == "double" || type == "FLOAT" || type == "DOUBLE") return isFloating(value);
    if (type == "bool" || type == "BOOL")   return isBool(value);
    return true; 
}

int Table::columnIndex(const std::string& column) const {
    for (int i = 0; i < (int)Attributes.size(); ++i)
        if (Attributes[i].first == column) return i;
    return -1;
}

bool Table::insertRow(int key, const std::vector<std::string>& row) {
    if (!validateRowShape(row)) return false;
    for (size_t i = 0; i < row.size(); ++i)
        if (!validateValueByType(Attributes[i].second, row[i])) return false;
    return tree.insert({ key, row });
}

std::optional<std::vector<std::string>> Table::getRow(int key) const {
    const auto& ref = tree.search(key);
    if (ref.first == -1) return std::nullopt;
    return ref.second;
}
