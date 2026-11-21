#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
int main() {
	int a = 10;
	auto op_1 = [a](const int& x, const int& y) {assert(x < y);return a >= x && a <= y;}; //lambda表达式,[]内表示捕获的外部变量,能为内部函数所用;()传入两个常量引用形参x,y,假定x必然小于y。
	// lambda函数在捕获非引用的变量的条件下，是无法对原变量造成修改的。
	std::cout << "op_1(3,5) = " << op_1(3, 5) << std::endl; // 因为 10>=3&&10<=5为假 , 故返回0(false);

	int b = 10;
	auto op_2 = [&b](const int& x) {b = b * x;return b;}; //按引用捕获b。此处的b被写入函数内部，是可以修改的
	std::cout << "op_2(3) = " << op_2(3) << std::endl;  //输出 10*3=30;
	std::cout << "b = " << b << std::endl; // b被修改为30;

	int c = 10;
	auto op_3 = [c](const int& x)mutable {c = c * x;return c;}; //mutable 表示可以在函数内部对捕获的值的副本来进行修改，但在外部的原始量不发生改变;
	std::cout << "op_3(3) = " << op_3(3) << std::endl; // 输出 内部副本c值=30;
	std::cout << "c = " << c << std::endl; // 外部的c并没有被修改，仍然输出 10。与普通函数不同，若再次执行op_3(3),此时内部c的副本值被修改为30*3=90，就会输出90;


	auto op_4 = [](const int& trial_1, const int& trial_2) {return trial_1 > trial_2;};//与std::sort,std::transform等也有小配合。
	std::vector<int> trial = { 5,1,3,2,4,8,9,7,6 };
	std::sort(trial.begin(), trial.end(), op_4);//因std::sort的第三个参数用来传入返回值为bool型的比较器函数，故传入op_4。此处执行了非默认的从大到小的排序(std::sort不传入比较器的情况下默认从小到大排序)
	for (auto& t : trial) { std::cout << t << " "; }
}