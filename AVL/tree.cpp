#include "tree.h"

#include <iostream>

namespace {

void updateBalance(tree* root, tree node, bool is_left, bool is_add);

tree* linkToNode(tree* root, tree node) {
    if (node -> parent == nullptr) return root;
    if (node -> parent -> left == node) return &node -> parent -> left;
    return &node -> parent -> right;
}

void rotateRight(tree* link) {
    Node* a = *link;
    Node* b = a -> left;
    Node* beta = b -> right;

    b -> right = a;
    b -> parent = a -> parent;

    a -> left = beta;
    a -> parent = b;
    if (beta != nullptr) beta -> parent = a;

    *link = b;
}

void rotateLeft(tree* link) {
    Node* a = *link;
    Node* b = a -> right;
    Node* beta = b -> left;

    b -> left = a;
    b -> parent = a -> parent;

    a -> right = beta;
    a -> parent = b;
    if (beta != nullptr) beta -> parent = a;

    *link = b;
}

void rotate(tree* root, tree node) {
    tree* link = linkToNode(root, node);

    if (node -> balance == 2) {
        Node* left = node -> left;
        if (left == nullptr) return;

        // LL
        if (left -> balance >= 0) {
            int leftBalance = left -> balance;
            rotateRight(link);

            if (leftBalance == 0) {
                node -> balance = 1;
                (*link) -> balance = -1;
            } else {
                node -> balance = 0;
                (*link) -> balance = 0;
            }
            return;
        }

        // LR
        Node* mid = left -> right;
        int midBalance = mid -> balance;

        rotateLeft(&node -> left);
        rotateRight(link);

        if (midBalance == 1) {
            node -> balance = -1;
            left -> balance = 0;
        } else if (midBalance == 0) {
            node -> balance = 0;
            left -> balance = 0;
        } else { // midBalance == -1
            node -> balance = 0;
            left -> balance = 1;
        }
        (*link) -> balance = 0;
        return;
    }

    if (node -> balance == -2) {
        Node* right = node -> right;
        if (right == nullptr) return;

        // RR
        if (right -> balance <= 0) {
            int rightBalance = right -> balance;
            rotateLeft(link);

            if (rightBalance == 0) {
                node -> balance = -1;
                (*link) -> balance = 1;
            } else {
                node -> balance = 0;
                (*link) -> balance = 0;
            }
            return;
        }

        // RL
        Node* mid = right -> left;
        int midBalance = mid -> balance;

        rotateRight(&node -> right);
        rotateLeft(link);

        if (midBalance == 1) {
            node -> balance = 0;
            right -> balance = -1;
        } else if (midBalance == 0) {
            node -> balance = 0;
            right -> balance = 0;
        } else { // midBalance == -1
            node -> balance = 1;
            right -> balance = 0;
        }
        (*link) -> balance = 0;
    }
}

bool addImpl(tree* root, tree* t, int value) {
    if (isEmpty(*t)) {
        *t = new Node{value, nullptr, nullptr, nullptr, 0};
        return true;
    }
    if (value < (*t) -> value) {
        if (isEmpty((*t) -> left)) {
            (*t) -> left = new Node{value, nullptr, nullptr, *t, 0};
            updateBalance(root, *t, true, true);
            return true;
        }
        return addImpl(root, &(*t) -> left, value);
    }
    if (value > (*t) -> value) {
        if (isEmpty((*t) -> right)) {
            (*t) -> right = new Node{value, nullptr, nullptr, *t, 0};
            updateBalance(root, *t, false, true);
            return true;
        }
        return addImpl(root, &(*t) -> right, value);
    }
    return false;
}

bool removeImpl(tree* root, tree* t, int value) {
    if (isEmpty(*t)) return false;

    if (value < (*t) -> value) {
        return removeImpl(root, &(*t) -> left, value);
    }
    if (value > (*t) -> value) {
        return removeImpl(root, &(*t) -> right, value);
    }

    Node* node = *t;
    if (isEmpty(node -> left) || isEmpty(node -> right)) {
        Node* parent = node -> parent;
        bool is_left_child = parent != nullptr && parent -> left == node;
        tree child = isEmpty(node -> left) ? node -> right : node -> left;

        *t = child;
        if (!isEmpty(child)) child -> parent = parent;
        delete node;

        if (parent != nullptr) updateBalance(root, parent, is_left_child, false);
        return true;
    }

    tree pred = node -> left;
    while (!isEmpty(pred -> right)) pred = pred -> right;
    int replacement = pred -> value;
    node -> value = replacement;
    return removeImpl(root, &node -> left, replacement);
}

void updateBalance(tree* root, tree node, bool is_left, bool is_add) {
    if (node == nullptr) return;
    node -> balance += (is_left == is_add) ? 1 : -1;

    if (is_add) {
        switch (node -> balance) {
            case 0:
                return;
            case -1:
            case 1:
                break;
            case -2:
            case 2:
                rotate(root, node);
                return;
        }
    } else {
        switch (node -> balance) {
            case 0:
                break;
            case -1:
            case 1:
                return;
            case -2:
            case 2:
                rotate(root, node);
                return;
        }
    }

    Node* parent = node -> parent;
    if (parent == nullptr) return;

    bool is_left_child = (parent -> left == node);
    updateBalance(root, parent, is_left_child, is_add);
}

} // namespace

tree createEmpty() {
    return nullptr;
}

tree build(int value, tree left, tree right) {
    tree node = new Node{value, left, right, nullptr, 0};
    if (!isEmpty(left)) left -> parent = node;
    if (!isEmpty(right)) right -> parent = node;
    return node;
}

void destroy(tree* t) {
    if (t == nullptr || isEmpty(*t)) return;
    destroy(&(*t) -> left);
    destroy(&(*t) -> right);
    delete *t;
    *t = nullptr;
}

bool isEmpty(tree t) {
    return t == nullptr;
}

int getValue(tree t) {
    return t -> value;
}

tree getLeft(tree t) {
    return t -> left;
}

tree getRight(tree t) {
    return t -> right;
}

bool add(tree* t, int value) {
    return addImpl(t, t, value);
}

bool remove(tree* t, int value) {
    return removeImpl(t, t, value);
}

void print(tree t, int lvl) {
    if (isEmpty(t)) return;
    print(getRight(t), lvl + 1);
    for (int i = 0; i < lvl; ++i) std::cout << '\t';
    std::cout << getValue(t) << " [" << t -> balance << "]\n";
    print(getLeft(t), lvl + 1);
}
