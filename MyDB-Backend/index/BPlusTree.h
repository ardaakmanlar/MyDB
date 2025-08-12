#pragma once
#include <vector>
#include <utility>
#include <string>
#include <algorithm>

#define DEGREE 3
#define MAX_KEYS   ((DEGREE) - 1)           // Max keys in a node
#define MIN_KEYS   (((DEGREE) + 1) / 2 - 1) // Min keys (non-root)
#define MIN_CHILD  (((DEGREE) + 1) / 2)     // Min children (non-root)



class Node{
public:
	std::vector<std::pair<int, std::vector<std::string>>> values;
	
	Node();
	Node(const std::pair<int, std::vector<std::string>>& value);
	Node(std::vector<std::pair<int, std::vector<std::string>>> _values);

	std::vector<Node*> insert(const std::pair<int, std::vector<std::string>>& value);

	virtual  const std::string& getClass()const = 0;
};

class LeafNode: public Node {
public:
	LeafNode* next;

	LeafNode();
	LeafNode(const std::pair<int, std::vector<std::string>>& value);
	LeafNode(std::vector<std::pair<int, std::vector<std::string>>> _values);

	const std::string& getClass()const override;
};


class InternalNode : public Node {
public:
	std::vector<Node*> ChildNodes;

	InternalNode();
	InternalNode(const std::pair<int, std::vector<std::string>>& value);
	InternalNode(std::vector<std::pair<int, std::vector<std::string>>> _values);

	const std::string& getClass()const override;
};


class BPlusTree{
private:
	LeafNode* head;
	Node* root;

public:
	BPlusTree();
	
	Node* insert(const std::pair<int, std::vector<std::string>>& value);
};

