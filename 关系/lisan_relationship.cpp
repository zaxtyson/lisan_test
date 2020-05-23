#include <iostream>
#include <sstream>
#include <set>
#include <regex>
#include <initializer_list>

template <typename T>
class Relationship
{
public:
    typedef std::pair<T, T> ordered_pair; // 序偶类型
    Relationship() = default;
    Relationship(std::initializer_list<ordered_pair> &&list) : op_set(list){};
    void based_on(std::set<T> &s) { base_set = s; }       // 设置关系为集合 s 上的关系
    void insert(ordered_pair &&op) { op_set.insert(op); } // 插入序偶
    bool contains(const ordered_pair &) const;            // 序偶是否存在关系中
    bool is_reflexive() const;                            // 是否满足自反性
    bool is_symmetric() const;                            // 是否满足对称性
    bool is_transitive() const;                           // 是否满足传递性
    bool is_equivalent() const;                           // 是否为等价关系
    std::set<std::set<T>> get_eq_class() const;           // 获取等价类集合(商集)

private:
    std::set<T> base_set;
    std::set<ordered_pair> op_set;
};

template <typename T>
bool Relationship<T>::contains(const ordered_pair &op) const
{
    return op_set.find(op) != op_set.end();
}

template <typename T>
bool Relationship<T>::is_reflexive() const
{
    for (auto &e : base_set) // 存在一个元素e, <e,e> 不在关系中, 则非自反
        if (!contains(ordered_pair(e, e)))
            return false;
    return true;
}

template <typename T>
bool Relationship<T>::is_symmetric() const
{
    for (auto &op : op_set) // 存在一个序偶 <x,y>, 关系中不存在<y,x>, 则非对称
        if (!contains(ordered_pair(op.second, op.first)))
            return false;
    return true;
}

template <typename T>
bool Relationship<T>::is_transitive() const
{
    if (base_set.empty()) // 空集的空关系是传递的
        return true;
    for (auto iter_i = op_set.cbegin(); iter_i != --op_set.cend(); ++iter_i)
    {
        auto iter_j = iter_i;
        for (++iter_j; iter_j != op_set.cend(); ++iter_j)
            if (iter_i->second == iter_j->first)
                if (!contains(ordered_pair(iter_i->first, iter_j->second)))
                    return false;
    }
    return true;
}

template <typename T>
bool Relationship<T>::is_equivalent() const
{
    return (is_symmetric() && is_reflexive() && is_transitive());
}

template <typename T>
std::set<std::set<T>> Relationship<T>::get_eq_class() const
{
    if (!is_equivalent())
        return {};
    std::set<std::set<T>> all_eq_class;
    for (auto &e : base_set)
    {
        std::set<T> eq_class; // 等价类
        for (auto &op : op_set)
            if (op.first == e)
                eq_class.insert(op.second);
        all_eq_class.insert(eq_class);
    }
    return all_eq_class;
}

template <typename T>
void show_relationship(Relationship<T> &r)
{
    std::cout << "集合满足:";
    std::cout << (r.is_reflexive() ? "自反性 " : "")
              << (r.is_symmetric() ? "对称性 " : "")
              << (r.is_transitive() ? "传递性" : "")
              << (r.is_equivalent() ? "\n等价类: " : "");

    for (auto &eq_class : r.get_eq_class())
    {
        std::stringstream set_str;
        set_str << "{";
        for (auto iter = eq_class.begin(); iter != eq_class.end(); ++iter)
            set_str << *iter << (iter == --eq_class.end() ? "" : ",");
        set_str << "}";
        std::cout << set_str.str() << " ";
    }
    std::cout << std::endl;
}

int main()
{
    // std::set<int> A = {1, 2, 3, 4, 5};
    // Relationship<int> R1 = {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}};
    // Relationship<int> R2 = {{1, 1}, {1, 3}, {1, 5}, {2, 2}, {2, 4}, {3, 1}, {3, 3}, {3, 5}, {4, 2}, {4, 4}, {5, 1}, {5, 3}, {5, 5}};
    // R1.based_on(A);
    // show_relationship(R1);
    // R2.based_on(A);
    // show_relationship(R2);

    std::string set_str, relation_str;
    std::set<double> A;
    Relationship<double> R;

    std::cout << "输入示例: 集合 {1,2,3}   关系 {<1,1>,<2,2>,<3,3>}   # 支持任意实数" << std::endl;
    std::cout << "请输入集合: ";
    std::cin >> set_str;
    std::cout << "请输入关系: ";
    std::cin >> relation_str;

    std::smatch match_str;
    while (std::regex_search(set_str, match_str, std::regex("\\d+(\\.\\d+)?")))
    {
        A.insert(std::stod(match_str.str()));
        set_str = match_str.suffix().str();
    }

    while (std::regex_search(relation_str, match_str, std::regex("<(\\d+(?:\\.\\d+)?),(\\d+(\\.\\d+)?)>")))
    {
        double first = std::stod(match_str[1].str());
        double second = std::stod(match_str[2].str());
        R.insert({first, second});
        relation_str = match_str.suffix().str();
    }

    R.based_on(A);
    show_relationship(R);
}