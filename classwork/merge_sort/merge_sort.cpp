#include <iostream>
#include <vector>

void merge(std::vector<int> &array, int l, int mid, int r) {
    std::vector<int> s(r - l);
    int i = l, j = mid, key = 0;

    while (i < mid && j < r) {
        if (array[i] <= array[j]) {
            s[key++] = array[i++];
        } else {
            s[key++] = array[j++];
        }
    }
    while (i < mid) {
        s[key++] = array[i++];
    }
    while (j < r) {
        s[key++] = array[j++];
    }

    for (int k = 0; k < key; ++k) {
        array[l + k] = s[k];
    }

}

void mergeSortRecursively(std::vector<int> &array, int l, int r) {
    if (r - l <= 1) return;
    int mid = l + (r - l) / 2;

    mergeSortRecursively(array, l, mid);
    mergeSortRecursively(array, mid, r);
    merge(array, l, mid, r);
}

void mergeSort(std::vector<int> &array) {
    mergeSortRecursively(array, 0, array.size());
}

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);

    std::vector<int> array = {2, 4, 7, 5, 8, 9, 3, 1, 6, 0};
    mergeSort(array);

    for (auto &e : array) std::cout << e << " ";
    std::cout << '\n';
}