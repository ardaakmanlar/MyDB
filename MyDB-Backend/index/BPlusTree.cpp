#include "BPlusTree.h"
using namespace std;

pair<int, vector<string>> globalTemp = { -1, {} };

Node::Node() : values() {}
Node::Node(const pair<int, vector<string>>& value) { values.push_back(value); }
Node::Node(vector<pair<int, vector<string>>> _values) : values(_values) {}

LeafNode::LeafNode() : Node(), next(nullptr) {}
LeafNode::LeafNode(const pair<int, vector<string>>& value) : Node(value), next(nullptr) {}
LeafNode::LeafNode(vector<pair<int, vector<string>>> _values) : Node(_values), next(nullptr) {}

const string& LeafNode::getClass() const {
    static const string name = "Leaf";
    return name;
}

Node* LeafNode::insert(const pair<int, vector<string>>& value) {
    values.push_back(value);
    sort(values.begin(), values.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    if (values.size() <= MAX_KEYS) return nullptr;

    size_t splitIndex = MIN_KEYS;
    vector<pair<int, vector<string>>> rightVals(values.begin() + splitIndex, values.end());
    values.erase(values.begin() + splitIndex, values.end());

    LeafNode* newNode = new LeafNode(rightVals);
    newNode->next = this->next;
    this->next = newNode;

    globalTemp = newNode->values.front();
    return newNode;
}

InternalNode::InternalNode() : Node(), ChildNodes() {}
InternalNode::InternalNode(const pair<int, vector<string>>& value) : Node(value), ChildNodes() {}
InternalNode::InternalNode(vector<pair<int, vector<string>>> _values) : Node(_values), ChildNodes() {}

const string& InternalNode::getClass() const {
    static const string name = "Internal";
    return name;
}

Node* InternalNode::insert(const pair<int, vector<string>>& value) {
    size_t idx = std::lower_bound(
        values.begin(), values.end(), value.first,
        [](const auto& a, int key) { return a.first < key; }
    ) - values.begin();

    if (idx >= ChildNodes.size()) idx = ChildNodes.size() - 1;

    Node* splitNode = ChildNodes[idx]->insert(value);
    if (!splitNode) return nullptr;

    pair<int, vector<string>> upKey;
    if (globalTemp.first != -1) { upKey = globalTemp; globalTemp = { -1, {} }; }
    else { upKey = splitNode->values.front(); }

    values.insert(values.begin() + idx, upKey);
    ChildNodes.insert(ChildNodes.begin() + idx + 1, splitNode);

    if (values.size() <= MAX_KEYS) return nullptr;

    size_t mid = MIN_KEYS;
    pair<int, vector<string>> promote = values[mid];
    vector<pair<int, vector<string>>> rightKeys(values.begin() + mid + 1, values.end());
    values.erase(values.begin() + mid, values.end());

    InternalNode* newNode = new InternalNode(rightKeys);

    auto rightChildBegin = ChildNodes.begin() + (mid + 1);
    newNode->ChildNodes.assign(rightChildBegin, ChildNodes.end());
    ChildNodes.erase(rightChildBegin, ChildNodes.end());

    globalTemp = promote;
    return newNode;
}

BPlusTree::BPlusTree() : root(nullptr) {}

bool BPlusTree::insert(const pair<int, vector<string>>& value) {
    if (!root) { root = new LeafNode(value); return true; }

    const auto& found = search(value.first);
    if (found.first != -1) {
        cerr << "Insert failed: key " << value.first << " already exists.\n";
        return false;
    }

    Node* result = root->insert(value);
    if (!result) return true;

    Node* leftChild = root;
    Node* rightChild = result;

    root = new InternalNode(globalTemp);
    globalTemp = { -1, {} };

    InternalNode* p = static_cast<InternalNode*>(root);
    p->ChildNodes.push_back(leftChild);
    p->ChildNodes.push_back(rightChild);

    return true;
}

void BPlusTree::print() const {
    if (!root) { cout << "(empty tree)\n"; return; }

    const Node* current = root;
    while (current && current->getClass() == string("Internal")) {
        const InternalNode* in = static_cast<const InternalNode*>(current);
        if (!in->ChildNodes.empty()) current = in->ChildNodes.front(); else break;
    }

    const LeafNode* leaf = static_cast<const LeafNode*>(current);
    cout << "Leaf chain: ";
    while (leaf) {
        cout << "[ ";
        for (auto& v : leaf->values) cout << v.first << " ";
        cout << "]";
        if (leaf->next) cout << " -> ";
        leaf = leaf->next;
    }
    cout << "\n";
}

void BPlusTree::printNode(const Node* node, int level) const {
    cout << string(level * 4, ' ');
    cout << "[" << node->getClass() << "] ";
    for (auto& v : node->values) cout << v.first << " ";
    cout << "\n";

    if (node->getClass() == string("Internal")) {
        const InternalNode* in = static_cast<const InternalNode*>(node);
        for (auto child : in->ChildNodes) if (child) printNode(child, level + 1);
    }
}


const pair<int, vector<string>>& BPlusTree::search(int id) const {
    static const pair<int, vector<string>> notFound = { -1, {} };
    if (!root) return notFound;

    const Node* cur = root;

    while (true) {
        if (cur->getClass() == "Leaf") {
            const auto& vals = cur->values;
            auto it = std::lower_bound(
                vals.begin(), vals.end(), id,
                [](const auto& a, int key) { return a.first < key; }
            );
            if (it != vals.end() && it->first == id) {
                return *it; 
            }
            return notFound; 
        }
        else {
            const auto* inode = static_cast<const InternalNode*>(cur);

            size_t idx = std::lower_bound(
                inode->values.begin(), inode->values.end(), id,
                [](const auto& a, int key) { return a.first < key; }
            ) - inode->values.begin();

            if (idx >= inode->ChildNodes.size() || inode->ChildNodes[idx] == nullptr) {
                return notFound; 
            }
            cur = inode->ChildNodes[idx];
        }
    }
}