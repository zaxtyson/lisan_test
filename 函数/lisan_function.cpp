#include <iostream>
#include <set>
#include <regex>

template <typename K, typename V>
class Function
{
public:
    typedef std::pair<K, V> ordered_pair;                                                   // 序偶类型
    void set_doom(const std::set<K> &d) { doom = std::move(d); }                            // 设置定义域
    void set_rank(const std::set<V> &r) { rank = std::move(r); }                            // 设置值域的父集
    void set_relationship(const std::set<ordered_pair> &r) { relationship = std::move(r); } // 设置函数关系
    bool is_function();                                                                     // 是否函数
    bool is_injective();                                                                    // 是否单射
    bool is_surjective();                                                                   // 是否满射
    bool is_bijective();                                                                    // 是否双射
private:
    std::set<K> doom;
    std::set<V> rank;
    std::set<ordered_pair> relationship;
    int is_func_flag{-1};
};

template <typename K, typename V>
bool Function<K, V>::is_function()
{
    if (is_func_flag == 1) // 减少重复计算的开销
        return true;
    else if (is_func_flag == 0)
        return false;

    for (auto &d : doom)
    {
        size_t count = 0; // 统计元素的像的个数
        for (auto &op : relationship)
            if (op.first == d)
                count++;
        if (count != 1) // 没有像或者一对多，不是函数
        {
            is_func_flag = 0;
            return false;
        }
    }
    is_func_flag = 1;
    return true;
}
template <typename K, typename V>
bool Function<K, V>::is_injective()
{
    if (!is_function())
        return false; // 不是函数，不算了
    for (auto &r : rank)
    {
        size_t count = 0; // 原像的个数
        for (auto &op : relationship)
            if (op.second == r)
                count++;
        if (count > 1) // 存在一个像，原象不唯一，非单射
            return false;
    }
    return true;
}

template <typename K, typename V>
bool Function<K, V>::is_surjective()
{
    if (!is_function())
        return false;
    for (auto &r : rank)
    {
        size_t count = 0; // 原象个数
        for (auto &op : relationship)
        {
            if (op.second == r)
                count++;
        }
        if (count == 0) // 没有原象，非满射
            return false;
    }
    return true;
}

template <typename K, typename V>
bool Function<K, V>::is_bijective()
{
    if (!is_function())
        return false;
    return is_injective() && is_surjective();
}

template <typename K, typename V>
void show_function_info(Function<K, V> &f)
{
    std::cout << "这坨玩意: ";
    if (!f.is_function())
    {
        std::cout << "不是函数" << std::endl;
        return;
    }
    if (f.is_bijective())
        std::cout << "是双射";
    else if (f.is_injective())
        std::cout << "是单射 ";
    else if (f.is_surjective())
        std::cout << "是满射 ";
    else
        std::cout << "只是一个普通函数";
}

int main()
{
    // 功能测试
    // Function<double, std::string> func;
    // func.set_doom({1, 2, 3.14});
    // func.set_rank({"Bird","cat","fish","dog"});
    // func.set_relationship({{1, "Bird"}, {2, "fish"}, {3.14, "fish"}});
    // show_function_info(func);

    std::string doom_str, rank_str, rela_str;
    std::set<double> doom;                                              // 定义域是实数
    std::set<std::string> rank;                                         // 值域是字符(串)集
    std::set<Function<double, std::string>::ordered_pair> relationship; // 函数关系集合

    std::cout << "定义域支持任意实数，值域支持任意英文字符串" << std::endl;
    std::cout << "输入示例: 定义域 {1,2,3.14}  值域父集 {bird,cat,dog} 函数关系 {<1,bird>,<2,cat>,<3.14,dog>}" << std::endl;
    std::cout << "请输入定义域: ";
    std::cin >> doom_str;
    std::cout << "请输入值域的父集: ";
    std::cin >> rank_str;
    std::cout << "请输入关系: ";
    std::cin >> rela_str;

    std::smatch match_str;
    while (std::regex_search(doom_str, match_str, std::regex("\\d+(\\.\\d+)?"))) // 解析输入的定义域字符串
    {
        doom.insert(std::stod(match_str.str()));
        doom_str = match_str.suffix().str();
    }

    while (std::regex_search(rank_str, match_str, std::regex("[a-zA-Z]+"))) // 解析输入的值域字符串
    {
        rank.insert(match_str.str());
        rank_str = match_str.suffix().str();
    }

    while (std::regex_search(rela_str, match_str, std::regex("<(\\d+(?:\\.\\d+)?),([a-zA-Z]+)>"))) // 解析输入的关系字符串
    {
        double first = std::stod(match_str[1].str());
        std::string second = match_str[2].str();
        relationship.insert({first, second});
        rela_str = match_str.suffix().str();
    }

    Function<double, std::string> func;
    func.set_doom(doom);
    func.set_rank(rank);
    func.set_relationship(relationship);
    show_function_info(func);
}
