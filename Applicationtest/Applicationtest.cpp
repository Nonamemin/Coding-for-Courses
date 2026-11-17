#include<bits/stdc++.h>
#include<bits/extc++.h>
using namespace std;
using namespace __gnu_pbds;
const int N=1e5+5;
typedef tree<int, null_type, less<int>, rb_tree_tag, tree_order_statistics_node_update> ordered_set;
//pbds实现平衡树
int main()
{
    freopen("out.txt","w",stdout);
    ios::sync_with_stdio(false);
    cin.tie(nullptr);cout.tie(nullptr);
    int x1=4; auto x2=4;
    cout<<"整形数据测试:"<<endl;
    cout<<x1<<" "<<x2<<endl;
    string s1="Input"; auto s2="Input";
    cout<<"字符串string类型测试:"<<endl;
    cout<<s1<<" "<<s2<<endl;
    float c1=2.98; auto c2=2.98;
    //思考，这里的c2是float类型还是double类型
    cout<<"浮点数类型测试:"<<endl;
    cout<<c1<<" "<<c2<<endl;
    vector<pair<int,int> >G;
     std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<int> dist(1, 100);
    for(int i=1;i<=5;i++)
        G.push_back(make_pair(dist(gen),dist(gen)));
   std::vector<pair<int,int> >::iterator it;
   it=G.begin();
   cout << "--- 使用 auto 遍历输出 ---" << endl;
   for(;it!=G.end();it++)
   {
       cout<<(*it).first<<" "<<(*it).second<<endl;//特别声明
       //在c++中,（.）作为成员访问的优先级高于（*）解引用
       //所以此处想要取出迭代器指针it所指向的值需要添加括号
       //或者使用->,写做*it->first
   }
   cout << "\n--- 使用显式迭代器遍历输出 ---" << endl;
   for(auto x:G)
   {
        cout<<x.first<<" "<<x.second<<endl;
        //清爽干练
   }
    vector<int> nums(100);
    iota(nums.begin(), nums.end(), 1); // 1 到 100
    mt19937 gen1(random_device{}());
    shuffle(nums.begin(), nums.end(), gen1); // 打乱
    ordered_set tr;
    for (int i = 0; i < 10; ++i)
    {
        tr.insert(nums[i]);
    }
    int rk1=1;
    cout << "\n--- 使用 auto 遍历输出 ---" << endl;
    for (auto it = tr.begin(); it != tr.end(); ++it)
        cout <<"第 "<<rk1++<<" 名: "<< *it <<endl;
    cout << "\n\n";
    cout << "--- 使用显式迭代器遍历输出 ---" << endl;
    ordered_set::iterator it1 = tr.begin();
    // 如果想带上排名输出：
    int rank = 1;
    for (; it1 != tr.end(); ++it1, ++rank)
    {
        cout<<"第 "<<rank<<" 名: "<<*it1<<endl;
    }

    return 0;
}