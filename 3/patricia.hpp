#ifndef PATRICIA_HPP
#define PATRICIA_HPP

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

class PATRICIA {
    struct Node {
        std::string key;
        uint64_t value;
        int bit;
        Node* left;
        Node* right;
        uint32_t saveId;

        Node(const std::string& key, uint64_t value, int bit)
            : key(key), value(value), bit(bit), left(nullptr), right(nullptr), saveId(UINT32_MAX) {}
    };

public:
    PATRICIA() {
        head = new Node("", 0, -1);
        head->left = head;
        nodeCount = 0;
    }

    std::optional<uint64_t> search(std::string key) {
        toLowerCase(key);

        Node* t = traverse(key).second;
        if (t->key != key) return std::nullopt;
        return t->value;
    }

    bool insert(std::string key, uint64_t value) {
        toLowerCase(key);

        Node* t = traverse(key).second;
        if (t->key == key) return false;

        int newBit = 0;
        while (getBit(key, newBit) == getBit(t->key, newBit)) ++newBit;

        Node* prev = head;
        Node* cur = head->left;
        while (cur->bit < newBit && cur->bit > prev->bit) {
            prev = cur;
            if (getBit(key, cur->bit) == 1) cur = cur->right;
            else cur = cur->left;
        }

        Node* newNode = new Node(key, value, newBit);
        if (prev->right == cur) prev->right = newNode;
        else prev->left = newNode;

        if (getBit(key, newBit) == 1) {
            newNode->right = newNode;
            newNode->left = cur;
        } else {
            newNode->left = newNode;
            newNode->right = cur;
        }

        ++nodeCount;
        return true;
    }

    bool remove(std::string key) {
        toLowerCase(key);

        Node* grand = head;
        Node* parent = head;
        Node* cur = head->left;

        while (cur->bit > parent->bit) {
            grand = parent;
            parent = cur;
            if (getBit(key, cur->bit)) cur = cur->right;
            else cur = cur->left;
        }

        if (cur->key != key) return false;

        Node* target = cur;

        if (parent == target) {
            Node* other = (target->left == target) ? target->right : target->left;

            if (other != head && other->bit > grand->bit) {
                Node* holder = findBacklinkHolder(other, grand, key, target);
                if (holder != nullptr) setChosenChild(holder, key, target, holder);
            }

            replaceChild(grand, target, other);
            delete target;
            --nodeCount;
            return true;
        }

        Node* q = parent;
        Node* other = (q->left == target) ? q->right : q->left;
        Node* promoted = (other == q) ? target : other;

        if (other != q && other->bit > q->bit) {
            Node* holder = findBacklinkHolder(other, q, q->key, q);
            if (holder != nullptr) setChosenChild(holder, q->key, q, target);
        }

        replaceChild(grand, q, promoted);
        target->key = q->key;
        target->value = q->value;

        delete q;
        --nodeCount;
        return true;
    }

    std::string save(const std::string& path) {
        std::ofstream file(path, std::ios::binary);
        if (!file) return std::strerror(errno);

        std::vector<Node*> nodes;
        nodes.reserve(nodeCount);
        collectNodes(head->left, -1, nodes);

        uint32_t n = nodeCount;
        file.write(reinterpret_cast<const char*>(&n), sizeof(n));
        if (!file) return streamWriteError();

        for (auto* node : nodes) {
            uint16_t len = static_cast<uint16_t>(node->key.size());

            file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file.write(node->key.data(), len);
            file.write(reinterpret_cast<const char*>(&node->value), sizeof(uint64_t));
            file.write(reinterpret_cast<const char*>(&node->bit), sizeof(int));

            uint32_t leftId = (node->left == head) ? UINT32_MAX : node->left->saveId;
            uint32_t rightId = (node->right == head) ? UINT32_MAX : node->right->saveId;
            file.write(reinterpret_cast<const char*>(&leftId), sizeof(leftId));
            file.write(reinterpret_cast<const char*>(&rightId), sizeof(rightId));

            if (!file) return streamWriteError();
        }

        file.flush();
        if (!file) return streamWriteError();

        return "";
    }

    std::string load(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) return std::strerror(errno);
        uint32_t n = 0;

        if (!file.read(reinterpret_cast<char*>(&n), sizeof(n))) return streamReadError(file);

        PATRICIA temp;
        std::vector<Node*> nodes(n, nullptr);
        std::vector<uint32_t> leftIds(n, UINT32_MAX);
        std::vector<uint32_t> rightIds(n, UINT32_MAX);
        auto cleanup = [&]() {
            for (Node* node : nodes) {
                delete node;
            }
        };

        for (uint32_t i = 0; i < n; ++i) {
            uint16_t len = 0;
            if (!file.read(reinterpret_cast<char*>(&len), sizeof(len))) {
                cleanup();
                return streamReadError(file);
            }
            if (len == 0 || len > 256) {
                cleanup();
                return "invalid file format";
            }

            std::string key(len, '\0');
            if (!file.read(key.data(), len)) {
                cleanup();
                return streamReadError(file);
            }
            for (unsigned char ch : key) {
                if (!std::isalpha(ch)) {
                    cleanup();
                    return "invalid file format";
                }
            }

            uint64_t value = 0;
            if (!file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
                cleanup();
                return streamReadError(file);
            }

            int bit = 0;
            if (!file.read(reinterpret_cast<char*>(&bit), sizeof(bit))) {
                cleanup();
                return streamReadError(file);
            }
            if (bit < 0) {
                cleanup();
                return "invalid file format";
            }

            if (!file.read(reinterpret_cast<char*>(&leftIds[i]), sizeof(leftIds[i]))) {
                cleanup();
                return streamReadError(file);
            }
            if (!file.read(reinterpret_cast<char*>(&rightIds[i]), sizeof(rightIds[i]))) {
                cleanup();
                return streamReadError(file);
            }

            nodes[i] = new Node(key, value, bit);
            nodes[i]->saveId = i;
        }

        for (uint32_t i = 0; i < n; ++i) {
            if (leftIds[i] != UINT32_MAX && leftIds[i] >= n) {
                cleanup();
                return "invalid file format";
            }
            if (rightIds[i] != UINT32_MAX && rightIds[i] >= n) {
                cleanup();
                return "invalid file format";
            }
            nodes[i]->left = (leftIds[i] == UINT32_MAX) ? temp.head : nodes[leftIds[i]];
            nodes[i]->right = (rightIds[i] == UINT32_MAX) ? temp.head : nodes[rightIds[i]];
        }

        char extra = '\0';
        if (file.read(&extra, 1)) {
            cleanup();
            return "invalid file format";
        }
        if (!file.eof()) {
            cleanup();
            return "invalid file format";
        }

        temp.head->left = (n == 0) ? temp.head : nodes[0];
        temp.nodeCount = n;

        if (n > 0) {
            std::vector<unsigned char> seen(n, 0);
            if (!temp.validateLoadedTree(temp.head->left, -1, seen)) {
                cleanup();
                return "invalid file format";
            }
            for (unsigned char mark : seen) {
                if (mark == 0) {
                    cleanup();
                    return "invalid file format";
                }
            }
        }

        std::swap(head, temp.head);
        std::swap(nodeCount, temp.nodeCount);
        return "";
    }

    ~PATRICIA() {
        clear(head->left, -1);
        delete head;
    }

private:
    Node* head;
    uint32_t nodeCount;

    int getBit(const std::string& key, int i) {
        int charIdx = i / 8;
        int bitIdx = 7 - i % 8;
        if (static_cast<size_t>(charIdx) >= key.size()) return 0;
        return (static_cast<unsigned char>(key[charIdx]) >> bitIdx) & 1;
    }

    void toLowerCase(std::string& key) {
        std::transform(key.begin(), key.end(), key.begin(), [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });
    }

    std::string streamReadError(const std::ifstream& file) {
        if (file.bad() && errno != 0) return std::strerror(errno);
        return "invalid file format";
    }

    std::string streamWriteError() {
        if (errno != 0) return std::strerror(errno);
        return "write error";
    }

    std::pair<Node*, Node*> traverse(const std::string& key) {
        Node* prev = head;
        Node* cur = head->left;
        while (cur->bit > prev->bit) {
            prev = cur;
            if (getBit(key, cur->bit) == 1) cur = cur->right;
            else cur = cur->left;
        }
        return {prev, cur};
    }

    void replaceChild(Node* parent, Node* oldChild, Node* newChild) {
        if (parent->left == oldChild) parent->left = newChild;
        else parent->right = newChild;
    }

    void setChosenChild(Node* parent, const std::string& key, Node* oldChild, Node* newChild) {
        if (getBit(key, parent->bit) == 1) {
            if (parent->right == oldChild) parent->right = newChild;
        } else {
            if (parent->left == oldChild) parent->left = newChild;
        }
    }

    Node* findBacklinkHolder(Node* start, Node* parentCtx, const std::string& key, Node* target) {
        Node* prev = parentCtx;
        Node* cur = start;

        while (cur->bit > prev->bit) {
            Node* next = getBit(key, cur->bit) == 1 ? cur->right : cur->left;
            if (next == target) return cur;
            prev = cur;
            cur = next;
        }

        return nullptr;
    }

    bool validateLoadedTree(Node* cur, int parentBit, std::vector<unsigned char>& seen) {
        if (cur == head) return true;
        if (cur->bit <= parentBit) return true;
        if (cur->saveId >= seen.size() || seen[cur->saveId] != 0) return false;

        seen[cur->saveId] = 1;
        return validateLoadedTree(cur->left, cur->bit, seen)
            && validateLoadedTree(cur->right, cur->bit, seen);
    }

    void collectNodes(Node* cur, int parentBit, std::vector<Node*>& nodes) {
        if (cur->bit <= parentBit) return;

        cur->saveId = static_cast<uint32_t>(nodes.size());
        nodes.push_back(cur);

        collectNodes(cur->left, cur->bit, nodes);
        collectNodes(cur->right, cur->bit, nodes);
    }

    void clear(Node* cur, int parentBit) {
        if (cur->bit <= parentBit) return;
        clear(cur->left, cur->bit);
        clear(cur->right, cur->bit);
        delete cur;
    }
};

#endif
