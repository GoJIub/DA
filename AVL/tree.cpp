#include "tree.h"

#include <iostream>

template <typename T>
AVLTree<T>::AVLTree() : root_(nullptr) {}

template <typename T>
AVLTree<T>::~AVLTree() {
    clear();
}

template <typename T>
bool AVLTree<T>::empty() const {
    return isEmpty(root_);
}

template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::build(const T& value, Node* left, Node* right) {
    Node* node = new Node{value, left, right, nullptr, 0};
    if (!isEmpty(left)) left->parent = node;
    if (!isEmpty(right)) right->parent = node;
    return node;
}

template <typename T>
void AVLTree<T>::destroy(Node*& node) {
    if (isEmpty(node)) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
    node = nullptr;
}

template <typename T>
typename AVLTree<T>::Node** AVLTree<T>::linkToNode(Node* node) {
    if (node->parent == nullptr) return &root_;
    if (node->parent->left == node) return &node->parent->left;
    return &node->parent->right;
}

template <typename T>
void AVLTree<T>::rotateRight(Node** link) {
    Node* a = *link;
    Node* b = a->left;
    Node* beta = b->right;

    b->right = a;
    b->parent = a->parent;

    a->left = beta;
    a->parent = b;
    if (beta != nullptr) beta->parent = a;

    *link = b;
}

template <typename T>
void AVLTree<T>::rotateLeft(Node** link) {
    Node* a = *link;
    Node* b = a->right;
    Node* beta = b->left;

    b->left = a;
    b->parent = a->parent;

    a->right = beta;
    a->parent = b;
    if (beta != nullptr) beta->parent = a;

    *link = b;
}

template <typename T>
void AVLTree<T>::rotate(Node* node) {
    Node** link = linkToNode(node);

    if (node->balance == 2) {
        Node* left = node->left;
        if (left == nullptr) return;

        // LL
        if (left->balance >= 0) {
            int leftBalance = left->balance;
            rotateRight(link);

            if (leftBalance == 0) {
                node->balance = 1;
                (*link)->balance = -1;
            } else {
                node->balance = 0;
                (*link)->balance = 0;
            }
            return;
        }

        // LR
        Node* mid = left->right;
        int midBalance = mid->balance;

        rotateLeft(&node->left);
        rotateRight(link);

        if (midBalance == 1) {
            node->balance = -1;
            left->balance = 0;
        } else if (midBalance == 0) {
            node->balance = 0;
            left->balance = 0;
        } else { // midBalance == -1
            node->balance = 0;
            left->balance = 1;
        }
        (*link)->balance = 0;
        return;
    }

    if (node->balance == -2) {
        Node* right = node->right;
        if (right == nullptr) return;

        // RR
        if (right->balance <= 0) {
            int rightBalance = right->balance;
            rotateLeft(link);

            if (rightBalance == 0) {
                node->balance = -1;
                (*link)->balance = 1;
            } else {
                node->balance = 0;
                (*link)->balance = 0;
            }
            return;
        }

        // RL
        Node* mid = right->left;
        int midBalance = mid->balance;

        rotateRight(&node->right);
        rotateLeft(link);

        if (midBalance == 1) {
            node->balance = 0;
            right->balance = -1;
        } else if (midBalance == 0) {
            node->balance = 0;
            right->balance = 0;
        } else { // midBalance == -1
            node->balance = 1;
            right->balance = 0;
        }
        (*link)->balance = 0;
    }
}

template <typename T>
void AVLTree<T>::updateBalance(Node* node, bool isLeft, bool isAdd) {
    if (node == nullptr) return;
    node->balance += (isLeft == isAdd) ? 1 : -1;

    if (isAdd) {
        switch (node->balance) {
            case 0:
                return;
            case -1:
            case 1:
                break;
            case -2:
            case 2:
                rotate(node);
                return;
        }
    } else {
        switch (node->balance) {
            case 0:
                break;
            case -1:
            case 1:
                return;
            case -2:
            case 2:
                rotate(node);
                return;
        }
    }

    Node* parent = node->parent;
    if (parent == nullptr) return;

    bool isLeftChild = (parent->left == node);
    updateBalance(parent, isLeftChild, isAdd);
}

template <typename T>
bool AVLTree<T>::addImpl(Node*& node, const T& value, Node* parent) {
    if (isEmpty(node)) {
        node = new Node{value, nullptr, nullptr, parent, 0};
        return true;
    }

    if (value < node->value) {
        if (isEmpty(node->left)) {
            node->left = new Node{value, nullptr, nullptr, node, 0};
            updateBalance(node, true, true);
            return true;
        }
        return addImpl(node->left, value, node);
    }

    if (value > node->value) {
        if (isEmpty(node->right)) {
            node->right = new Node{value, nullptr, nullptr, node, 0};
            updateBalance(node, false, true);
            return true;
        }
        return addImpl(node->right, value, node);
    }

    return false;
}

template <typename T>
bool AVLTree<T>::removeImpl(Node*& node, const T& value) {
    if (isEmpty(node)) return false;

    if (value < node->value) return removeImpl(node->left, value);
    if (value > node->value) return removeImpl(node->right, value);

    Node* toDelete = node;
    if (isEmpty(toDelete->left) || isEmpty(toDelete->right)) {
        Node* parent = toDelete->parent;
        bool isLeftChild = parent != nullptr && parent->left == toDelete;
        Node* child = isEmpty(toDelete->left) ? toDelete->right : toDelete->left;

        node = child;
        if (!isEmpty(child)) child->parent = parent;
        delete toDelete;

        if (parent != nullptr) updateBalance(parent, isLeftChild, false);
        return true;
    }

    Node* pred = toDelete->left;
    while (!isEmpty(pred->right)) pred = pred->right;
    toDelete->value = pred->value;
    return removeImpl(toDelete->left, pred->value);
}

template <typename T>
bool AVLTree<T>::isEmpty(Node* node) {
    return node == nullptr;
}

template <typename T>
const T& AVLTree<T>::getValue(Node* node) {
    return node->value;
}

template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::getLeft(Node* node) {
    return node->left;
}

template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::getRight(Node* node) {
    return node->right;
}

template <typename T>
bool AVLTree<T>::add(const T& value) {
    return addImpl(root_, value, nullptr);
}

template <typename T>
bool AVLTree<T>::remove(const T& value) {
    return removeImpl(root_, value);
}

template <typename T>
void AVLTree<T>::print(int level) const {
    struct Printer {
        static void run(Node* node, int lvl) {
            if (node == nullptr) return;
            run(node->right, lvl + 1);
            for (int i = 0; i < lvl; ++i) std::cout << '\t';
            std::cout << node->value << " [" << node->balance << "]\n";
            run(node->left, lvl + 1);
        }
    };

    Printer::run(root_, level);
}

template <typename T>
void AVLTree<T>::clear() {
    destroy(root_);
}

// Explicit instantiation for current project type.
template class AVLTree<int>;
