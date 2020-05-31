#include <iostream>
#include <sstream>

int gcd(int x, int y) // 求两个数的最大公约数
{
    return y == 0 ? x : gcd(y, x % y);
}

std::string linear_expr(int x, int y) // 求最大公约数的线性表达式
{
    char exp_char[100];
    int a = 1, b = 0, d = gcd(x, y);
    while (((x * a - d) % y != 0) && ((x * a + d) % y != 0))
        a++;
    if ((x * a - d) % y == 0)
        b = -(x * a - d) / y;
    else
    {
        b = (x * a + d) / y;
        a = -x;
    }
    if (a > 0)
        snprintf(exp_char, 100, "%d*%d+%d*(%d)=%d", x, a, y, b, d);
    else
        snprintf(exp_char, 100, "%d*(%d)+%d*%d=%d", x, a, y, b, d);
    return std::string(exp_char);
}

int main()
{
    int x, y;
    std::cout << "请输入两个正整数: ";
    std::cin >> x >> y;
    std::cout << x << " 与 " << y << " 的最大公约数为: " << gcd(x, y) << std::endl;
    std::cout << "线性表达式为: " << linear_expr(x, y) << std::endl;
}
