# 代码说明：`auto` 类型推导 · 迭代器遍历 · PBDS 平衡树

> 一句话总结：这是一段 **C++（GCC）课堂演示程序**，用三组小实验对比 `auto` 与显式类型的差别，
> 用两种写法遍历 `vector<pair<int,int>>`，最后把 10 个随机数插入 GNU PBDS 的有序平衡树，
> 再按"名次"升序打印出来。所有输出被重定向到当前目录的 `out.txt`，控制台不显示任何内容。

---

## 一、程序定位

| 项目 | 内容 |
| --- | --- |
| 语言 / 标准 | C++11 及以上（`auto`、`mt19937`、`iota` 都需要 C++11） |
| 编译器要求 | **必须是 GCC / MinGW**：`<bits/stdc++.h>`、`<bits/extc++.h>`、`__gnu_pbds` 都是 GNU 扩展，MSVC 不支持 |
| 涉及知识点 | `auto` 推导规则、`vector`/`pair`、迭代器、范围 for、随机数引擎与分布、`iota`/`shuffle`、PBDS 有序集合（红黑树）、输出重定向 |
| 输入 | 无 |
| 输出 | 当前工作目录下的 `out.txt`（覆盖写） |

编译运行（在本机 MinGW 8.1 上实测通过，`-Wall -Wextra` 无警告）：

```bash
g++ -std=c++14 demo.cpp -o demo.exe
./demo.exe
```

**注意**：运行后控制台是空的，结果在 `out.txt` 里。看不到输出不是程序没跑，而是被 `freopen` 重定向了。

---

## 二、执行流程

```
① L10   freopen          → 把 stdout 换成 out.txt
② L11-12 加速 cin/cout   → 本程序没有输入，纯属模板习惯
③ L13-22 auto 三组实验   → int / string vs const char* / float vs double
④ L23-28 造 5 个随机 pair → random_device → mt19937 → uniform_int_distribution
⑤ L29-38 显式迭代器遍历
⑥ L39-44 范围 for 遍历
⑦ L45-48 1~100 填好再打乱
⑧ L49-53 取前 10 个插入 ordered_set（平衡树）
⑨ L54-57 / L58-66 两种写法遍历平衡树，按名次打印
```

---

## 三、逐段解析

### 3.1 头文件与命名空间（L1–L4）

```cpp
#include<bits/stdc++.h>     // GCC 的"全家桶"：一次引入所有标准库
#include<bits/extc++.h>    // GCC 的扩展库：__gnu_pbds（平衡树）等
using namespace std;
using namespace __gnu_pbds; // 少写前缀，下面可直接写 tree / null_type / rb_tree_tag
```

- `<bits/stdc++.h>` 不是标准头文件，只在 GCC 下存在，优点是省事，缺点是编译变慢、不可移植。
- 第 4 行的 `using namespace __gnu_pbds;` 专为第 6 行的 `tree` 服务。

### 3.2 `ordered_set` 的类型定义（L5–L6）—— 全程序的"主角"

```cpp
const int N=1e5+5;   // ① 本程序里根本没用到，属于模板残留（1e5 是 double，赋给 int 恰好无损）
typedef tree<int, null_type, less<int>, rb_tree_tag,
             tree_order_statistics_node_update> ordered_set;
```

`tree` 是 `__gnu_pbds` 提供的**平衡树容器**，5 个模板参数逐个看：

| 参数 | 本例取值 | 含义 |
| --- | --- | --- |
| ① Key | `int` | 键（元素）类型 |
| ② Mapped | `null_type` | 映射类型。写 `null_type` 就是 **set**（只有键）；写 `int` 就变成 **map**（键→值） |
| ③ Compare | `less<int>` | 升序。换成 `greater<int>` 就是降序 |
| ④ Tag | `rb_tree_tag` | 底层实现：红黑树（另一个可选 `splay_tree_tag` 伸展树） |
| ⑤ Node Update | `tree_order_statistics_node_update` | 让每个结点额外维护"子树大小"。**加上它才能用 `order_of_key` / `find_by_order` 这两个 O(log n) 的排名接口**；不加（用 `null_node_update`）则没有，且结点更小更快 |

因此这一行得到的 `ordered_set` 等价于"支持求排名的 `std::set<int>`"，插入/删除/查找都是 O(log n)。

实测它的接口（本机验证）：`insert`（返回 `pair<iterator,bool>`，重复元素被忽略）、`erase`、`find`、`lower_bound`、`upper_bound`、`size`、`empty`、`begin/end`、`order_of_key`、`find_by_order`。

> ⚠️ 本程序其实**没有用到** `order_of_key` / `find_by_order`（那才是 PBDS 存在的理由），
> 只是把平衡树当作"自动排序的容器"来中序遍历。第 6 节给出了这两个接口的用法。

### 3.3 输出重定向与 IO 加速（L10–L12）

```cpp
freopen("out.txt","w",stdout);   // 把标准输出重定向到 out.txt，"w" 表示覆盖
ios::sync_with_stdio(false);
cin.tie(nullptr); cout.tie(nullptr);
```

- `freopen` 之后，`cout` / `printf` 写的东西全部进文件；**控制台一片空白**。路径是相对路径，文件生成在"运行时的当前目录"，不是在源码所在的目录。
- **没检查返回值是隐患**：`freopen` 打不开文件时返回 `NULL`，而此时 `stdout` 已经被关闭，后续所有 `cout` **静默失效**——不报错、不提示，程序照样 `return 0`。（本次实测就复现了：在无写权限的目录下运行，`out.txt` 没生成，控制台也一个字都没有。）
- `sync_with_stdio(false)` + `tie(nullptr)` 是竞赛/大数据量常用的加速手段；本程序没有输入，纯粹是习惯。注意关掉同步后**不要混用** `printf/scanf` 与 `cout/cin`（其实混用也不会错，但输出顺序不再受保证）。

### 3.4 三组 `auto` 实验（L13–L22）—— 全程序的教学核心

```cpp
int x1=4;                    auto x2=4;        // 都是 int
string s1="Input";           auto s2="Input";  // s1 是 std::string，s2 是 const char* ！
float c1=2.98;               auto c2=2.98;     // c1 是 float，c2 是 double ！
```

| 变量 | 显式类型 | `auto` 版本的真实类型 | 结论 |
| --- | --- | --- | --- |
| `x1` / `x2` | `int` | `int`（字面量 `4` 是 `int`） | 一致 |
| `s1` / `s2` | `std::string` | **`const char*`** | **不一致**：字符串字面量是 `const char[6]`，`auto` 会把它**退化为指针** |
| `c1` / `c2` | `float` | **`double`** | **不一致**：字面量 `2.98` 默认是 `double`，`auto` 原样保留 |

**L20 那个问题的答案：`c2` 是 `double`，不是 `float`。** 三条独立证据（本机实测）：

```cpp
sizeof(c2) == 8                              // double 8 字节，float 4 字节
is_same<decltype(c2), double>::value == true // 编译期判定为真
is_same<decltype(c2), float>::value  == false
```

`c2` 是 `double` 带来的实际影响：

- 它比 `c1` 精度更高、占用更大（8 字节 vs 4 字节），打印时两者都显示 `2.98`（默认 6 位有效数字），**从输出上看不出区别**；
- 若参与大量运算，`double` 更慢但更准；
- `float c1 = 2.98;` 是"`double` 窄化到 `float`"，合法的拷贝初始化，但会丢精度。`-Wconversion` 会明确警告：
  `conversion from 'double' to 'float' changes value from '2.98e+0' to '2.98000002e+0f'`。
  想要 `float` 请写字面量后缀：`float c1 = 2.98f;` 或 `auto c1 = 2.98f;`。

`s2` 是 `const char*` 的后果同样隐蔽：

```cpp
cout << s2;          // 正常输出 Input —— 看不出任何异常
s2.size();           // ✗ 编译错误：request for member 'size' in 's2', which is of non-class type 'const char*'
s2 = "abc";          // ✓ 指针重新指向
s2.length()/substr() // ✗ 都不存在
```

**`auto` 的推导规则小结**（本例覆盖了其中三条）：

1. 字面量保持"原本的类型"：`4`→`int`、`2.98`→`double`、`2.98f`→`float`、`"Input"`→`const char[6]`；
2. 数组/函数会**退化**为指针（所以 `s2` 成了 `const char*`）；
3. `auto` 会**丢掉**顶层 `const` 和引用（要保留就写 `const auto&` / `auto&` / `decltype(auto)`）；
4. 想让 `s2` 真是 `std::string`，就显式写 `string`，或用字面量后缀：`auto s2 = "Input"s;`（需 `using namespace std::string_literals;`）。

### 3.5 造 5 个随机 `pair`（L23–L28）

```cpp
vector<pair<int,int>> G;
std::random_device rd;                        // 熵源
std::mt19937 gen(rd());                       // 梅森旋转引擎，用它产生随机数
std::uniform_int_distribution<int> dist(1,100);// 把引擎输出映射到 [1,100] 的均匀整数
for(int i=1;i<=5;i++)
    G.push_back(make_pair(dist(gen), dist(gen)));
```

- 标准的三件套：**引擎**（产生伪随机位流）+ **分布**（映射到需要的范围/分布）。不要用 `gen()%100`——取模会引入偏差。
- ⚠️ **两个 `dist(gen)` 在同一个函数调用里，实参求值顺序是"未指定"的**：不同编译器可能把先产生的数放进 `.first`，也可能放进 `.second`。结果只是两个成员对调，不影响正确性，但说明"同一行里连续取两次随机数"可读性不佳，建议先取到临时变量再构造。
- ⚠️ **在本机 MinGW 8.1 上 `std::random_device` 并不随机**：实测 `rd.entropy() == 0`，两次运行 `rd()` 输出完全相同的序列（`3499211612 581869302 ...`），所以这段"随机数据"**每次运行都长得一模一样**（连续多次运行，5 组数都是 `73 57 / 92 35 / 60 1 / 52 88 / 98 96`）。想要真正每次不同，得用时间做种子：

```cpp
mt19937 gen((unsigned)chrono::steady_clock::now().time_since_epoch().count());
```

  反过来，**做实验/对拍时"固定种子"反而是优点**（结果可复现），所以别把 `random_device` 当成"随机性"的保证。

### 3.6 `vector` 的两种遍历（L29–L44）—— 一个必须记住的优先级坑

**写法 A：显式迭代器（L29–L38）**

```cpp
std::vector<pair<int,int>>::iterator it;   // 类型名又长又臭 —— 这正是 auto 要解决的问题
it = G.begin();
for(; it != G.end(); it++)
    cout << (*it).first << " " << (*it).second << endl;
```

`(*it).first` 的括号**不能省**：成员访问 `.` 的优先级高于解引用 `*`，写成 `*it.first` 会被解析成 `*(it.first)` 而报错。

但 L37 的注释 `//或者使用->,写做*it->first` **是错的**。`->` 的优先级同样高于 `*`，所以

```cpp
*it->first   ≡   *(it->first)   // it->first 是 int，对 int 解引用 → 编译错误
```

本机实测报错原文：

```
error: invalid type argument of unary '*' (have 'int')
```

正确写法是 **`it->first`**（`it->first` ≡ `(*it).first`）。注释的本意应该是"改用 `->`，写成 `it->first`"。

**写法 B：范围 for + `auto`（L39–L44）**

```cpp
for(auto x : G)               // x 是 pair<int,int> 的【副本】
    cout << x.first << " " << x.second << endl;
```

- 确实"清爽干练"，但 `auto x` 是**按值拷贝**每个元素。5 个 `pair<int,int>` 无所谓，若元素是大对象（字符串、大结构体、整棵树）就该写 `const auto& x`（只读）或 `auto& x`（要改）。
- ⚠️ **L31 与 L39 的两句提示语写反了**：L31 打印的是"使用 auto 遍历输出"，但紧跟的是**显式迭代器**循环；L39 打印"使用显式迭代器遍历输出"，紧跟的却是**范围 for**。因为两者遍历的是同一个 `G`，输出内容完全相同，所以 `out.txt` 里看不出矛盾，只有读源码才发现。作为教学示例应当对调这两句 `cout`。
  （顺带说明：后面平衡树那一段的标签是**对**的——L55 的分支确实用了 `auto`，L59 的分支确实用了显式 `ordered_set::iterator`。）

### 3.7 1~100 洗牌取前 10（L45–L48）

```cpp
vector<int> nums(100);                        // 100 个元素，值初始化为 0
iota(nums.begin(), nums.end(), 1);            // 依次填 1,2,3,...,100（<numeric>，被全家桶带进来了）
mt19937 gen1(random_device{}());              // 又是随机引擎（同样受上面"熵为 0"的影响）
shuffle(nums.begin(), nums.end(), gen1);      // Fisher-Yates 洗牌
```

`nums` 此时是 `1..100` 的一个**随机排列**，所以 `nums[0..9]` 是 10 个**互不相同**的数——这保证了后面 10 次 `insert` 不会出现重复元素被丢弃的情况。

### 3.8 插入 PBDS 平衡树并按名次输出（L49–L66）

```cpp
ordered_set tr;
for (int i = 0; i < 10; ++i) tr.insert(nums[i]);   // 插入即排序，O(log n)
```

两段遍历只是写法不同，效果一致：

```cpp
// L54-57：相对"简洁"的 for + auto
int rk1 = 1;
for (auto it = tr.begin(); it != tr.end(); ++it)
    cout << "第 " << rk1++ << " 名: " << *it << endl;

// L60-65：显式类型 + 逗号表达式同时推进两个变量
ordered_set::iterator it1 = tr.begin();
int rank = 1;
for (; it1 != tr.end(); ++it1, ++rank)
    cout << "第 " << rank << " 名: " << *it1 << endl;
```

要点：

1. **遍历顺序 = 键的升序**，与插入顺序无关。所以"第 k 名"是"值第 k 小"，不是"第 k 个插入的"。想让"第 1 名"最大，把比较器改成 `greater<int>`。
2. `*it` 得到元素值，类型是 `const int&`（set 的迭代器不允许改键，否则会破坏树序）。
3. PBDS 的迭代器是**双向**的，`++`/`--` 都行，但**不支持随机访问**（不能写 `it + 3`）。要随机访问下的第 k 小，用 `find_by_order(k)`。
4. `int rank = 1;`（L62）与 `<type_traits>` 里的模板 `std::rank` **同名**。因为上面 `using namespace std;`，这个局部变量会遮蔽模板；本例合法，但属于不良命名（同一作用域里再写 `rank<int>` 就编不过了），建议叫 `rnk`。
5. L56 的 `auto it = tr.begin()` 遮蔽了 L29 那个 `vector` 的 `it`。因为都在 `for` 语句自己的作用域里，合法且无影响，但复用同名变量容易读混。
6. L60 的 `ordered_set::iterator it1` 正是"显式类型"的示范——`ordered_set` 是 typedef 出来的短名字，所以这里还不算难写；若是原生 `tree<...>` 全名就非常长了，这才是 `auto` 的价值所在。
7. `tr.end()` 每次循环都重新求值（本身是 O(1) 的 `end()` 调用），无性能问题，但与 `G.end()` 同理，属于习惯问题。

### 3.9 收尾（L67–L69）

`return 0;` 正常返回。`freopen` 打开的文件会在进程结束时由系统关闭，文件内容已随每次 `endl` 的 flush 落盘。

---

## 四、实测输出（`out.txt`）

以下为本机一次运行的完整内容（随机部分每次不同——在这台 MinGW 上其实相同，见 3.5）：

```
整形数据测试:
4 4
字符串string类型测试:
Input Input
浮点数类型测试:
2.98 2.98
--- 使用 auto 遍历输出 ---
73 57
92 35
60 1
52 88
98 96

--- 使用显式迭代器遍历输出 ---
73 57
92 35
60 1
52 88
98 96

--- 使用 auto 遍历输出 ---
第 1 名: 12
第 2 名: 24
第 3 名: 35
第 4 名: 38
第 5 名: 55
第 6 名: 62
第 7 名: 65
第 8 名: 76
第 9 名: 86
第 10 名: 97

--- 使用显式迭代器遍历输出 ---
第 1 名: 12
...
```

可以看出：`vector` 两段输出完全一致（同一容器、同一顺序）；平衡树段输出的是**升序**的 10 个数，且"名次"连续递增。

---

## 五、问题清单

| # | 位置 | 问题 | 建议 | 严重度 |
| --- | --- | --- | --- | --- |
| 1 | L37 注释 | 说 `->` 写作 `*it->first`，实际等价于 `*(it->first)`，**编译不过** | 改成 `it->first` | 高（错误知识） |
| 2 | L31 / L39 | 两句提示语与紧随其后的循环写法**对调**了 | 互换两条 `cout` | 中（误导） |
| 3 | L16 | `auto s2="Input"` 得到 `const char*` 而非 `string` | 显式 `string s2`，或 `"Input"s` | 中（演示点没点透） |
| 4 | L19 | `float c1=2.98` 由 `double` 窄化，丢精度 | `2.98f` | 低 |
| 5 | L10 | `freopen` 未检查返回值，失败时输出**静默消失** | `if(!freopen(...)) { perror(...); return 1; }` | 中 |
| 6 | L28 | 同一表达式里两次 `dist(gen)`，实参求值顺序未指定 | 先取到临时变量 | 低 |
| 7 | L24–25 / L47 | `random_device` 在本环境**熵为 0**，每次运行结果相同 | 需要真随机用 `chrono` 做种子；需要复现则显式固定种子 | 低（认知陷阱） |
| 8 | L40–44 | `for(auto x : G)` 按值拷贝 | `const auto& x` | 低 |
| 9 | L62 | 变量名 `rank` 与 `std::rank` 同名 | 改名 `rnk` | 低（风格） |
| 10 | L5 | `N` 定义后从未使用 | 删掉 | 低 |
| 11 | L1–2 | `<bits/*.h>` 与 `__gnu_pbds` 均为 GCC 专有 | 提交评测前确认是 GNU 环境 | 提示 |
| 12 | L29–38 | 迭代器 `it` 定义在循环外，作用域过大 | 写进 `for` 的初始化语句 | 低 |

---

## 六、三个思考题

**Q1：L19 的 `c2` 是 `float` 还是 `double`？**
`double`。字面量 `2.98` 本身就是 `double`，`auto` 不做任何"看起来更像 float"的猜测。验证：`sizeof(c2)==8`、`is_same<decltype(c2),double>::value` 为 `true`、`-Wconversion` 对 `float c1=2.98` 报窄化警告。

**Q2：为什么 `cout << s2` 看上去和 `s1` 一样？**
因为 `operator<<` 对 `const char*` 有重载，会按 C 字符串输出到 `\0` 为止，效果与 `std::string` 相同。**输出相同不等于类型相同**——一旦调用成员函数（`size()`、`substr()`）、拼接、或作为 `string&` 形参传递，`const char*` 就立刻暴露。

**Q3：为什么平衡树输出是升序，而不是插入顺序？**
`ordered_set` 是**有序关联容器**，元素按比较器 `less<int>` 组织在红黑树里，`begin()` 指向最小元素，中序遍历天然升序。它不保存插入顺序；需要插入顺序请用 `vector` 或 `list`。

---


```cpp
#include <bits/stdc++.h>
#include <bits/extc++.h>
using namespace std;
using namespace __gnu_pbds;

typedef tree<int, null_type, less<int>, rb_tree_tag,
             tree_order_statistics_node_update> ordered_set;

int main()
{
    if (!freopen("out.txt", "w", stdout)) {      // 修正：检查返回值
        perror("freopen");
        return 1;
    }
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // ---------- 1. auto 推导 ----------
    int    x1 = 4;        auto x2 = 4;         // 都是 int
    string s1 = "Input";  auto s2 = "Input";   // s2 是 const char*，不是 string
    using namespace std::string_literals;
    auto s3 = "Input"s;                        // 想要 string 就加后缀 s
    float  c1 = 2.98f;    auto c2 = 2.98;      // c2 是 double

    cout << "int    : " << x1 << " " << x2 << "  sizeof=" << sizeof(x2) << '\n';
    cout << "string : " << s1 << " | " << s2 << " | " << s3
         << "  s2_is_constchar*=" << is_same<decltype(s2), const char*>::value
         << "  s3_is_string="   << is_same<decltype(s3), string>::value << '\n';
    cout << "float  : " << c1 << " " << c2
         << "  sizeof(c2)=" << sizeof(c2) << " (float=4, double=8)\n\n";

    // ---------- 2. vector<pair> 随机数据 ----------
    mt19937 gen(20240514);                     // 固定种子：结果可复现
    uniform_int_distribution<int> dist(1, 100);
    vector<pair<int, int>> G;
    for (int i = 0; i < 5; ++i) {
        int a = dist(gen), b = dist(gen);      // 先取数再构造，规避求值顺序问题
        G.push_back({a, b});
    }

    cout << "--- 显式迭代器 ---\n";
    for (vector<pair<int, int>>::iterator it = G.begin(); it != G.end(); ++it)
        cout << it->first << ' ' << (*it).second << '\n';   // it->first 才对

    cout << "--- 范围 for（auto）---\n";
    for (const auto& p : G)                    // 引用，避免逐个拷贝
        cout << p.first << ' ' << p.second << '\n';

    // ---------- 3. PBDS 平衡树 ----------
    vector<int> nums(100);
    iota(nums.begin(), nums.end(), 1);
    shuffle(nums.begin(), nums.end(), gen);

    ordered_set tr;
    for (int i = 0; i < 10; ++i) tr.insert(nums[i]);

    cout << "\n--- 平衡树中序遍历（升序）---\n";
    int rnk = 1;                               // 不叫 rank，避免遮蔽 std::rank
    for (auto it = tr.begin(); it != tr.end(); ++it)
        cout << "第 " << rnk++ << " 名: " << *it << '\n';

    // 真正用到 PBDS 的两个 O(log n) 接口
    int v = nums[0];
    cout << "\norder_of_key(" << v << ") = " << tr.order_of_key(v)
         << "   -> 比它小的有 " << tr.order_of_key(v) << " 个，故它是第 "
         << tr.order_of_key(v) + 1 << " 小\n";
    cout << "find_by_order(0) = " << *tr.find_by_order(0) << "   -> 最小值\n";
    cout << "find_by_order(9) = " << *tr.find_by_order(9) << "   -> 最大值\n";
    return 0;
}
```

改进版实测输出（固定种子，可复现）：

```
int    : 4 4  sizeof=4
string : Input | Input | Input  s2_is_constchar*=1  s3_is_string=1
float  : 2.98 2.98  sizeof(c2)=8 (float=4, double=8)

--- 显式迭代器 ---
86 40
37 20
18 12
50 92
58 71
--- 范围 for（auto）---
86 40
37 20
18 12
50 92
58 71

--- 平衡树中序遍历（升序）---
第 1 名: 3
第 2 名: 5
第 3 名: 12
第 4 名: 39
第 5 名: 49
第 6 名: 55
第 7 名: 76
第 8 名: 84
第 9 名: 85
第 10 名: 89

order_of_key(49) = 4   -> 比它小的有 4 个，故它是第 5 小
find_by_order(0) = 3   -> 最小值
find_by_order(9) = 89   -> 最大值
```

---

##  七、速查表

**`auto` 推导**

| 写法 | 推导结果 | 提示 |
| --- | --- | --- |
| `auto a = 4;` | `int` | 字面量保持原类型 |
| `auto b = 4u;` | `unsigned` | 后缀参与推导 |
| `auto c = 2.98;` | `double` | 想要 float 写 `2.98f` |
| `auto d = "abc";` | `const char*` | 想要 string 写 `"abc"s`（C++14） |
| `auto e = v[0];` | 元素类型副本 | 要引用写 `auto&` / `const auto&` |
| `auto f = func();` | 返回值类型，引用被丢弃 | 要保留引用写 `decltype(auto)` |

**PBDS `ordered_set` 常用接口**

| 接口 | 含义 | 复杂度 |
| --- | --- | --- |
| `insert(x)` | 插入，返回 `pair<iterator,bool>`，`second==false` 表示已存在 | O(log n) |
| `erase(x)` / `erase(it)` | 按键/按迭代器删除 | O(log n) |
| `find(x)` / `lower_bound` / `upper_bound` | 查找 | O(log n) |
| `order_of_key(x)` | **严格小于 x 的元素个数**（x 若存在，就是它的 0 基下标） | O(log n) |
| `find_by_order(k)` | 第 k 小元素（**0 基**）的迭代器；k ≥ size 返回 `end()` | O(log n) |
| `size()` / `empty()` / `clear()` / `begin()` / `end()` | 常规操作 | O(1) / O(n) |

**易混点**

- `it->first` ≡ `(*it).first`，而 `*it->first` ≡ `*(it->first)` → 编译错误。
- 树遍历是**升序**（由比较器决定），不是插入顺序。
- set 的迭代器是"只读"的，`*it = 5` 不允许；要改就删了再插。
- PBDS 树每个结点比 `std::set` 更大（还要存子树大小），N 很大时注意内存。
