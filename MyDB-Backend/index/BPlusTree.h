#pragma once
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <iostream>

#define DEGREE 128
#define MAX_KEYS   ((DEGREE) - 1)           // Max keys in a node
#define MIN_KEYS   (((DEGREE) + 1) / 2 - 1) // Min keys (non-root)
#define MIN_CHILD  (((DEGREE) + 1) / 2)     // Min children (non-root)


#define CHILD_COUNT(keys) ((keys) + 1)          // Child count from keys
#define KEYS_COUNT(children) ((children) - 1)   // Keys count from children


class Node{
public:
	std::vector<std::pair<int, std::vector<std::string>>> values;
	
	Node();
	Node(const std::pair<int, std::vector<std::string>>& value);
	Node(std::vector<std::pair<int, std::vector<std::string>>> _values);


	virtual const std::string& getClass()const = 0;

	virtual Node* insert(const std::pair<int, std::vector<std::string>>& value) = 0;
};

class LeafNode: public Node {
public:
	LeafNode* next;

	LeafNode();
	LeafNode(const std::pair<int, std::vector<std::string>>& value);
	LeafNode(std::vector<std::pair<int, std::vector<std::string>>> _values);

	const std::string& getClass()const override;

	Node* insert(const std::pair<int, std::vector<std::string>>& value) override;
};


class InternalNode : public Node {
public:
	std::vector<Node*> ChildNodes;

	InternalNode();
	InternalNode(const std::pair<int, std::vector<std::string>>& value);
	InternalNode(std::vector<std::pair<int, std::vector<std::string>>> _values);

	const std::string& getClass()const override;

	Node* insert(const std::pair<int, std::vector<std::string>>& value) override;
};

//Index should be positive.
class BPlusTree{
private:
	Node* root;

public:
	BPlusTree();
	
	bool insert(const std::pair<int, std::vector<std::string>>& value);

	const std::pair<int, std::vector<std::string>>& search(int id)const;

	void print()const;
	
	void printNode(const Node* node, int level) const;
};


 
