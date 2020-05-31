#include <iostream>
#include <cmath>
#include <sstream>

bool is_prime(int n) // 判断一个数是否为素数
{
    for (int i = 2; i <= std::sqrt(n); ++i)
        if (n % i == 0)
            return false;
    return true;
}

std::string prime_expr(int n) // 求一个的素数分解式
{
    std::stringstream buff;
    buff << n << "=";
    for (size_t p = 2; n > 1; ++p) // 如果 n > 1, 就去找质数除它
    {
        if (is_prime(p))
            while (n % p == 0)
            {
                buff << p << "*"; // n 里面可以提出一个质数 p
                n /= p;
            }
    }
    std::string expr_str = buff.str();
    return expr_str.substr(0, expr_str.size() - 1); // 去除字符串最后多余的 "*" 号
}

int main()
{
    int n;
    std::cout << "请输入一个大于2的整数: ";
    std::cin >> n;
    std::cout << "素数分解式为: " << prime_expr(n);
}
