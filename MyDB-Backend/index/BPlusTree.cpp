#include "BPlusTree.h"

using namespace std;

pair<int, vector<string>> globalTemp = { -1, {} };

Node::Node() : values() {}

Node::Node(const pair<int, vector<string>>& value){
	values.push_back(value);
}

Node::Node(vector<pair<int, vector<string>>> _values): values(_values) {}

LeafNode::LeafNode(): Node(), next(nullptr){}

LeafNode::LeafNode(const pair<int, vector<string>>& value) : Node(value), next(nullptr) {}

LeafNode::LeafNode(vector<pair<int, vector<string>>> _values) : Node(_values), next(nullptr){}

const string& LeafNode::getClass()const {
	return "Leaf";
}


Node* LeafNode::insert(const pair<int, vector<string>>& value) {
	values.push_back(value);

	sort(values.begin(), values.end(), [](const auto& a, const auto& b) {
		return a.first < b.first;
	});


	if (values.size() <= MAX_KEYS) {
		return nullptr;
	}
	
	vector<pair<int, vector<string>>> tempVec;

	for (size_t i = values.size() - MIN_KEYS; i < values.size(); i++) {
		tempVec.push_back(values[i]);
	}
	
	values.erase(values.end() - MIN_KEYS, values.end());

	LeafNode* newNode = new LeafNode(tempVec);
	
	newNode->next = this->next;
	next = newNode;

	return newNode;

}

InternalNode::InternalNode() : Node(), ChildNodes() {}

InternalNode::InternalNode(const pair<int, vector<string>>& value) : Node(value), ChildNodes() {}

InternalNode::InternalNode(vector<pair<int, vector<string>>> _values) : Node(_values), ChildNodes() {}

const string& InternalNode::getClass()const {
	return "Internal";
}

Node* InternalNode::insert(const pair<int, vector<string>>& value) {
	size_t idx = std::lower_bound(
		values.begin(),
		values.end(),
		value.first,
		[](const auto& a, int key) {
			return a.first < key; 
		}
	) - values.begin();


	Node* splitNode = ChildNodes[idx]->insert(value);

	if (!splitNode) {
		return nullptr;
	}


	if (globalTemp.first == -1) {
		values.push_back(splitNode->values.front());
	}
	else {
		values.push_back(globalTemp);
		globalTemp = { -1, {} };
	}

	sort(values.begin(), values.end(), [](const auto& a, const auto& b) {
		return a.first < b.first;
		});

	if (values.size() <= MAX_KEYS) {
		return nullptr;
	}
	
	vector<pair<int, vector<string>>> tempVec;
	for (size_t i = values.size() - MIN_KEYS; i < values.size(); i++) {
		tempVec.push_back(values[i]);
	}

	globalTemp = tempVec.front();

	values.erase(values.end() - (MIN_KEYS + 1) , values.end());

	InternalNode* newNode = new InternalNode(tempVec);
	
	for (size_t i = values.size() - CHILD_COUNT(newNode->values.size()); i < values.size(); i++) {
		newNode->ChildNodes.push_back(this->ChildNodes[i]);
	}

	this->ChildNodes.erase(this->ChildNodes.end() - (MIN_KEYS + 1), this->ChildNodes.end());

	return newNode;
}

BPlusTree::BPlusTree(): root(nullptr){}

bool BPlusTree::insert(const pair<int, vector<string>>& value) {
	if (!root) {
		root = new LeafNode(value);
		return true;
	}

	Node* result = root->insert(value);

	if (!result) {
		return true;
	}

	if (globalTemp.first == -1) {
		root = result;
	}
	else {
		Node* leftChild = root;

		root = new InternalNode(globalTemp);
		globalTemp = { -1, {} };

		InternalNode* internalPointer = static_cast<InternalNode*>(root);
		internalPointer->ChildNodes.push_back(leftChild);
		internalPointer->ChildNodes.push_back(result);
	}
	return true;
}


void BPlusTree::print() const {
	if (!root) {
		cout << "(empty tree)\n";
		return;
	}

	// Önce kökten en sola in
	const Node* current = root;
	while (current && current->getClass() == string("Internal")) {
		const InternalNode* internal = static_cast<const InternalNode*>(current);
		if (!internal->ChildNodes.empty())
			current = internal->ChildNodes.front();
		else
			break;
	}

	// Þimdi leaf zincirini yazdýr
	const LeafNode* leaf = static_cast<const LeafNode*>(current);
	cout << "Leaf chain: ";
	while (leaf) {
		cout << "[ ";
		for (auto& val : leaf->values) {
			cout << val.first << " ";
		}
		cout << "]";
		if (leaf->next) cout << " -> ";
		leaf = leaf->next;
	}
	cout << "\n";
}

void BPlusTree::printNode(const Node* node, int level) const {
	// Girinti ile seviye göstermek
	cout << string(level * 4, ' ');

	// Node tipini ve deðerlerini yazdýr
	cout << "[" << node->getClass() << "] ";
	for (auto& val : node->values) {
		cout << val.first << " ";
	}
	cout << "\n";

	// InternalNode ise çocuklarý yazdýr
	if (node->getClass() == string("Internal")) {
		const InternalNode* internal = static_cast<const InternalNode*>(node);
		for (auto child : internal->ChildNodes) {
			if (child) {
				printNode(child, level + 1);
			}
		}
	}
}