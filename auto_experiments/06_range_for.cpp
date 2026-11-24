#include <iostream>
#include <vector>

int main() {
    std::vector<int> values{1, 2, 3};

    for (auto value : values) {
        value *= 10;
    }

    std::cout << "after auto:";
    for (const auto& value : values) {
        std::cout << ' ' << value;
    }
    std::cout << '\n';

    for (auto& value : values) {
        value *= 10;
    }

    std::cout << "after auto&:";
    for (const auto& value : values) {
        std::cout << ' ' << value;
    }
    std::cout << '\n';
}
