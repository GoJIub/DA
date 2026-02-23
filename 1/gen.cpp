#include <iostream>
#include <random>
#include <string>

int main(int argc, char** argv)
{
    if (argc < 2)
        return 0;

    int n = std::stoi(argv[1]);

    std::mt19937 gen(123);
    std::uniform_int_distribution<int> key_dist(0, 999999);
    std::uniform_int_distribution<int> char_dist(0, 25);

    for (int i = 0; i < n; ++i) {
        int key = key_dist(gen);

        std::string s(64, 'a');
        for (char& c : s)
            c = char_dist(gen) + 'a';

        std::cout << key << '\t' << s << '\n';
    }
}