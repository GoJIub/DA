#include <iostream>
#include <vector>
#include <cstdint>

void countSort(std::vector<uint64_t> &nums, int shift) {
    int k = 256;
    std::vector<int> cnt(k, 0);
    for (const auto &e : nums) {
        ++cnt[(e >> shift) & 0xFF];
    }
    for (int i = 1; i < k; ++i) {
        cnt[i] += cnt[i - 1];
    }
    std::vector<uint64_t> res(nums.size());
    for (int i = nums.size() - 1; i >= 0; --i) {
        res[--cnt[(nums[i] >> shift) & 0xFF]] = nums[i];
    }
    nums = std::move(res);
}

void radixSort(std::vector<uint64_t> &nums) {
    for (int shift = 0; shift < 256; shift += 8) {
        countSort(nums, shift);
    }
}

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);

    std::vector<uint64_t> array = {255, 17, 999, 9, 80, 91, 356, 11, 666, 0};
    radixSort(array);

    for (auto &e : array) std::cout << e << " ";
    std::cout << '\n';
}