// FloatOrDouble.cpp
#include <cstdio>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <cxxabi.h>
#include <cstdlib>

template <class T>
std::string type_name() {
    int status = 0;
    char* p = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
    std::string r = (status == 0 && p) ? p : typeid(T).name();
    std::free(p);
    return r;
}

template <class T>
void report(const char* label, const T&) {
    std::printf("%-30s -> %s\n", label, type_name<T>().c_str());
}

int main()
{
    freopen("out.txt","w",stdout);
    // ===== 任务 1：浮点字面量 =====
    auto d  = 3.14;     // 无后缀 -> double
    auto f  = 3.14f;    // f 后缀 -> float
    auto ld = 3.14L;    // L 后缀 -> long double

    static_assert(std::is_same<decltype(d),  double>::value,      "3.14 should be double");
    static_assert(std::is_same<decltype(f),  float>::value,       "3.14f should be float");
    static_assert(std::is_same<decltype(ld), long double>::value, "3.14L should be long double");
    static_assert(!std::is_same<decltype(d), float>::value,       "3.14 is NOT float");

    std::puts("=== Task 1: float literals ===");
    report("auto d  = 3.14",  d);
    report("auto f  = 3.14f", f);
    report("auto ld = 3.14L", ld);

    // ===== 任务 2：字符串 / 字符 =====
    auto lit = "hello";                 // -> const char*
    auto ch  = 'h';                     // -> char
    auto s   = std::string("hello");    // -> std::string

    static_assert(std::is_same<decltype(lit), const char*>::value, "");
    static_assert(std::is_same<decltype(ch),  char>::value,        "");
    static_assert(std::is_same<decltype(s),   std::string>::value, "");
    static_assert(!std::is_same<decltype(lit), std::string>::value, "\"hello\" is NOT std::string");
    static_assert(!std::is_same<decltype(lit), char>::value,        "\"hello\" is NOT char");

    std::puts("\n=== Task 2: string / char ===");
    report("auto lit = \"hello\"",        lit);
    report("auto ch  = 'h'",              ch);
    report("auto s   = std::string(...)", s);

    return 0;
}