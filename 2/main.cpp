#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <optional>
#include <fstream>
#include <cstring>
#include <cctype>
#include <utility>

class PATRICIA {
    struct Node {
        std::string key;
        uint64_t value;
        int bit;
        Node* left;
        Node* right;

        Node(const std::string& key, uint64_t value, int bit)
            : key(key), value(value), bit(bit), left(nullptr), right(nullptr) {}
    };

public:
    PATRICIA() {
        head = new Node("", 0, -1);
        head->left = head;
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
        return true;
    }

    std::string save(const std::string& path) {
        std::ofstream file(path, std::ios::binary);
        if (!file) return std::strerror(errno);

        std::vector<Node*> nodes;
        collectNodes(head->left, -1, nodes);

        std::unordered_map<Node*, uint32_t> id;
        for (uint32_t i = 0; i < nodes.size(); ++i) {
            id[nodes[i]] = i;
        }

        uint32_t n = nodes.size();
        file.write(reinterpret_cast<const char*>(&n), sizeof(n));
        if (!file) return streamWriteError();

        for (auto* node : nodes) {
            uint16_t len = static_cast<uint16_t>(node->key.size());

            file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file.write(node->key.data(), len);
            file.write(reinterpret_cast<const char*>(&node->value), sizeof(uint64_t));
            file.write(reinterpret_cast<const char*>(&node->bit), sizeof(int));

            uint32_t leftId = (node->left == head) ? UINT32_MAX : id[node->left];
            uint32_t rightId = (node->right == head) ? UINT32_MAX : id[node->right];
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

        for (Node* node : nodes) {
            Node* found = temp.traverse(node->key).second;
            if (found != node || found->value != node->value) {
                cleanup();
                return "invalid file format";
            }
        }

        nodes.assign(n, nullptr);
        std::swap(head, temp.head);
        return "";
    }

    ~PATRICIA() {
        clear(head->left, -1);
        delete head;
    }

private:
    Node* head;

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

    void collectNodes(Node* cur, int parentBit, std::vector<Node*>& nodes) {
        if (cur->bit <= parentBit) return;

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

int main() {
    PATRICIA dictionary;

    std::string cmd;
    while (std::cin >> cmd) {
        try {
            if (cmd == "+") {
                std::string word;
                uint64_t value;
                std::cin >> word >> value;

                if (dictionary.insert(word, value)) std::cout << "OK\n";
                else std::cout << "Exist\n";
            } else if (cmd == "-") {
                std::string word;
                std::cin >> word;

                if (dictionary.remove(word)) std::cout << "OK\n";
                else std::cout << "NoSuchWord\n";
            } else if (cmd == "!") {
                std::string action;
                std::string path;
                std::cin >> action >> path;

                if (action == "Save") {
                    std::string err = dictionary.save(path);
                    if (err.empty()) std::cout << "OK\n";
                    else std::cout << "ERROR: " << err << "\n";
                } else if (action == "Load") {
                    std::string err = dictionary.load(path);
                    if (err.empty()) std::cout << "OK\n";
                    else std::cout << "ERROR: " << err << "\n";
                } else {
                    std::cout << "ERROR: unknown command\n";
                }
            } else {
                auto result = dictionary.search(cmd);

                if (result) std::cout << "OK: " << *result << "\n";
                else std::cout << "NoSuchWord\n";
            }
        } catch (const std::bad_alloc&) {
            std::cout << "ERROR: not enough memory\n";
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << "\n";
        }
    }
}
