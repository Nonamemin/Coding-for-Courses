#include <iostream>
#include <type_traits>

int main() {
    const int source = 7;
    auto& source_ref = source;

    int number = 20;
    auto&& lvalue_ref = number;
    auto&& temporary_ref = 30;

    static_assert(std::is_same_v<decltype(source_ref), const int&>);
    static_assert(std::is_same_v<decltype(lvalue_ref), int&>);
    static_assert(std::is_same_v<decltype(temporary_ref), int&&>);

    lvalue_ref = 21;

    std::cout << "source_ref = " << source_ref << '\n';
    std::cout << "number = " << number << '\n';
    std::cout << "temporary_ref = " << temporary_ref << '\n';
}
