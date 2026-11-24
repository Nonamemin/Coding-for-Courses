#include <iostream>
#include <type_traits>
#include <vector>

int main() {
    std::vector<int> values{10, 20, 30};

    std::vector<int>::iterator explicit_iterator = values.begin();
    auto inferred_iterator = values.begin();

    static_assert(std::is_same_v<
        decltype(explicit_iterator),
        decltype(inferred_iterator)
    >);

    std::cout << "first value = " << *inferred_iterator << '\n';
}
