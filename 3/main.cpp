#include <exception>
#include <iostream>
#include <string>

#include "patricia.hpp"

int main() {
    PATRICIA dictionary;

    std::string cmd;
    while (std::cin >> cmd) {
        try {
            if (cmd == "+") {
                std::string word;
                uint64_t value;
                std::cin >> word >> value;

                if (dictionary.insert(word, value)) std::cout << "OK\n";
                else std::cout << "Exist\n";
            } else if (cmd == "-") {
                std::string word;
                std::cin >> word;

                if (dictionary.remove(word)) std::cout << "OK\n";
                else std::cout << "NoSuchWord\n";
            } else if (cmd == "!") {
                std::string action;
                std::string path;
                std::cin >> action >> path;

                if (action == "Save") {
                    std::string err = dictionary.save(path);
                    if (err.empty()) std::cout << "OK\n";
                    else std::cout << "ERROR: " << err << "\n";
                } else if (action == "Load") {
                    std::string err = dictionary.load(path);
                    if (err.empty()) std::cout << "OK\n";
                    else std::cout << "ERROR: " << err << "\n";
                } else {
                    std::cout << "ERROR: unknown command\n";
                }
            } else {
                auto result = dictionary.search(cmd);

                if (result) std::cout << "OK: " << *result << "\n";
                else std::cout << "NoSuchWord\n";
            }
        } catch (const std::bad_alloc&) {
            std::cout << "ERROR: not enough memory\n";
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << "\n";
        }
    }
}
