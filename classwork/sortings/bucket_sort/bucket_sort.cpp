#include <iostream>
#include <vector>
#include <algorithm>

void bucketSort(std::vector<double> &nums) {
    int n = nums.size();
    if (n == 0) return;

    std::vector<std::vector<double>> buckets(n);
    for (double &num : nums) {
        int idx = num * n;
        buckets[idx].push_back(num);
    }
    for (int i = 0; i < n; ++i) {
        std::sort(buckets[i].begin(), buckets[i].end());
    }
    int idx = 0;
    for (int i = 0; i < buckets.size(); ++i) {
        for (const auto &e : buckets[i]) {
            nums[idx] = e;
            ++idx;
        }
    }
}

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);

    std::vector<double> array = {.255, .17, .999, .9, .80, .91, .356, .11, .666, 0};
    bucketSort(array);

    for (auto &e : array) std::cout << e << " ";
    std::cout << '\n';
}