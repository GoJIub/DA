#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

class SufTree {

    struct Node {
        int beg;
        int* end;
        int idx;
        std::unordered_map<char, Node*> children;
        Node* suf_link;

        Node(int beg, int* end, int idx = -1)
        : beg(beg), end(end), idx(idx), suf_link(nullptr) {}

        int edge_len() const {
            return *end - beg + 1;
        }
    };

public:

    SufTree(const std::string& text) {
        root = new Node(-1, new int(-1));
        root->suf_link = root;
        active_node = root;

        for (char c : text) extend(c);
        extend('$');
    }

    std::vector<int> search(const std::string& pattern) {
        std::vector<int> entries;

        Node* cur = root;
        int i = 0;
        int n = pattern.size();
        while (i < n) {
            if (!cur->children.count(pattern[i])) return entries;

            Node* child = cur->children[pattern[i]];
            int l = child->beg, r = *child->end;
            for (int j = l; j <= r; ++j) {
                if (i >= n) break;
                if (pattern[i] != text[j]) return entries;
                ++i;
            }
            cur = child;
            if (i >= n) break;
        }

        collect(cur, entries);
        return entries;
    }

private:

    std::string text;

    Node* root;
    int   global_end{-1};

    Node* active_node{nullptr};
    int   active_edge{0};
    int   active_length{0};
    int   remaining{0};

    Node* make_leaf(int idx) {
        return new Node(global_end, &global_end, idx);
    }

    Node* make_internal(int beg, int end) {
        return new Node(beg, new int(end), -1);
    }

    void update_active_point() {
        if (active_node == root && active_length > 0) {
            --active_length;
            active_edge = global_end - remaining + 1;
        } else if (active_node != root) {
            active_node = active_node->suf_link;
        }
    }

    void extend(char c) {
        text += c;
        ++global_end;
        ++remaining;

        Node* last_internal = nullptr;

        while (remaining > 0) {
            if (active_length == 0) active_edge = global_end;

            if (!active_node->children.count(text[active_edge])) {
                active_node->children[text[active_edge]] = make_leaf(global_end - remaining + 1);
                if (last_internal) { last_internal->suf_link = active_node; last_internal = nullptr; }
                --remaining;
                update_active_point();
            } else {
                Node* child = active_node->children[text[active_edge]];

                if (active_length >= child->edge_len()) {
                    active_edge += child->edge_len();
                    active_length -= child->edge_len();
                    active_node = child;
                    continue;
                }

                if (text[child->beg + active_length] == c) {
                    ++active_length;
                    if (last_internal) { last_internal->suf_link = active_node; last_internal = nullptr; }
                    break;
                }

                int old_beg = child->beg;
                Node* internal = make_internal(old_beg, old_beg + active_length - 1);
                child->beg = old_beg + active_length;
                internal->children[text[child->beg]] = child;
                internal->children[c] = make_leaf(global_end - remaining + 1);
                active_node->children[text[old_beg]] = internal;

                if (last_internal) last_internal->suf_link = internal;
                last_internal = internal;

                --remaining;
                update_active_point();
            }
        }
        if (last_internal) last_internal->suf_link = root;
    }

    void collect(Node* cur, std::vector<int>& v) {
        if (cur->children.empty()) {
            v.push_back(cur->idx);
        } else {
            for (auto& [key, val] : cur->children) collect(val, v);
        }
    }

};

int main() {
    std::string text;
    std::cin >> text;

    SufTree st(text);

    int i = 0;
    std::string pattern;
    while (std::cin >> pattern) {
        ++i;
        std::vector<int> ans = st.search(pattern);
        if (ans.empty()) continue;
        std::sort(ans.begin(), ans.end());
        std::cout << i << ": ";
        for (int j = 0; j < (int)ans.size(); ++j) {
            if (j != 0) std::cout << ", ";
            std::cout << ans[j] + 1;
        }
        std::cout << "\n";
    }
}