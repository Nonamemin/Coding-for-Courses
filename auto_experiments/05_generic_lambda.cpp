#include <iostream>
#include <type_traits>
#include <utility>

int main() {
    auto identity = [](auto&& value) -> decltype(auto) {
        return std::forward<decltype(value)>(value);
    };

    int original = 40;
    int& alias = identity(original);
    alias = 41;

    auto copied_temporary = identity(50);

    static_assert(std::is_same_v<decltype(alias), int&>);
    static_assert(std::is_same_v<decltype(copied_temporary), int>);

    std::cout << "original = " << original << '\n';
    std::cout << "copied_temporary = " << copied_temporary << '\n';
}
