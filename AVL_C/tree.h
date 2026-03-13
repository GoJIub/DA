#pragma once

typedef struct Node Node;

struct Node {
    int value;
    Node* left;
    Node* right;
    Node* parent;
    int balance;
};

typedef Node *tree;

tree createEmpty();
tree build(int val, tree left, tree right);
void destroy(tree* t);

bool isEmpty(tree t);
int getValue(tree t);
tree getLeft(tree t);
tree getRight(tree t);

bool add(tree* t, int val);
bool remove(tree* t, int val);

void print(tree t, int lvl = 0);
