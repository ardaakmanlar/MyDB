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
        [](int k, const std::pair<int, std::vector<std::string>>& a) { return k < a.first; })
        - values.begin();

    if (idx >= ChildNodes.size() || ChildNodes[idx] == nullptr) return false;

    Node* childBefore = ChildNodes[idx];

    int oldFirst = INT_MIN;
    bool wasLeaf = (childBefore->getClass() == std::string("Leaf"));
    if (wasLeaf) {
        LeafNode* lf = static_cast<LeafNode*>(childBefore);
        if (!lf->values.empty()) oldFirst = lf->values.front().first;
    }

    bool ok = ChildNodes[idx]->remove(key, this, (int)idx);
    if (!ok) return false;

    Node* child = ChildNodes[idx];
    if (child == nullptr) return false;

    if (wasLeaf) {
        LeafNode* lf = dynamic_cast<LeafNode*>(child);
        if (lf && !lf->values.empty()) {
            int newFirst = lf->values.front().first;
            if (oldFirst != INT_MIN && newFirst != oldFirst) {
                if (idx > 0) values[idx - 1].first = newFirst;
            }
        }
    }

    bool needFix = child->values.size() < MIN_KEYS;

    if (needFix) {
        if (LeafNode* leaf = dynamic_cast<LeafNode*>(child)) {
            if (idx > 0) {
                LeafNode* leftLeaf = (idx >= 1) ? dynamic_cast<LeafNode*>(ChildNodes[idx - 1]) : nullptr;
                if (leftLeaf && leftLeaf->values.size() > MIN_KEYS) {
                    leaf->values.insert(leaf->values.begin(), leftLeaf->values.back());
                    leftLeaf->values.pop_back();
                    values[idx - 1] = leaf->values.front();
                    return true;
                }
            }
            if (idx + 1 < ChildNodes.size()) {
                LeafNode* rightLeaf = dynamic_cast<LeafNode*>(ChildNodes[idx + 1]);
                if (rightLeaf && rightLeaf->values.size() > MIN_KEYS) {
                    leaf->values.push_back(rightLeaf->values.front());
                    rightLeaf->values.erase(rightLeaf->values.begin());
                    values[idx] = rightLeaf->values.front();
                    return true;
                }
            }
            if (idx > 0) {
                LeafNode* leftLeaf = dynamic_cast<LeafNode*>(ChildNodes[idx - 1]);
                if (!leftLeaf) return true;
                leftLeaf->values.insert(leftLeaf->values.end(), leaf->values.begin(), leaf->values.end());
                leftLeaf->next = leaf->next;
                delete leaf;
                ChildNodes.erase(ChildNodes.begin() + idx);
                values.erase(values.begin() + (idx - 1));
            }
            else if (idx + 1 < ChildNodes.size()) {
                LeafNode* rightLeaf = dynamic_cast<LeafNode*>(ChildNodes[idx + 1]);
                if (!rightLeaf) return true;
                leaf->values.insert(leaf->values.end(), rightLeaf->values.begin(), rightLeaf->values.end());
                leaf->next = rightLeaf->next;
                delete rightLeaf;
                ChildNodes.erase(ChildNodes.begin() + idx + 1);
                values.erase(values.begin() + idx);
            }
        }
        else {
            InternalNode* childInt = dynamic_cast<InternalNode*>(child);
            if (!childInt) return true;

            if (idx > 0) {
                InternalNode* leftInt = dynamic_cast<InternalNode*>(ChildNodes[idx - 1]);
                if (leftInt && leftInt->values.size() > MIN_KEYS) {
                    childInt->values.insert(childInt->values.begin(), values[idx - 1]);
                    values[idx - 1] = leftInt->values.back();
                    leftInt->values.pop_back();
                    childInt->ChildNodes.insert(childInt->ChildNodes.begin(), leftInt->ChildNodes.back());
                    leftInt->ChildNodes.pop_back();
                    return true;
                }
            }
            if (idx + 1 < ChildNodes.size()) {
                InternalNode* rightInt = dynamic_cast<InternalNode*>(ChildNodes[idx + 1]);
                if (rightInt && rightInt->values.size() > MIN_KEYS) {
                    childInt->values.push_back(values[idx]);
                    values[idx] = rightInt->values.front();
                    rightInt->values.erase(rightInt->values.begin());
                    childInt->ChildNodes.push_back(rightInt->ChildNodes.front());
                    rightInt->ChildNodes.erase(rightInt->ChildNodes.begin());
                    return true;
                }
            }
            if (idx > 0) {
                InternalNode* leftInt = dynamic_cast<InternalNode*>(ChildNodes[idx - 1]);
                if (!leftInt) return true;
                leftInt->values.push_back(values[idx - 1]);
                leftInt->values.insert(leftInt->values.end(), childInt->values.begin(), childInt->values.end());
                leftInt->ChildNodes.insert(leftInt->ChildNodes.end(), childInt->ChildNodes.begin(), childInt->ChildNodes.end());
                delete childInt;
                ChildNodes.erase(ChildNodes.begin() + idx);
                values.erase(values.begin() + (idx - 1));
            }
            else if (idx + 1 < ChildNodes.size()) {
                InternalNode* rightInt = dynamic_cast<InternalNode*>(ChildNodes[idx + 1]);
                if (!rightInt) return true;
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


LeafNode* BPlusTree::findLeaf(int key) {
    if (!root) return nullptr;

    Node* cur = root;
    while (cur->getClass() == std::string("Internal")) {
        InternalNode* inode = static_cast<InternalNode*>(cur);
        size_t idx = std::upper_bound(inode->values.begin(), inode->values.end(), key,
            [](int k, const auto& a) { return k < a.first; })
            - inode->values.begin();
        if (idx >= inode->ChildNodes.size() || inode->ChildNodes[idx] == nullptr) {
            return nullptr;
        }
        cur = inode->ChildNodes[idx];
    }
    return dynamic_cast<LeafNode*>(cur);
}

bool LeafNode::updateValue(int key, const std::vector<std::string>& newValues, int maxN) {
    auto it = std::lower_bound(values.begin(), values.end(), key,
        [](const auto& a, int k) { return a.first < k; });
    if (it == values.end() || it->first != key) return false;

    if (maxN < 0 || (int)newValues.size() <= maxN) {
        it->second = newValues;
    }
    else {
        it->second.assign(newValues.begin(), newValues.begin() + maxN);
    }
    return true;
}

bool BPlusTree::update(int key, const std::vector<std::string>& newValues, int maxN) {
    if (!root) return false;
    LeafNode* leaf = findLeaf(key);
    if (!leaf) return false;
    return leaf->updateValue(key, newValues, maxN);
}

bool BPlusTree::updateKey(int oldKey, int newKey, const std::vector<std::string>& newValues, int maxN) {
    if (!root) return false;

    if (oldKey == newKey) {
        return update(oldKey, newValues, maxN);
    }

    const auto& oldRec = search(oldKey);
    if (oldRec.first == -1) return false;

    if (search(newKey).first != -1) return false;

    std::vector<std::string> payload;
    if (newValues.empty()) {
        payload = oldRec.second;
    }
    else if (maxN < 0 || (int)newValues.size() <= maxN) {
        payload = newValues;
    }
    else {
        payload.assign(newValues.begin(), newValues.begin() + maxN);
    }

    if (!remove(oldKey)) return false;
    if (!insert({ newKey, payload })) {
        insert(oldRec); 
        return false;
    }
    return true;
}

void BPlusTree::printRows() const {
    if (!root) {
        std::cout << "(empty tree)\n";
        return;
    }

    // En soldaki yapraða in
    const Node* current = root;
    while (current && current->getClass() == std::string("Internal")) {
        const InternalNode* in = static_cast<const InternalNode*>(current);
        if (!in->ChildNodes.empty()) current = in->ChildNodes.front();
        else break;
    }

    const LeafNode* leaf = static_cast<const LeafNode*>(current);

    while (leaf) {
        for (const auto& row : leaf->values) {
            std::cout << row.first; // id
            for (const auto& s : row.second) {
                std::cout << " | " << s;
            }
            std::cout << "\n";
        }
        leaf = leaf->next;
    }
}

std::vector<std::pair<int, std::vector<std::string>>> BPlusTree::collect(int limit) const {
    std::vector<std::pair<int, std::vector<std::string>>> result;
    if (!root) return result;
    const Node* current = root;
    while (current && current->getClass() == std::string("Internal")) {
        const InternalNode* in = static_cast<const InternalNode*>(current);
        if (in->ChildNodes.empty()) return result;
        current = in->ChildNodes.front();
    }
    const LeafNode* leaf = static_cast<const LeafNode*>(current);
    int cnt = 0;
    while (leaf && (limit < 0 || cnt < limit)) {
        for (const auto& kv : leaf->values) {
            if (limit >= 0 && cnt >= limit) break;
            result.push_back(kv);
            ++cnt;
        }
        leaf = leaf->next;
    }
    return result;
}

