/*
 * Бинарное дерево поиска + Декартово дерево (Treap)
 * Реализация на C++17
 */

#include <iostream>
#include <random>
#include <stdexcept>

// ============================================================
//  БИНАРНОЕ ДЕРЕВО ПОИСКА (BST)
// ============================================================

struct BSTNode {
    int   key;
    BSTNode* left  = nullptr;
    BSTNode* right = nullptr;
    BSTNode* parent = nullptr;
    explicit BSTNode(int k) : key(k) {}
};

class BST {
public:
    ~BST() { clear(root_); }

    // ── Поиск ──────────────────────────────────────────────
    BSTNode* search(int key) const {
        BSTNode* cur = root_;
        while (cur) {
            if      (key < cur->key) cur = cur->left;
            else if (key > cur->key) cur = cur->right;
            else                     return cur;
        }
        return nullptr;
    }

    // ── Вставка ────────────────────────────────────────────
    void insert(int key) {
        BSTNode* parent = nullptr;
        BSTNode* cur    = root_;
        while (cur) {
            parent = cur;
            if      (key < cur->key) cur = cur->left;
            else if (key > cur->key) cur = cur->right;
            else return;                    // дубликаты игнорируем
        }
        BSTNode* node = new BSTNode(key);
        node->parent  = parent;
        if (!parent)            root_         = node;
        else if (key < parent->key) parent->left  = node;
        else                        parent->right = node;
    }

    // ── Удаление ───────────────────────────────────────────
    bool remove(int key) {
        BSTNode* node = search(key);
        if (!node) return false;
        deleteNode(node);
        return true;
    }

    // ── Одинарные повороты ────────────────────────────────
    bool rotateLeft(int key) {
        BSTNode* x = search(key);
        if (!x || !x->right) return false;
        rotateLeft(x);
        return true;
    }

    bool rotateRight(int key) {
        BSTNode* x = search(key);
        if (!x || !x->left) return false;
        rotateRight(x);
        return true;
    }

    // ── Двойные повороты (AVL) ─────────────────────────────
    //
    // Left-Right (LR): левый поворот вокруг левого сына,
    //                  затем правый поворот вокруг x.
    //
    //       x               x              z
    //      / \             / \            / \
    //     y   D   →LL→   z   D   →RR→  y   x
    //    / \             / \           / \ / \
    //   A   z           y   C         A  B C  D
    //      / \         / \
    //     B   C       A   B
    //
    bool rotateLeftRight(int key) {
        BSTNode* x = search(key);
        if (!x || !x->left || !x->left->right) return false;
        rotateLeft(x->left);
        rotateRight(x);
        return true;
    }

    // Right-Left (RL): правый поворот вокруг правого сына,
    //                  затем левый поворот вокруг x.
    //
    //     x               x                z
    //    / \             / \              / \
    //   A   y   →RR→   A   z   →LL→    x   y
    //      / \             / \         / \ / \
    //     z   D           B   y       A  B C  D
    //    / \                 / \
    //   B   C               C   D
    //
    bool rotateRightLeft(int key) {
        BSTNode* x = search(key);
        if (!x || !x->right || !x->right->left) return false;
        rotateRight(x->right);
        rotateLeft(x);
        return true;
    }

    // ── Вывод дерева (in-order) ────────────────────────────
    void printInOrder() const {
        inOrder(root_);
        std::cout << '\n';
    }

    void printTree() const {
        printHelper(root_, "", false);
    }

private:
    BSTNode* root_ = nullptr;

    // Вспомогательная функция: замена поддерева u на поддерево v
    void transplant(BSTNode* u, BSTNode* v) {
        if (!u->parent)             root_          = v;
        else if (u == u->parent->left)  u->parent->left  = v;
        else                            u->parent->right = v;
        if (v) v->parent = u->parent;
    }

    BSTNode* minimum(BSTNode* node) const {
        while (node->left) node = node->left;
        return node;
    }

    void deleteNode(BSTNode* node) {
        if (!node->left) {
            transplant(node, node->right);
        } else if (!node->right) {
            transplant(node, node->left);
        } else {
            // Находим successor (минимум правого поддерева)
            BSTNode* suc = minimum(node->right);
            if (suc->parent != node) {
                transplant(suc, suc->right);
                suc->right         = node->right;
                suc->right->parent = suc;
            }
            transplant(node, suc);
            suc->left         = node->left;
            suc->left->parent = suc;
        }
        delete node;
    }

    // x должен иметь правого сына y
    void rotateLeft(BSTNode* x) {
        BSTNode* y  = x->right;
        x->right    = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent)               root_          = y;
        else if (x == x->parent->left) x->parent->left  = y;
        else                           x->parent->right = y;
        y->left   = x;
        x->parent = y;
    }

    // x должен иметь левого сына y
    void rotateRight(BSTNode* x) {
        BSTNode* y  = x->left;
        x->left     = y->right;
        if (y->right) y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent)                root_          = y;
        else if (x == x->parent->right) x->parent->right = y;
        else                            x->parent->left  = y;
        y->right  = x;
        x->parent = y;
    }

    void inOrder(BSTNode* node) const {
        if (!node) return;
        inOrder(node->left);
        std::cout << node->key << ' ';
        inOrder(node->right);
    }

    void printHelper(BSTNode* node, const std::string& prefix, bool isLeft) const {
        if (!node) return;
        std::cout << prefix;
        std::cout << (isLeft ? "├── " : "└── ");
        std::cout << node->key << '\n';
        printHelper(node->left,  prefix + (isLeft ? "│   " : "    "), true);
        printHelper(node->right, prefix + (isLeft ? "│   " : "    "), false);
    }

    void clear(BSTNode* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }
};


// ============================================================
//  ДЕКАРТОВО ДЕРЕВО / TREAP
// ============================================================
//
//  Каждый узел хранит:
//    key      — ключ (BST-порядок по ключу)
//    priority — приоритет (heap-порядок; макс. куча)
//
//  Основные операции:
//    split(t, k, l, r)  — разбивает t на l (key≤k) и r (key>k)
//    merge(l, r)        — сливает два treap-а (все ключи l < ключей r)
//    insert(t, key)     — вставка через split + merge
//    remove(t, key)     — удаление через split + merge
// ============================================================

struct TreapNode {
    int  key;
    int  priority;
    TreapNode* left  = nullptr;
    TreapNode* right = nullptr;
    TreapNode(int k, int p) : key(k), priority(p) {}
};

class Treap {
public:
    explicit Treap() : rng_(std::random_device{}()) {}
    ~Treap() { clear(root_); }

    // ── Split ──────────────────────────────────────────────
    // Разбивает дерево на два:
    //   l — узлы с ключами ≤ key
    //   r — узлы с ключами >  key
    void split(int key, TreapNode*& l, TreapNode*& r) {
        split(root_, key, l, r);
        root_ = nullptr;          // дерево «опустошено»
    }

    // Версия без изменения текущего дерева (для внутреннего использования)
    static void split(TreapNode* t, int key,
                      TreapNode*& l, TreapNode*& r) {
        if (!t) { l = r = nullptr; return; }
        if (t->key <= key) {
            split(t->right, key, t->right, r);
            l = t;
        } else {
            split(t->left, key, l, t->left);
            r = t;
        }
    }

    // ── Merge ──────────────────────────────────────────────
    // Сливает два дерева (все ключи left < ключей right)
    static TreapNode* merge(TreapNode* l, TreapNode* r) {
        if (!l) return r;
        if (!r) return l;
        if (l->priority >= r->priority) {
            l->right = merge(l->right, r);
            return l;
        } else {
            r->left = merge(l, r->left);
            return r;
        }
    }

    // ── Вставка ────────────────────────────────────────────
    void insert(int key) {
        if (search(key)) return;            // дубликаты игнорируем
        auto* node = new TreapNode(key, randomPriority());
        TreapNode* l = nullptr;
        TreapNode* r = nullptr;
        split(root_, key - 1, l, r);       // l: ключи < key, r: ключи ≥ key
        root_ = merge(merge(l, node), r);
    }

    // ── Удаление ───────────────────────────────────────────
    bool remove(int key) {
        if (!search(key)) return false;
        TreapNode* l  = nullptr;
        TreapNode* m  = nullptr;
        TreapNode* r  = nullptr;
        split(root_, key - 1, l, m);       // l: < key,  m: ≥ key
        split(m,     key,     m, r);       // m: == key, r: > key
        delete m;                           // m — один узел с данным ключом
        root_ = merge(l, r);
        return true;
    }

    // ── Поиск ──────────────────────────────────────────────
    bool search(int key) const {
        TreapNode* cur = root_;
        while (cur) {
            if      (key < cur->key) cur = cur->left;
            else if (key > cur->key) cur = cur->right;
            else                     return true;
        }
        return false;
    }

    // ── Вывод ──────────────────────────────────────────────
    void printInOrder() const {
        inOrder(root_);
        std::cout << '\n';
    }

    void printTree() const {
        printHelper(root_, "", false);
    }

    TreapNode* root_ = nullptr;     // публичный только для демо split/merge

private:
    std::mt19937 rng_;

    int randomPriority() {
        return std::uniform_int_distribution<int>(1, 1'000'000)(rng_);
    }

    void inOrder(TreapNode* node) const {
        if (!node) return;
        inOrder(node->left);
        std::cout << node->key << "(p=" << node->priority << ") ";
        inOrder(node->right);
    }

    void printHelper(TreapNode* node,
                     const std::string& prefix, bool isLeft) const {
        if (!node) return;
        std::cout << prefix;
        std::cout << (isLeft ? "├── " : "└── ");
        std::cout << node->key << " [p=" << node->priority << "]\n";
        printHelper(node->left,  prefix + (isLeft ? "│   " : "    "), true);
        printHelper(node->right, prefix + (isLeft ? "│   " : "    "), false);
    }

    void clear(TreapNode* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }
};


// ============================================================
//  ДЕМОНСТРАЦИЯ
// ============================================================

void demoBST() {
    std::cout << "======================================\n";
    std::cout << "   БИНАРНОЕ ДЕРЕВО ПОИСКА (BST)\n";
    std::cout << "======================================\n\n";

    BST bst;
    for (int v : {50, 30, 70, 20, 40, 60, 80}) bst.insert(v);

    std::cout << "Дерево после вставки {50,30,70,20,40,60,80}:\n";
    bst.printTree();
    std::cout << "In-order: ";
    bst.printInOrder();

    std::cout << "\nПоиск 40: " << (bst.search(40) ? "найден" : "не найден") << '\n';
    std::cout << "Поиск 99: " << (bst.search(99) ? "найден" : "не найден") << '\n';

    // --- одинарные повороты ---
    std::cout << "\n--- Одинарный левый поворот вокруг 30 ---\n";
    bst.rotateLeft(30);
    bst.printTree();

    std::cout << "\n--- Одинарный правый поворот вокруг 50 ---\n";
    bst.rotateRight(50);
    bst.printTree();

    // --- двойной LR (зигзаг влево-вправо) ---
    //   10
    //   /
    //  5
    //   \
    //    8
    std::cout << "\n--- Двойной Left-Right (LR) поворот вокруг 10 ---\n";
    std::cout << "  Исходное дерево:\n";
    BST lr;
    for (int v : {10, 5, 8}) lr.insert(v);
    lr.printTree();
    lr.rotateLeftRight(10);
    std::cout << "  После LR:\n";
    lr.printTree();

    // --- двойной RL (зигзаг вправо-влево) ---
    //   10
    //     \
    //     20
    //     /
    //    15
    std::cout << "\n--- Двойной Right-Left (RL) поворот вокруг 10 ---\n";
    std::cout << "  Исходное дерево:\n";
    BST rl;
    for (int v : {10, 20, 15}) rl.insert(v);
    rl.printTree();
    rl.rotateRightLeft(10);
    std::cout << "  После RL:\n";
    rl.printTree();

    // --- удаление ---
    std::cout << "\n--- Удаление 50 (два сына) ---\n";
    bst.remove(50);
    bst.printTree();
    std::cout << "In-order: ";
    bst.printInOrder();
}

void demoTreap() {
    std::cout << "\n======================================\n";
    std::cout << "     ДЕКАРТОВО ДЕРЕВО (TREAP)\n";
    std::cout << "======================================\n\n";

    Treap treap;
    for (int v : {5, 2, 8, 1, 4, 7, 10}) treap.insert(v);

    std::cout << "Дерево после вставки {5,2,8,1,4,7,10}:\n";
    treap.printTree();
    std::cout << "In-order: ";
    treap.printInOrder();

    std::cout << "\nПоиск 4: " << (treap.search(4) ? "найден" : "не найден") << '\n';
    std::cout << "Поиск 6: " << (treap.search(6) ? "найден" : "не найден") << '\n';

    std::cout << "\nУдаление 5:\n";
    treap.remove(5);
    treap.printTree();
    std::cout << "In-order: ";
    treap.printInOrder();

    // Демонстрация split / merge
    std::cout << "\nSplit по ключу 4 (l ≤ 4, r > 4):\n";
    TreapNode* l = nullptr;
    TreapNode* r = nullptr;
    Treap::split(treap.root_, 4, l, r);
    treap.root_ = nullptr;          // treap больше не владеет узлами

    // Печатаем без создания Treap-объектов, чтобы избежать двойного delete
    auto printRaw = [](auto& self, TreapNode* node,
                       const std::string& prefix, bool isLeft) -> void {
        if (!node) return;
        std::cout << prefix << (isLeft ? "├── " : "└── ")
                  << node->key << " [p=" << node->priority << "]\n";
        self(self, node->left,  prefix + (isLeft ? "│   " : "    "), true);
        self(self, node->right, prefix + (isLeft ? "│   " : "    "), false);
    };

    std::cout << "  Левая часть (≤4):\n";
    printRaw(printRaw, l, "  ", false);

    std::cout << "  Правая часть (>4):\n";
    printRaw(printRaw, r, "  ", false);

    std::cout << "\nMerge обратно:\n";
    TreapNode* merged = Treap::merge(l, r);

    // Отдаём объединённое дерево новому Treap-объекту
    Treap result;
    result.root_ = merged;
    result.printTree();
    std::cout << "In-order: ";
    result.printInOrder();
}

int main() {
    demoBST();
    demoTreap();
    return 0;
}
