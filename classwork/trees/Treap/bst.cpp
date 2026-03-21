#include <iostream>
#include <vector>

class BST {
    struct TNode {
        int val;
        int priority;
        TNode* left;
        TNode* right;
        TNode(int val) : val(val), priority(::rand()), left(nullptr), right(nullptr) {}
    };

public:
    TNode* search(int val) { return find(root, val); }

    void insert(int val) {
        TNode* t1;
        TNode* t2;

        split(root, val, t1, t2);
        t1 = merge(t1, new TNode(val));
        root = merge(t1, t2);
    }

    void remove(int val) {
        TNode* t1;
        TNode* t2;
        TNode* toDelete;

        split(root, val, t1, t2);
        split(t1, val - 1, t1, toDelete);
        delete toDelete;
        root = merge(t1, t2);
    }

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
            return find(cur -> left, val);
        } else {
            return find(cur -> right, val);
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
            } else if (cur -> left) {
                TNode* rest = cur -> left;
                delete cur;
                return rest;
            } else if (cur -> right) {
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

    TNode* merge(TNode* t1, TNode* t2) {
        if (t1 == nullptr) {
            return t2;
        }
        if (t2 == nullptr) {
            return t1;
        }

        if (t1 -> priority > t2 -> priority) {
            t1 -> right = merge(t1 -> right, t2);
            return t1;
        }

        t2 -> left = merge(t1, t2 -> left);
        return t2;
    }

    void split(TNode* t, int key, TNode*& t1, TNode*& t2) {
        if (t == nullptr) {
            t1 = t2 = nullptr;
            return;
        }
        if (t -> val <= key) {
            split(t -> right, key, t -> right, t2);
            t1 = t;
        } else {
            split(t -> left, key, t1, t -> left);
            t2 = t;
        }
    }

    std::pair<TNode*, TNode*> split(TNode* t, int key) {

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