#include "tree.h"

#include <iostream>

int main() {
    int command;
    tree t = createEmpty();
    int value;
    bool running = true;

    while (running) {
        std::cout << "Commands: \n"
                  << "1. Add node\n"
                  << "2. Remove node\n"
                  << "3. Print tree\n"
                  << ">> ";

        if (!(std::cin >> command)) break;
        switch (command) {
            case 1:
                std::cout << ">> Enter value: ";
                if (!(std::cin >> value)) {
                    running = false;
                    break;
                }
                add(&t, value);
                break;
            case 2:
                std::cout << ">> Enter value: ";
                if (!(std::cin >> value)) {
                    running = false;
                    break;
                }
                remove(&t, value);
                break;
            case 3:
                print(t);
                break;
            default:
                break;
        }
    }

    std::cout << '\n';
    destroy(&t);
}
