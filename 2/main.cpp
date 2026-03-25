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

private:
    Node* head;
    
    int getBit(const std::string& key, int i) {
        int charIdx = i / 8;
        int bitIdx = 7 - i % 8;
        if (charIdx >= key.size()) return 0;
        return (key[charIdx] >> bitIdx) & 1;
    }

    Node* search(std::string key) {
        return search(head->left, key, head);
    }

    Node* search(Node* cur, std::string key, Node* prev) {
        if (cur->bit <= prev->bit) return cur;
        if (getBit(key, cur->bit) == 1) return search(cur->right, key, cur);
        else return search(cur->left, key, cur);
    }
};