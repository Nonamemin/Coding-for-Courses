#include <iostream>
#include <type_traits>

int main() {
    const int source = 7;
    auto copied = source;

    static_assert(std::is_same_v<decltype(copied), int>);

    copied = 8;
    std::cout << "source = " << source << '\n';
    std::cout << "copied = " << copied << '\n';
    return 0;
}
