#include <iostream>
#include <vector>

class BST {
    struct TNode {
        int val;
        TNode* left;
        TNode* right;
        TNode(int val) : val(val), left(nullptr), right(nullptr) {}
    };

public:
    BST() : root(nullptr) {}

    TNode* search(int val) { return find(root, val); }

    void insert(int val) { root = insert(root, val); }

    void remove(int val) { root = remove(root, val); }

    void inorder() { inorder(root); }

private:
    TNode* root;

    TNode* find(TNode* cur, int val) {
        if (cur == nullptr) {
            return nullptr;
        }

        if (cur -> val == val) {
            return cur;
        } else if (cur -> val < val) {
            return find(cur -> right, val);
        } else {
            return find(cur -> left, val);
        }
    }

    TNode* insert(TNode* cur, int val) {
        if (cur == nullptr) {
            return new TNode(val);
        }

        if (cur -> val > val) {
            cur -> left = insert(cur -> left, val);
        } else if (cur -> val < val) {
            cur -> right = insert(cur -> right, val);
        }

        return cur;
    }

    TNode* remove(TNode* cur, int val) {
        if (cur == nullptr) {
            return nullptr;
        }

        if (cur -> val < val) {
            cur -> right = remove(cur -> right, val);
        } else if (cur -> val > val) {
            cur -> left = remove(cur -> left, val);
        } else {
            if (!cur -> left && !cur -> right) {
                delete cur;
                return nullptr;
            } else if (cur -> left && !cur -> right) {
                TNode* rest = cur -> left;
                delete cur;
                return rest;
            } else if (!cur -> left && cur -> right) {
                TNode* rest = cur -> right;
                delete cur;
                return rest;
            } else {
                TNode* rest = minRight(cur -> right);
                cur -> val = rest -> val;
                cur -> right = remove(cur -> right, cur -> val);
            }
        }
        return cur;
    }

    TNode* minRight(TNode* cur) {
        while (cur && cur -> left) {
            cur = cur -> left;
        }

        return cur;
    }

    void inorder(TNode* cur) {
        if (cur == nullptr) {
            return;
        }
        inorder(cur -> left);
        std::cout << cur -> val << ' ';
        inorder(cur -> right);
    }

    TNode* rightRotate(TNode* y) {
        if (y == nullptr) {
            return nullptr;
        }

        auto x = y -> left;
        auto beta = x -> right;
        y -> left = beta;
        x -> right = y;
        return x;
    }

    TNode* rotateLeft(TNode* x) {
        if (x == nullptr) {
            return nullptr;
        }

        auto y = x -> right;
        auto beta = y -> left;
        x -> right = beta;
        y -> left = x;

        return y;
    }
};


int main() {
    BST tree = BST();
    std::vector<int> values = {3, 14, 15, 9, 2, 6, 5, 35};

    for (int& val : values) {
        tree.insert(val);
    }

    tree.inorder();

    std::vector<int> toDelete = {3, 15, 9, 6, 35};

    for (int& val : toDelete) {
        tree.remove(val);
    }

    tree.inorder();
}
