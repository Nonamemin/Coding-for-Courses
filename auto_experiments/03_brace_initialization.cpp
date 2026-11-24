#include <initializer_list>
#include <iostream>
#include <type_traits>

int main() {
    auto one{1};
    auto numbers = {1, 2, 3};

    static_assert(std::is_same_v<decltype(one), int>);
    static_assert(
        std::is_same_v<decltype(numbers), std::initializer_list<int>>
    );

    std::cout << "one = " << one << '\n';
    std::cout << "numbers =";
    for (int number : numbers) {
        std::cout << ' ' << number;
    }
    std::cout << '\n';
}
