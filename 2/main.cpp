#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
#include <optional>

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

        auto [p, x] = traverse(key);
        if (x->key != key) return false;

        Node* q = traverse(p->key).first;

        Node* prev = head;
        Node* cur = head->left;
        while (cur != p) {
            prev = cur;
            if (getBit(p->key, cur->bit) == 1) cur = cur->right;
            else cur = cur->left;
        }

        Node* m = prev;
        Node* n;
        if (p->left == x) n = p->right;
        else n = p->left;

        if (m->left == p) m->left = n;
        else m->right = n;
        if (x != p) {
            x->key = p->key;
            x->value = p->value;

            if (q->left == p) q->left = x;
            else q->right = x;
        }
        delete p;

        return true;
    }

private:
    Node* head;
    
    int getBit(const std::string& key, int i) {
        int charIdx = i / 8;
        int bitIdx = 7 - i % 8;
        if (charIdx >= key.size()) return 0;
        return (key[charIdx] >> bitIdx) & 1;
    }

    void toLowerCase(std::string& key) {
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
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
};


void serialize(PATRICIA P) {}
void deserialize(PATRICIA P) {}


int main() {
    PATRICIA P;

    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "+") {
            std::string word;
            uint64_t value;
            std::cin >> word >> value;

            if (P.insert(word, value)) std::cout << "OK\n";
            else std::cout << "Exist\n";
        } else if (cmd == "-") {
            std::string word;
            std::cin >> word;

            if (P.remove(word)) std::cout << "OK\n";
            else std::cout << "NoSuchWord\n";
        } else if (cmd == "!") {
            std::string action, path;
            std::cin >> action >> path;

            if (action == "Save") {
                serialize(P);
            } else {
                deserialize(P);
            }
        } else {
            auto result = P.search(cmd);

            if (result) std::cout << "OK:" << *result << "\n";
            else std::cout << "NoSuchWord\n";
        }
    }
}