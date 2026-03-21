#include <iostream>

void countSort(int* array, int n) {
    if (n <= 1) return;

    int k = array[0];
    for (int i = 1; i < n; ++i) {
        k = std::max(k, array[i]);
    }

    int* cnt = new int[k + 1]();
    for (int i = 0; i < n; ++i) {
        ++cnt[array[i]];
    }
    for (int i = 1; i <= k; ++i) {
        cnt[i] += cnt[i - 1];
    }

    int* out = new int[n];
    for (int i = n - 1; i >= 0; --i) {
        out[--cnt[array[i]]] = array[i];
    }
    for (int i = 0; i < n; ++i) {
        array[i] = out[i];
    }

    delete[] out;
    delete[] cnt;
}

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);

    int array[] = {2, 4, 7, 5, 8, 9, 3, 1, 6, 0};
    int n = sizeof(array) / sizeof(int);
    countSort(array, n);

    for (auto &e : array) std::cout << e << " ";
    std::cout << '\n';
}
