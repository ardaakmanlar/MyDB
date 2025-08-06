#pragma once
#include <string>
#include <vector>

class Column {
public:
	std::string name;
	std::string type;

	Column(const std::string& _name, const std::string& _type);

};


class Row {
public:
	std::vector<std::string> values;

	Row(const std::vector<std::string>& _values);
};

