#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using std::vector;

template <typename T>
class Set;
template <typename T>
std::ostream &operator<<(std::ostream &, const Set<T> &);
template <typename T>
Set<T> operator+(const Set<T> &, const Set<T> &);

template <typename T>
class Set
{
private:
    vector<T> data;

public:
    Set() : data(vector<T>()) {}                                     // 默认构造函数, 构造空集
    Set(std::initializer_list<T> &&set);                             // 通过成员初始化列表构造集合
    int card() const { return data.size(); }                         // 获取集合元素个数
    void append(T i);                                                // 插入元素 i
    bool is_empty() const { return data.size() == 0; }               // 是否空集
    bool contains(T i) const;                                        // 集合是否包含元素 i
    bool operator==(const Set<T> &) const;                           // 判断集合是否相等
    Set<T> operator-(const Set<T> &) const;                          // 求差集
    Set<T> operator&(const Set<T> &)const;                           // 求交集
    Set<Set<T>> pow_set() const;                                     // 求幂集
    Set<Set<T>> sub_set(int) const;                                  // 求n元子集
    vector<vector<T>> permute() const;                               // 求集合的全排列
    friend Set operator+<T>(const Set &, const Set &);               // 求并集
    friend std::ostream &operator<<<T>(std::ostream &, const Set &); // 重载 << , 支持直接 cout << Set
};

template <typename T>
Set<T>::Set(std::initializer_list<T> &&set) // 使用成员初始化列表创建集合
{
    data = vector<T>();
    for (const auto &i : set) // 防止插入重复的元素
        if (!this->contains(i))
            data.push_back(i);
}

template <typename T>
bool Set<T>::contains(T e) const // 判断集合是否包含元素 e
{
    for (const auto &d : data)
        if (e == d)
            return true;
    return false;
}

template <typename T>
void Set<T>::append(T e) // 向集合添加元素
{
    if (!this->contains(e))
        data.push_back(e);
}

template <typename T>
bool Set<T>::operator==(const Set<T> &other) const // 用于判断集合相等
{
    if (this->card() != other.card())
        return false;

    for (const auto &e : data)
        if (!other.contains(e))
            return false;
    return true;
}

template <typename T>
Set<T> Set<T>::operator-(const Set<T> &other) const // 求差集
{
    Set<T> subset;
    for (const auto e : data)
        if (!other.contains(e))
            subset.append(e);
    return subset;
}

template <typename T>
Set<T> operator+(const Set<T> &me, const Set<T> &other) // 求并集
{
    Set<T> add_set;
    for (const auto e : me.data)
        add_set.append(e);
    for (const auto i : other.data)
        add_set.append(i);
    return add_set;
}

template <typename T>
Set<T> Set<T>::operator&(const Set<T> &other) const // 求交集
{
    Set<T> union_set;
    for (const auto e : data)
        if (other.contains(e))
            union_set.append(e);
    return union_set;
}

//  生成一个 2^n 行 x n 列的 bool 表格, 用于筛选出子集
//    make_table(3)               {e1,e2,e3}
//     [                              ↓
//         [0, 0, 0],           [false,true,true]
//         [0, 0, 1],                 ↓
//         [0, 1, 0],              {e2,e3}
//         ...
//     ]

vector<bool *> make_table(int n)
{
    vector<bool *> table;
    for (int i = 0; i < pow(2, n); ++i) // 将十进制 i 转为二进制形式, 放到数组中
    {
        int num = i;
        bool *row = new bool[n]{false}; // 创建一个长度为 len, 默认值为 0 的数组
        for (int j = n - 1; num != 0 && j >= 0; --j)
        {
            row[j] = bool(num % 2);
            num /= 2;
        }
        table.push_back(row);
    }
    return table;
}

template <typename T>
Set<Set<T>> Set<T>::sub_set(int n) const // 求 n 元子集
{
    Set<Set<T>> subset;
    int card = this->card();
    for (const auto &row : make_table(card)) // 遍历二进制表格中的每一行, 如 [0,0,1,1,..]
    {
        int num_of_1 = 0; // 统计 1 的数量, 每个 1 对应了一个元素
        for (int i = 0; i < card; ++i)
            num_of_1 += row[i];
        if (num_of_1 == n) // 如果 1 的数量与要求的一致, 把该行中所有 1 对应的元素组成一个集合
        {
            Set<T> set;
            for (int i = 0; i < card; ++i)
                if (row[i])
                    set.append(data[i]);
            subset.append(set);
        }
    }
    return subset;
}

template <typename T>
Set<Set<T>> Set<T>::pow_set() const // 求幂集
{
    Set<Set<T>> pow_set;
    int card = this->card(); // 当前集合的元素个数
    for (const auto &row : make_table(card))
    {
        Set<T> set;
        for (int i = 0; i < card; ++i)
            if (row[i])
                set.append(data[i]);
        pow_set.append(set);
    }
    return pow_set;
}

template <typename T>
void backtrack(vector<T> elems, vector<T> path, vector<vector<T>> *result)
{
    if (path.size() == elems.size()) // 路径长度与数组长度相同, 递归结束
    {
        result->push_back(path);
        return;
    }
    for (T i = 0; i < elems.size(); ++i) // 遍历每一个叶子节点
    {
        // 没有出现在 path 中的元素作为下一步可选择的路径
        if (find(path.begin(), path.end(), elems[i]) == path.end())
        {
            path.push_back(elems[i]);       // 把当前节点放到路径中
            backtrack(elems, path, result); // 深度优先遍历
            path.pop_back();                // 遍历到终点, 回溯到上一个节点
        }
    }
}

template <typename T>
vector<vector<T>> Set<T>::permute() const // 求全排列
{
    vector<vector<T>> result;
    vector<T> path;
    backtrack(data, path, &result);
    return result;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const Set<T> &set) // 支持集合直接输出
{
    if (set.is_empty())
        return os << "{}";

    os << "{";
    for (int i = 0; i < set.card() - 1; ++i)
        os << set.data.at(i) << ',';
    os << set.data.at(set.card() - 1) << "}";
    return os;
}

int main()
{
    using namespace std;

    Set<int> a;                                          // 创建空集
    Set<int> b = {10, 111, 55, 4, 6, 6, 6, 12, 88, 999}; // 创建整数集 b (自动去重)
    Set<int> c = {55, 10, 88, 77, 44};                   // 整数集合 c
    Set<Set<int>> x = {a, c, b};                         // 创建元素为整数集的集合
    Set<string> s = {"cat", "dog", "bird"};              // 创建字符串集合
    Set<float> f = {3.14, 2.33, 12.6};                   // 创建浮点数集合

    cout << "集合 a: " << a << endl
         << "集合 b: " << b << endl
         << "集合 x: " << x << endl
         << "集合 s: " << s << endl
         << "集合 f: " << f << endl
         << endl;

    cout << "b 与 c 的差集: " << (b - c) << endl
         << "b 与 c 的并集: " << (b + c) << endl
         << "b 与 c 的交集: " << (b & c) << endl
         << endl;

    cout << "c 的 2 元子集: " << c.sub_set(2) << endl
         << "c 的幂集: " << c.pow_set() << endl
         << "s 的幂集: " << s.pow_set() << endl;

    vector<vector<int>> per_of_c = c.permute(); // c 的全排列
    cout << "c 的全排列:" << endl;
    for (auto &row : per_of_c)  // 输出全排列
    {
        for (auto &i : row)
            cout << i << ',';
        cout << endl;
    }
}