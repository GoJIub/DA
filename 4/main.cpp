#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

void toLowerCase(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
}

std::vector<int> computeZ(const std::vector<std::string>& s) {
    int n = s.size();
    std::vector<int> z(n, 0);
    
    int l = 0, r = 0;
    for (int i = 1; i < n; ++i) {
        if (i <= r) {
            z[i] = std::min(r - i + 1, z[i - l]);
        }
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            ++z[i];
        }
        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        }
    }

    return z;
}


int main() {
    std::string word;

    std::string p;
    std::getline(std::cin, p);

    std::vector<std::string> patterns;
    std::stringstream pss(p);
    while (pss >> word) {
        toLowerCase(word);
        patterns.push_back(word);
    }

    std::string t;
    int curLine = 0;
    std::vector<std::string> text;
    std::vector<std::pair<int, int>> info;
    while (std::getline(std::cin, t)) {
        ++curLine;
        int curWord = 0;
        std::stringstream tss(t);
        while (tss >> word) {
            ++curWord;
            toLowerCase(word);
            text.push_back(word);
            info.push_back({curLine, curWord});
        }
    }

    std::vector<std::string> s;
    s.reserve(patterns.size() + text.size() + 1);
    s.insert(s.end(), patterns.begin(), patterns.end());
    s.push_back("$");
    s.insert(s.end(), text.begin(), text.end());

    std::vector<int> z = computeZ(s);

    int m = patterns.size();
    for (int i = 1; i < s.size(); ++i) {
        if (z[i] == m) {
            auto cur = info[i - m - 1];
            std::cout << cur.first << ", " << cur.second << "\n";
        }
    }
}