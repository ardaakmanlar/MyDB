#pragma once
#include "Schema.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "../index/BPlusTree.h"

class Table{
private:
	std::string name;
	std::vector<Row> rows;
	std::vector<Column> columns;

	std::unordered_map<std::string,BPlusTree> 
};

