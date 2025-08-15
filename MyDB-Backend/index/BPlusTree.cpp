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
    size_t idx = std::upper_bound(values.begin(), values.end(), value.first,
        [](int key, const auto& a) { return key < a.first; })
        - values.begin();

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
            auto it = std::lower_bound(vals.begin(), vals.end(), id,
                [](const auto& a, int key) { return a.first < key; });
            if (it != vals.end() && it->first == id) return *it;
            return notFound;
        }
        else {
            const auto* inode = static_cast<const InternalNode*>(cur);
            size_t idx = std::upper_bound(inode->values.begin(), inode->values.end(), id,
                [](int key, const auto& a) { return key < a.first; })
                - inode->values.begin();
            if (idx >= inode->ChildNodes.size() || inode->ChildNodes[idx] == nullptr) return notFound;
            cur = inode->ChildNodes[idx];
        }
    }
}


bool BPlusTree::remove(int key) {
    if (!root) return false;
    bool ok = root->remove(key, nullptr, -1);
    if (auto internal = dynamic_cast<InternalNode*>(root)) {
        if (internal->values.empty() && internal->ChildNodes.size() == 1) {
            root = internal->ChildNodes[0];
            delete internal;
        }
    }
    if (auto leaf = dynamic_cast<LeafNode*>(root)) {
        if (leaf->values.empty()) {
            delete root;
            root = nullptr;
        }
    }
    return ok;
}

bool LeafNode::remove(int key, InternalNode* , int ) {
    auto it = lower_bound(values.begin(), values.end(), key,
        [](const auto& a, int k) { return a.first < k; });
    if (it == values.end() || it->first != key) return false;
    values.erase(it);
    return true;
}

bool InternalNode::remove(int key, InternalNode* parent, int parentIndex) {
    size_t idx = std::upper_bound(values.begin(), values.end(), key,
        [](int k, const auto& a) { return k < a.first; })
        - values.begin();

    Node* childBefore = ChildNodes[idx];

    int oldFirst = INT_MIN;
    bool wasLeaf = (childBefore->getClass() == "Leaf");
    if (wasLeaf) {
        auto* lf = static_cast<LeafNode*>(childBefore);
        if (!lf->values.empty()) oldFirst = lf->values.front().first;
    }

    bool ok = ChildNodes[idx]->remove(key, this, (int)idx);
    if (!ok) return false;

    Node* child = ChildNodes[idx];

    if (wasLeaf) {
        auto* lf = static_cast<LeafNode*>(child);
        if (lf && !lf->values.empty()) {
            int newFirst = lf->values.front().first;
            if (oldFirst != INT_MIN && newFirst != oldFirst) {
                if (idx > 0) values[idx - 1].first = newFirst;
            }
        }
    }

    bool needFix = (parent != nullptr ? child->values.size() < MIN_KEYS
        : child->values.size() < MIN_KEYS);

    if (needFix) {
        if (auto leaf = dynamic_cast<LeafNode*>(child)) {
            if (idx > 0) {
                if (auto leftLeaf = dynamic_cast<LeafNode*>(ChildNodes[idx - 1])) {
                    if (leftLeaf->values.size() > MIN_KEYS) {
                        leaf->values.insert(leaf->values.begin(), leftLeaf->values.back());
                        leftLeaf->values.pop_back();
                        values[idx - 1] = leaf->values.front();
                        return true;
                    }
                }
            }
            if (idx + 1 < ChildNodes.size()) {
                if (auto rightLeaf = dynamic_cast<LeafNode*>(ChildNodes[idx + 1])) {
                    if (rightLeaf->values.size() > MIN_KEYS) {
                        leaf->values.push_back(rightLeaf->values.front());
                        rightLeaf->values.erase(rightLeaf->values.begin());
                        values[idx] = rightLeaf->values.front();
                        return true;
                    }
                }
            }
            if (idx > 0) {
                auto leftLeaf = static_cast<LeafNode*>(ChildNodes[idx - 1]);
                leftLeaf->values.insert(leftLeaf->values.end(), leaf->values.begin(), leaf->values.end());
                leftLeaf->next = leaf->next;
                delete leaf;
                ChildNodes.erase(ChildNodes.begin() + idx);
                values.erase(values.begin() + (idx - 1));
            }
            else {
                auto rightLeaf = static_cast<LeafNode*>(ChildNodes[idx + 1]);
                leaf->values.insert(leaf->values.end(), rightLeaf->values.begin(), rightLeaf->values.end());
                leaf->next = rightLeaf->next;
                delete rightLeaf;
                ChildNodes.erase(ChildNodes.begin() + idx + 1);
                values.erase(values.begin() + idx);
            }
        }
        else {
            auto childInt = static_cast<InternalNode*>(child);
            if (idx > 0) {
                if (auto leftInt = dynamic_cast<InternalNode*>(ChildNodes[idx - 1])) {
                    if (leftInt->values.size() > MIN_KEYS) {
                        childInt->values.insert(childInt->values.begin(), values[idx - 1]);
                        values[idx - 1] = leftInt->values.back();
                        leftInt->values.pop_back();
                        childInt->ChildNodes.insert(childInt->ChildNodes.begin(), leftInt->ChildNodes.back());
                        leftInt->ChildNodes.pop_back();
                        return true;
                    }
                }
            }
            if (idx + 1 < ChildNodes.size()) {
                if (auto rightInt = dynamic_cast<InternalNode*>(ChildNodes[idx + 1])) {
                    if (rightInt->values.size() > MIN_KEYS) {
                        childInt->values.push_back(values[idx]);
                        values[idx] = rightInt->values.front();
                        rightInt->values.erase(rightInt->values.begin());
                        childInt->ChildNodes.push_back(rightInt->ChildNodes.front());
                        rightInt->ChildNodes.erase(rightInt->ChildNodes.begin());
                        return true;
                    }
                }
            }
            if (idx > 0) {
                auto leftInt = static_cast<InternalNode*>(ChildNodes[idx - 1]);
                leftInt->values.push_back(values[idx - 1]);
                leftInt->values.insert(leftInt->values.end(), childInt->values.begin(), childInt->values.end());
                leftInt->ChildNodes.insert(leftInt->ChildNodes.end(), childInt->ChildNodes.begin(), childInt->ChildNodes.end());
                delete childInt;
                ChildNodes.erase(ChildNodes.begin() + idx);
                values.erase(values.begin() + (idx - 1));
            }
            else {
                auto rightInt = static_cast<InternalNode*>(ChildNodes[idx + 1]);
                childInt->values.push_back(values[idx]);
                childInt->values.insert(childInt->values.end(), rightInt->values.begin(), rightInt->values.end());
                childInt->ChildNodes.insert(childInt->ChildNodes.end(), rightInt->ChildNodes.begin(), rightInt->ChildNodes.end());
                delete rightInt;
                ChildNodes.erase(ChildNodes.begin() + idx + 1);
                values.erase(values.begin() + idx);
            }
        }
    }

    return true;
}


void InternalNode::borrowFromLeft(int idx) {
    InternalNode* left = dynamic_cast<InternalNode*>(ChildNodes[idx - 1]);
    InternalNode* curr = dynamic_cast<InternalNode*>(ChildNodes[idx]);
    curr->values.insert(curr->values.begin(), values[idx - 1]);
    values[idx - 1] = left->values.back();
    left->values.pop_back();
    curr->ChildNodes.insert(curr->ChildNodes.begin(), left->ChildNodes.back());
    left->ChildNodes.pop_back();
}

void InternalNode::borrowFromRight(int idx) {
    InternalNode* right = dynamic_cast<InternalNode*>(ChildNodes[idx + 1]);
    InternalNode* curr = dynamic_cast<InternalNode*>(ChildNodes[idx]);
    curr->values.push_back(values[idx]);
    values[idx] = right->values.front();
    right->values.erase(right->values.begin());
    curr->ChildNodes.push_back(right->ChildNodes.front());
    right->ChildNodes.erase(right->ChildNodes.begin());
}

void InternalNode::mergeChildren(int idx) {
    Node* left = ChildNodes[idx];
    Node* right = ChildNodes[idx + 1];
    left->values.push_back(values[idx]);
    left->values.insert(left->values.end(), right->values.begin(), right->values.end());
    if (auto leftInternal = dynamic_cast<InternalNode*>(left)) {
        auto rightInternal = dynamic_cast<InternalNode*>(right);
        leftInternal->ChildNodes.insert(
            leftInternal->ChildNodes.end(),
            rightInternal->ChildNodes.begin(),
            rightInternal->ChildNodes.end()
        );
    }
    delete right;
    ChildNodes.erase(ChildNodes.begin() + idx + 1);
    values.erase(values.begin() + idx);
}
