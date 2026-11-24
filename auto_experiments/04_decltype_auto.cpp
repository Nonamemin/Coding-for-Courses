#include <iostream>
#include <type_traits>

int shared_value = 10;

auto return_by_value() {
    return (shared_value);
}

decltype(auto) return_by_reference() {
    return (shared_value);
}

int main() {
    auto copied_result = return_by_value();
    decltype(auto) referenced_result = return_by_reference();

    static_assert(std::is_same_v<decltype(copied_result), int>);
    static_assert(std::is_same_v<decltype(referenced_result), int&>);

    copied_result = 100;
    std::cout << "shared_value after changing copy = " << shared_value << '\n';

    referenced_result = 200;
    std::cout << "shared_value after changing reference = " << shared_value << '\n';
}
