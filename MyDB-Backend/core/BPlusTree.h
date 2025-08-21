#pragma once
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <iostream>

#define DEGREE 3
#define MAX_KEYS   ((DEGREE) - 1)
#define MIN_KEYS   (((DEGREE) + 1) / 2 - 1)
#define MIN_CHILD  (((DEGREE) + 1) / 2)

#define CHILD_COUNT(keys) ((keys) + 1)
#define KEYS_COUNT(children) ((children) - 1)

class InternalNode;

class Node {
public:
    std::vector<std::pair<int, std::vector<std::string>>> values;

    Node();
    Node(const std::pair<int, std::vector<std::string>>& value);
    Node(std::vector<std::pair<int, std::vector<std::string>>> _values);

    virtual const std::string& getClass() const = 0;
    virtual Node* insert(const std::pair<int, std::vector<std::string>>& value) = 0;
    virtual bool remove(int key, InternalNode* parent, int parentIndex) = 0;

    virtual ~Node() = default;
};

class LeafNode : public Node {
public:
    LeafNode* next;

    LeafNode();
    LeafNode(const std::pair<int, std::vector<std::string>>& value);
    LeafNode(std::vector<std::pair<int, std::vector<std::string>>> _values);

    const std::string& getClass() const override;
    Node* insert(const std::pair<int, std::vector<std::string>>& value) override;
    bool remove(int key, InternalNode* parent, int parentIndex) override;

    bool updateValue(int key, const std::vector<std::string>& newValues, int maxN = -1);

};

class InternalNode : public Node {
public:
    std::vector<Node*> ChildNodes;

    InternalNode();
    InternalNode(const std::pair<int, std::vector<std::string>>& value);
    InternalNode(std::vector<std::pair<int, std::vector<std::string>>> _values);

    const std::string& getClass() const override;
    Node* insert(const std::pair<int, std::vector<std::string>>& value) override;
    bool remove(int key, InternalNode* parent, int parentIndex) override;

    void borrowFromLeft(int idx);
    void borrowFromRight(int idx);
    void mergeChildren(int idx);
};

class BPlusTree {
private:
    Node* root;


    LeafNode* findLeaf(int key);
public:
    BPlusTree();

    bool insert(const std::pair<int, std::vector<std::string>>& value);
    const std::pair<int, std::vector<std::string>>& search(int id) const;
    void print() const;
    void printNode(const Node* node, int level) const;
    bool remove(int key);

    bool update(int key, const std::vector<std::string>& newValues, int maxN = -1);
    bool updateKey(int oldKey, int newKey, const std::vector<std::string>& newValues = {}, int maxN = -1);


    void printRows() const;

    std::vector<std::pair<int, std::vector<std::string>>> collect(int limit) const;


};
