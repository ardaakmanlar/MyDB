#include "BPlusTree.h"

Node::Node() : values() {}

Node::Node(const std::pair<int, std::vector<std::string>>& value){
	values.push_back(value);
}

Node::Node(std::vector<std::pair<int, std::vector<std::string>>> _values): values(_values) {}

LeafNode::LeafNode(): Node(), next(nullptr){}

LeafNode::LeafNode(const std::pair<int, std::vector<std::string>>& value) : Node(value), next(nullptr) {}

LeafNode::LeafNode(std::vector<std::pair<int, std::vector<std::string>>> _values) : Node(_values), next(nullptr) {}

const std::string& LeafNode::getClass()const {
	return "Leaf";
}

InternalNode::InternalNode() : Node(), ChildNodes() {}

InternalNode::InternalNode(const std::pair<int, std::vector<std::string>>& value) : Node(value), ChildNodes() {}

InternalNode::InternalNode(std::vector<std::pair<int, std::vector<std::string>>> _values) : Node(_values), ChildNodes() {}

const std::string& InternalNode::getClass()const {
	return "Internal";
}

BPlusTree::BPlusTree(): head(nullptr) , root(nullptr){}

Node* BPlusTree::insert(const std::pair<int, std::vector<std::string>>& value) {
	if (!root) {
		head = new LeafNode(value);
		root = head;
	}
	if (root->getClass() == "Internal") {
		for (size_t i = 0; i < root->values.size(); i++){
			if (value.first < root->values[i].first) {
				
			}
		}
	}


	root->values.push_back(value);
	sort(root->values.begin(), root->values.end(), [](const auto& a, const auto& b) {
		return a.first < b.first;
	});

	if (root->values.size() <= MAX_KEYS) {
	}


}