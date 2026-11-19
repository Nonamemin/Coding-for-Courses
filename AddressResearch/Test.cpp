// 实验：auto 与"取地址"（&）——用地址证明 auto 到底有没有拷贝、推导成什么类型
// 编译： g++ -std=c++14 auto_address.cpp -o auto_address.exe
// 仅 GCC / MinGW 可用（用到 __PRETTY_FUNCTION__ 与 __gnu_pbds）

#include <bits/stdc++.h>
#include <bits/extc++.h>
using namespace std;
using namespace __gnu_pbds;

typedef tree<int, null_type, less<int>, rb_tree_tag,
             tree_order_statistics_node_update> ordered_set;

// ---------- 工具：把 decltype 的类型名打印成人能看懂的样子 ----------
template <class T> struct TypeName
{
    static string get()
    {
        string s = __PRETTY_FUNCTION__;          // 形如: ... [with T = int*]
        size_t a = s.find("T = ");
        size_t b = s.rfind(']');                 // 必须 rfind：数组类型 int[5] 里也有 ']'
        if (a != string::npos && b != string::npos && b > a)
            s = s.substr(a + 4, b - a - 4);
        size_t semi = s.find(';');               // 去掉 GCC 附带的别名说明
        if (semi != string::npos) s = s.substr(0, semi);
        // 把啰嗦的库类型名缩短
        for (const char* from : {"std::__cxx11::basic_string<char>"})
        {
            size_t p;
            while ((p = s.find(from)) != string::npos) s.replace(p, strlen(from), "std::string");
        }
        if (s.size() > 58)
        {
            size_t lt = s.find('<');
            if (lt != string::npos) s = s.substr(0, lt) + "<...>";
        }
        return s;
    }
    static const char* c_str() { static string s = get(); return s.c_str(); }
};

template <class T> void show_type(const char* label)
{
    cout << "    " << label << "  =>  " << TypeName<T>::c_str() << '\n';
}

static const char* same_obj(const void* a, const void* b)
{
    return a == b ? "同一对象" : "不同对象";
}

int twice(int v) { return v * 2; }

int main()
{
    cout << "================== auto 与取地址 实验 ==================\n";

    // ---------------------------------------------------------------
    cout << "\n[1] auto 变量是独立对象吗？\n";
    int  x1 = 4;
    auto x2 = 4;                       // int
    cout << "    &x1 = " << (const void*)&x1 << "   &x2 = " << (const void*)&x2
         << "   -> " << same_obj(&x1, &x2) << '\n';
    show_type<decltype(x1)>("x1");
    show_type<decltype(x2)>("x2");
    show_type<decltype(&x2)>("&x2");
    x2 = 99;
    cout << "    x2 = 99 之后： x1 = " << x1 << ", x2 = " << x2 << "（两个独立对象，互不影响）\n";

    // ---------------------------------------------------------------
    cout << "\n[2] auto 会剥掉引用：拷贝还是别名？\n";
    int& r  = x1;
    auto  y = r;                       // int    —— 拷贝
    auto& z = r;                       // int&   —— 别名（int& & 折叠成 int&）
    cout << "    &x1 = " << (const void*)&x1 << '\n'
         << "    &y  = " << (const void*)&y  << "   -> " << same_obj(&y, &x1) << "   [auto  y = r]\n"
         << "    &z  = " << (const void*)&z  << "   -> " << same_obj(&z, &x1) << "   [auto& z = r]\n";
    show_type<decltype(r)>("r");
    show_type<decltype(y)>("y");
    show_type<decltype(z)>("z");
    z = 7;
    cout << "    z = 7 之后： x1 = " << x1 << "（z 就是 x1）， y 仍为 " << y << '\n';

    // ---------------------------------------------------------------
    cout << "\n[3] auto 会丢掉顶层 const\n";
    const int ci = 10;
    auto  a = ci;                      // int        —— const 被丢弃
    auto& b = ci;                      // const int& —— const 被保留
    show_type<decltype(ci)>("ci");
    show_type<decltype(a)>("a  = ci");
    show_type<decltype(b)>("b  = ci (auto&)");
    cout << "    &ci = " << (const void*)&ci << "   &a = " << (const void*)&a
         << "   -> " << same_obj(&ci, &a) << "（a 是副本）\n"
         << "    &ci = " << (const void*)&ci << "   &b = " << (const void*)&b
         << "   -> " << same_obj(&ci, &b) << "（b 是别名）\n"
         << "    a = 11 合法； b = 11 编译报错（b 是 const int&）\n";

    // ---------------------------------------------------------------
    cout << "\n[4] 指针场景：auto / auto* / auto** / auto&\n";
    int* p    = &x1;
    auto  q   = p;                     // int*
    auto* w   = p;                     // int*
    auto& e   = p;                     // int*&
    auto** pp = &p;                    // int**
    show_type<decltype(p)>("p  = &x1");
    show_type<decltype(q)>("q  = p");
    show_type<decltype(w)>("w  = p (auto*)");
    show_type<decltype(e)>("e  = p (auto&)");
    show_type<decltype(pp)>("pp = &p (auto**)");
    cout << "    &q  = " << (const void*)&q  << "   -> " << same_obj(&q, &p) << "（指针变量本身也是拷贝）\n"
         << "    &w  = " << (const void*)&w  << "   -> " << same_obj(&w, &p) << '\n'
         << "    &e  = " << (const void*)&e  << "   -> " << same_obj(&e, &p) << "（e 就是 p）\n"
         << "    q == p ? " << (q == p) << "   w == p ? " << (w == p) << '\n'
         << "    *pp 与 &x1：" << same_obj(*pp, &x1) << '\n';

    // ---------------------------------------------------------------
    cout << "\n[5] 数组：auto 让数组退化成指针，& 数组得到「指向数组的指针」\n";
    int  arr[5] = {1, 2, 3, 4, 5};
    auto ap  = arr;                    // int*        —— 退化
    auto aq  = &arr;                   // int(*)[5]   —— 整个数组的地址
    auto* az = &arr;                   // int(*)[5]   —— 同上，写法不同
    show_type<decltype(ap)>("ap  = arr");
    show_type<decltype(aq)>("aq  = &arr");
    show_type<decltype(az)>("az  = &arr (auto*)");
    show_type<decltype(&ap)>("&ap");
    cout << "    ap = " << (const void*)ap << "   &arr[0] = " << (const void*)&arr[0]
         << "   -> " << same_obj(ap, &arr[0]) << '\n'
         << "    &ap = " << (const void*)&ap << "（ap 这个指针变量自己的地址）\n"
         << "    aq = " << (const void*)aq << "（数值与 ap 相同，但类型不同）\n"
         << "    aq == ap ? " << ((const void*)aq == (const void*)ap)
         << "（数值相同；但直接写 aq == ap 会编译报错：distinct pointer types）\n"
         << "    sizeof(*aq) = " << sizeof(*aq) << " 字节  -> *aq 是整个数组\n"
         << "    (char*)(aq+1) - (char*)aq = " << (long)((const char*)(aq + 1) - (const char*)aq)
         << " 字节  -> 跨 5 个 int\n"
         << "    (char*)(ap+1) - (char*)ap = " << (long)((const char*)(ap + 1) - (const char*)ap)
         << " 字节  -> 跨 1 个 int\n";

    // ---------------------------------------------------------------
    cout << "\n[6] 字符串字面量 vs std::string：四类地址\n";
    auto   s2 = "Input";               // const char*，指向只读区的字面量
    string s1 = "Input";               // std::string，自己存一份副本
    auto   s3 = s1;                    // std::string，深拷贝
    auto   s4 = "Input";               // 同一个字面量（地址可能被合并）
    show_type<decltype(s2)>("s2 = \"Input\"");
    show_type<decltype(s1)>("s1 = string");
    show_type<decltype(s3)>("s3 = s1");
    show_type<decltype(&s2)>("&s2");
    cout << "    s2         = " << (const void*)s2          << "   <- 字面量本体（只读区）\n"
         << "    &s2        = " << (const void*)&s2         << "   <- 指针变量（栈）\n"
         << "    s1.c_str() = " << (const void*)s1.c_str()  << "   <- string 的字符缓冲区\n"
         << "    &s1        = " << (const void*)&s1         << "   <- string 对象（栈）\n"
         << "    s3.c_str() = " << (const void*)s3.c_str()  << "   <- 另一块缓冲区\n"
         << "    &s3        = " << (const void*)&s3         << '\n'
         << "    s2 == \"Input\" : " << (s2 == (const char*)"Input") << '\n'
         << "    s2 与 s4 指向同一字面量：" << same_obj(s2, s4) << "（相同字面量允许被合并）\n"
         << "    s2 与 s1.c_str() 同一块内存：" << same_obj(s2, s1.c_str()) << '\n'
         << "    s1.c_str() 与 s3.c_str() 同一块内存：" << same_obj(s1.c_str(), s3.c_str()) << '\n'
         << "    s1.c_str() - (char*)&s1 = " << (long)((const char*)s1.c_str() - (const char*)&s1)
         << " 字节（libstdc++ 短字符串优化：数据就放在对象内部）\n"
         << "    推测地址关系：&s1 < s1.c_str() < &s1 + " << sizeof(s1) << " ? "
         << (((const char*)s1.c_str() > (const char*)&s1) &&
             ((const char*)s1.c_str() < (const char*)&s1 + sizeof(s1)) ? "是（SSO 生效）" : "否")
         << '\n';

    // ---------------------------------------------------------------
    cout << "\n[7] decltype 与括号：decltype(x) 是 int，decltype((x)) 是 int&\n";
    show_type<decltype(x1)>("decltype(x1)");
    show_type<decltype((x1))>("decltype((x1))");
    show_type<decltype(+x1)>("decltype(+x1)");
    cout << "    规则：decltype(变量名) 取声明类型；decltype((表达式)) 取引用类型（左值 -> T&）\n";

    // ---------------------------------------------------------------
    cout << "\n[8] auto&&：转发引用的折叠\n";
    auto&& fr  = 4;                    // int&&  —— 绑定临时量，生命周期延长
    auto&& fr2 = x1;                   // int&   —— 引用折叠
    show_type<decltype(fr)>("auto&& fr  = 4");
    show_type<decltype(fr2)>("auto&& fr2 = x1");
    cout << "    &fr  = " << (const void*)&fr  << "（临时对象，被延长到 fr 作用域结束）\n"
         << "    &fr2 = " << (const void*)&fr2 << "   -> " << same_obj(&fr2, &x1) << '\n';

    // ---------------------------------------------------------------
    cout << "\n[9] 函数与函数指针（函数名同样退化）\n";
    auto  fp1 = twice;                 // int(*)(int)
    auto  fp2 = &twice;                // int(*)(int)
    auto& fr3 = twice;                 // int(&)(int)  —— 绑到函数本身
    show_type<decltype(fp1)>("fp1 = twice");
    show_type<decltype(fp2)>("fp2 = &twice");
    show_type<decltype(&fp1)>("&fp1");
    show_type<decltype(fr3)>("fr3 = twice (auto&)");
    cout << "    fp1 == fp2 ? " << (fp1 == fp2) << "    &fr3 == fp1 ? " << (&fr3 == fp1)
         << "（都指向同一个函数代码）\n"
         << "    &fp1 = " << (const void*)&fp1 << "（函数指针变量自己的地址，在栈上）\n";

    // ---------------------------------------------------------------
    cout << "\n[10] 容器：元素地址连续；范围 for 拷贝 vs 引用\n";
    vector<pair<int, int>> G{{1, 2}, {3, 4}, {5, 6}};
    cout << "    G.data() = " << (const void*)G.data() << "   &G[0] = " << (const void*)&G[0] << '\n';
    for (size_t i = 0; i + 1 < G.size(); ++i)
        cout << "    (char*)&G[" << i + 1 << "] - (char*)&G[" << i << "] = "
             << (long)((const char*)&G[i + 1] - (const char*)&G[i]) << " 字节（元素连续）\n";
    auto it0 = G.begin();
    cout << "    &*G.begin() 与 &G[0]：" << same_obj(&*it0, &G[0]) << '\n';
    show_type<decltype(it0)>("it0 = G.begin()");
    show_type<decltype(&it0)>("&it0");
    show_type<decltype(&*it0)>("&*it0");
    cout << "    &it0 = " << (const void*)&it0 << "（迭代器对象自己的地址，与元素无关）\n";
    cout << "    --- for (auto v : G) 按值拷贝 ---\n";
    int idx = 0;
    for (auto v : G)
    {
        cout << "        &v = " << (const void*)&v
             << "   &G[" << idx << "] = " << (const void*)&G[idx]
             << "   -> " << same_obj(&v, &G[idx]) << '\n';
        ++idx;
    }
    cout << "    --- for (auto& v : G) 引用 ---\n";
    idx = 0;
    for (auto& v : G)
    {
        cout << "        &v = " << (const void*)&v
             << "   &G[" << idx << "] = " << (const void*)&G[idx]
             << "   -> " << same_obj(&v, &G[idx]) << '\n';
        ++idx;
    }

    // ---------------------------------------------------------------
    cout << "\n[11] PBDS 平衡树：结点在堆上，地址顺序与键顺序无关\n";
    ordered_set tr;
    for (int k : {30, 10, 20, 50, 40}) tr.insert(k);
    cout << "    插入顺序 30 10 20 50 40，中序遍历（升序）的结点地址：\n";
    for (auto t = tr.begin(); t != tr.end(); ++t)
    {
        auto nxt = t;
        ++nxt;
        cout << "        键 " << setw(2) << *t << "   &*t = " << (const void*)&*t;
        if (nxt != tr.end())
            cout << "   到下一个键的地址差 " << (long)((const char*)&*nxt - (const char*)&*t) << " 字节";
        cout << '\n';
    }
    show_type<decltype(tr.begin())>("tr.begin()");
    show_type<decltype(&*tr.begin())>("&*tr.begin()");
    show_type<decltype(*tr.begin())>("*tr.begin()");
    cout << "    结论：键有序而地址无序（每插入一个新结点就 new 一块），\n"
         << "          所以树迭代器只能 ++/--，不能 it + 3。\n";

    cout << "\n================== 实验结束 ==================\n";
    return 0;
}
