#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "../index/BPlusTree.h"

class Table{
private:
	std::string name;
	BPlusTree tree;
};

