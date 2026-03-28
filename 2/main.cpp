#include <iostream>
#include <vector>
#include <string>

class PATRICIA {
    struct Node {
        int bit;
        std::string key;
        uint64_t value;
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

    Node* search(const std::string& key) {
        return traverse(key).second;
    }

    bool insert(const std::string& key, uint64_t value) {
        Node* t = search(key);
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

private:
    Node* head;
    
    int getBit(const std::string& key, int i) {
        int charIdx = i / 8;
        int bitIdx = 7 - i % 8;
        if (charIdx >= key.size()) return 0;
        return (key[charIdx] >> bitIdx) & 1;
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