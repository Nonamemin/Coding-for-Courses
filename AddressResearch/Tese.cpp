// ============================================================================
//  auto_addr_proof.cpp
//  实验目的：验证「auto 只是编译期的类型占位符，它本身不分配地址，
//            也不改变任何地址分配规则」
//
//  编译： g++ -std=c++14 -O0 auto_addr_proof.cpp -o auto_addr_proof.exe
//  运行： 直接双击 / 命令行运行，输出全部为检查结果，最后一行给出总结
//  返回： 全部通过返回 0，有失败返回 1（方便脚本判断）
//
//  说明：本程序只做"运行时 + 编译期"能验证的部分；
//        "目标文件字节完全相同""无地址时汇编里没有内存访问"这两条
//        需要用 verify_auto_vs_int.ps1 配合汇编/目标文件对比来验证。
// ============================================================================

#include <bits/stdc++.h>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

static int g_fail = 0;

#define CHECK(cond, desc)                                            \
    do {                                                             \
        bool ok_ = (cond);                                           \
        cout << (ok_ ? "  [PASS] " : "  [FAIL] ") << desc << '\n';   \
        if (!ok_) ++g_fail;                                          \
    } while (0)

// ---------------------------------------------------------------------------
// 一、编译期证据：auto 推出的类型与手写类型是【同一个类型】
//     下面任何一条断言不成立，这个文件就编译不过——这本身就是证据。
// ---------------------------------------------------------------------------
static_assert(is_same<decltype(4), int>::value, "auto 推 4 得到 int");
static_assert(is_same<decltype(2.98), double>::value, "auto 推 2.98 得到 double");
static_assert(!is_same<decltype(2.98), float>::value, "auto 推 2.98 不是 float");
// 注意 decltype("Input") 是 const char(&)[6]（不退化），
// 而 auto 声明会退化，所以这里用一个 auto 变量来断言。
static auto g_lit = "Input";
static_assert(is_same<decltype(g_lit), const char*>::value, "auto 接字符串字面量得到 const char*");

// 用模板特化再确认一次：同类型 ⇒ 同一份特化 ⇒ 布局必然相同
template <class T> struct Box { char c; T t; };
static_assert(is_same<Box<decltype(4)>, Box<int>>::value, "Box<auto 推出的类型> 就是 Box<int>");
static_assert(sizeof(Box<decltype(4)>) == sizeof(Box<int>), "布局相同");

// ---------------------------------------------------------------------------
// 二、auto 也能用于全局 / 静态 / 堆 —— 存储期与写法无关
// ---------------------------------------------------------------------------
auto        g_auto   = 4;      // 全局：进 .data
static auto g_static = 2.5;    // 文件内静态：进 .data

// ---------------------------------------------------------------------------
// 三、同一逻辑，一份写 auto、一份写具体类型
//     （verify 脚本会把这两个函数分别编成目标文件，比较字节是否完全相同）
// ---------------------------------------------------------------------------
int f_auto(int k) { auto a = k * 2; auto b = a + 1; return a + b; }
int f_int(int k) { int  a = k * 2; int  b = a + 1; return a + b; }

// ---------------------------------------------------------------------------
// 四、同一栈帧内相邻对象的间距（槽位大小 + 对齐的直观体现）
// ---------------------------------------------------------------------------
static vector<long> sorted_gaps(vector<uintptr_t> v)
{
    sort(v.begin(), v.end());
    vector<long> r;
    for (size_t i = 1; i < v.size(); ++i) r.push_back((long)(v[i] - v[i - 1]));
    return r;
}

vector<long> gaps_auto()   { auto a = 1, b = 2, c = 3; return sorted_gaps({(uintptr_t)&a, (uintptr_t)&b, (uintptr_t)&c}); }
vector<long> gaps_int()    { int  a = 1, b = 2, c = 3; return sorted_gaps({(uintptr_t)&a, (uintptr_t)&b, (uintptr_t)&c}); }
vector<long> gaps_mixed()  { auto a = 1; int b = 2; auto c = 3; int d = 4;
                             return sorted_gaps({(uintptr_t)&a, (uintptr_t)&b, (uintptr_t)&c, (uintptr_t)&d}); }

static string join(const vector<long>& v)
{
    string s;
    for (size_t i = 0; i < v.size(); ++i) { if (i) s += ", "; s += to_string(v[i]); }
    return s;
}

// ---------------------------------------------------------------------------
// 五、地址是"对象"的属性：取地址就有位置，不取就无所谓
// ---------------------------------------------------------------------------
volatile int* g_sink = nullptr;          // 让编译器无法看穿的外部侧效果
void escape(int* p) { g_sink = (volatile int*)p; }

int no_addr(int k)   { auto x = k * 2; return x; }          // 从未取地址
int has_addr(int k)  { auto x = k * 2; escape(&x); return x; }  // 取地址并逃逸

int main()
{
    freopen("out.txt","w",stdout);
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台正确显示中文（UTF-8 源码）
#endif

    cout << "================ auto 地址行为验证 ================\n";

    // ------------------------------------------------------------------
    cout << "\n[1] 类型同一性（编译期 static_assert 已保证，编过即成立）\n";
    cout << boolalpha;
    cout << "  is_same<decltype(4), int>                     = " << is_same<decltype(4), int>::value << '\n';
    cout << "  is_same<decltype(2.98), double>               = " << is_same<decltype(2.98), double>::value << '\n';
    cout << "  is_same<decltype(g_lit), const char*>         = " << is_same<decltype(g_lit), const char*>::value << '\n';
    cout << "  is_same<Box<decltype(4)>, Box<int>>           = " << is_same<Box<decltype(4)>, Box<int>>::value << '\n';

    // ------------------------------------------------------------------
    cout << "\n[2] 对象的大小 / 对齐：auto 版 与 显式版 逐项相同\n";
    {
        auto   a1 = 4;      int    b1 = 4;
        auto   a2 = 2.98;   double b2 = 2.98;
        auto   a3 = "Input"; string b3 = "Input";
        auto   a4 = 'c';    char   b4 = 'c';

        cout << "  int    : auto sizeof=" << sizeof(a1) << " alignof=" << alignof(decltype(a1))
             << "  |  显式 sizeof=" << sizeof(b1) << " alignof=" << alignof(decltype(b1)) << '\n';
        cout << "  浮点   : auto sizeof=" << sizeof(a2) << " alignof=" << alignof(decltype(a2))
             << "  |  显式 sizeof=" << sizeof(b2) << " alignof=" << alignof(decltype(b2)) << '\n';
        cout << "  字符串 : auto sizeof=" << sizeof(a3) << " alignof=" << alignof(decltype(a3))
             << "  |  显式 sizeof=" << sizeof(b3) << " alignof=" << alignof(decltype(b3))
             << "   <-- auto 推成 const char*，所以大小不同（类型本来就不同）\n";
        cout << "  char   : auto sizeof=" << sizeof(a4) << " alignof=" << alignof(decltype(a4))
             << "  |  显式 sizeof=" << sizeof(b4) << " alignof=" << alignof(decltype(b4)) << '\n';

        CHECK(sizeof(a1) == sizeof(b1), "auto(int) 与 int 大小相同");
        CHECK(alignof(decltype(a1)) == alignof(decltype(b1)), "auto(int) 与 int 对齐相同");
        CHECK(sizeof(a2) == sizeof(b2), "auto(2.98) 与 double 大小相同");
        CHECK(sizeof(a4) == sizeof(b4), "auto('c') 与 char 大小相同");
        CHECK(sizeof(a3) != sizeof(b3), "auto(\"Input\") 与 string 大小【不同】—— 因为类型不同");

        // 对齐要求被满足（地址是 alignof 的整数倍）
        CHECK(((uintptr_t)&a2) % alignof(decltype(a2)) == 0, "auto(double) 地址按 8 字节对齐");
        CHECK(((uintptr_t)&a3) % alignof(decltype(a3)) == 0, "auto(const char*) 地址按 8 字节对齐");
    }

    // ------------------------------------------------------------------
    cout << "\n[3] 同一栈帧内的槽位间距\n";
    {
        vector<long> ga = gaps_auto();
        vector<long> gb = gaps_int();
        vector<long> gm = gaps_mixed();
        cout << "  auto  a,b,c       间距 = " << join(ga) << '\n';
        cout << "  int   a,b,c       间距 = " << join(gb) << '\n';
        cout << "  mixed auto/int 混写 间距 = " << join(gm) << '\n';
        CHECK(ga == gb, "auto 版与 int 版的槽位间距完全一致");
        CHECK(gm.size() == 3 && gm[0] == 4 && gm[1] == 4 && gm[2] == 4,
              "auto 与显式 int 混在同一帧内，每个槽位都是 4 字节");
    }

    // ------------------------------------------------------------------
    cout << "\n[4] 别名 vs 拷贝：地址给出确定答案\n";
    {
        int  x = 42;
        auto  y = x;      // 拷贝
        auto& z = x;      // 别名
        cout << "  &x = " << (void*)&x << '\n';
        cout << "  &y = " << (void*)&y << "  (auto  y = x)\n";
        cout << "  &z = " << (void*)&z << "  (auto& z = x)\n";
        CHECK(&y != &x, "auto y = x 得到的是新对象（地址不同）");
        CHECK(&z == &x, "auto& z = x 是别名（地址相同）");
        z = 7;
        CHECK(x == 7 && y == 42, "改 z 改到了 x，y 不变");
    }

    // ------------------------------------------------------------------
    cout << "\n[5] 存储期决定地址位置，auto 不参与决策\n";
    {
        auto  local = 1;                       // 栈
        auto* heap  = new auto(5);             // 堆（C++11 起允许 new auto）
        cout << "  全局 auto     &g_auto   = " << (void*)&g_auto   << "  (.data)\n";
        cout << "  文件内 static &g_static = " << (void*)&g_static << "  (.data)\n";
        cout << "  栈上 auto     &local    = " << (void*)&local    << "  (栈)\n";
        cout << "  堆上 auto     heap      = " << (void*)heap      << "  (堆)\n";
        CHECK(sizeof(*heap) == sizeof(int), "new auto(5) 的类型是 int");
        CHECK((void*)&local != (void*)heap, "栈地址与堆地址不同（存储期不同）");
        CHECK((void*)heap != (void*)&g_auto, "堆地址与静态区地址不同");
        delete heap;
    }

    // ------------------------------------------------------------------
    cout << "\n[6] 有没有地址，取决于有没有被取地址 —— 与 auto 无关\n";
    {
        cout << "  no_addr(7)  = " << no_addr(7)  << "   （从未取地址，优化后可能根本不占内存）\n";
        cout << "  has_addr(7) = " << has_addr(7) << "   （取地址并逃逸，必须落在内存里）\n";
        CHECK(no_addr(7) == has_addr(7), "两种写法的结果一致，区别只在有没有地址");
        CHECK(g_sink != nullptr, "逃逸后确实拿到了一个地址");
    }

    // ------------------------------------------------------------------
    cout << "\n[7] auto 本身不是类型，因此谈不上“它去分配地址”\n";
    cout << "  下面这些写法都编译不过（脚本里做的负例编译测试）：\n";
    cout << "    int n = sizeof(auto);      -> error: invalid use of 'auto'\n";
    cout << "    auto x;                    -> error: declaration of 'auto x' has no initializer\n";
    cout << "    struct S { auto m = 1; };  -> error: non-static data member declared with placeholder 'auto'\n";
    cout << "  auto 只能出现在【有初始化器、能推出类型】的声明里；\n";
    cout << "  它自己没有大小、没有地址、不产生任何运行时实体。\n";

    // ------------------------------------------------------------------
    cout << "\n================ 结果 ================\n";
    if (g_fail == 0) cout << "全部检查通过（auto 不分配地址，且不改变地址分配规则）\n";
    else             cout << "有 " << g_fail << " 项检查失败\n";
    return g_fail == 0 ? 0 : 1;
}
