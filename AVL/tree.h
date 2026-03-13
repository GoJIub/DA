#pragma once

template <typename T>
class AVLTree {
public:
    AVLTree();
    ~AVLTree();

    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    bool empty() const;
    bool add(const T& value);
    bool remove(const T& value);
    void print(int level = 0) const;
    void clear();

private:
    struct Node {
        T value;
        Node* left;
        Node* right;
        Node* parent;
        int balance;
    };

    Node* root_;

    Node* build(const T& value, Node* left, Node* right);
    void destroy(Node*& node);
    Node** linkToNode(Node* node);
    void rotateLeft(Node** link);
    void rotateRight(Node** link);
    void rotate(Node* node);
    void updateBalance(Node* node, bool isLeft, bool isAdd);
    bool addImpl(Node*& node, const T& value, Node* parent);
    bool removeImpl(Node*& node, const T& value);

    static bool isEmpty(Node* node);
    static const T& getValue(Node* node);
    static Node* getLeft(Node* node);
    static Node* getRight(Node* node);
};
