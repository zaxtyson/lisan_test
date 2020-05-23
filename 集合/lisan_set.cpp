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
    Set() : data(vector<T>()) {}                                     // Ĭ�Ϲ��캯��, ����ռ�
    Set(std::initializer_list<T> &&set);                             // ͨ����Ա��ʼ���б��켯��
    int card() const { return data.size(); }                         // ��ȡ����Ԫ�ظ���
    void append(T i);                                                // ����Ԫ�� i
    bool is_empty() const { return data.size() == 0; }               // �Ƿ�ռ�
    bool contains(T i) const;                                        // �����Ƿ����Ԫ�� i
    bool operator==(const Set<T> &) const;                           // �жϼ����Ƿ����
    Set<T> operator-(const Set<T> &) const;                          // ��
    Set<T> operator&(const Set<T> &)const;                           // �󽻼�
    Set<Set<T>> pow_set() const;                                     // ���ݼ�
    Set<Set<T>> sub_set(int) const;                                  // ��nԪ�Ӽ�
    vector<vector<T>> permute() const;                               // �󼯺ϵ�ȫ����
    friend Set operator+<T>(const Set &, const Set &);               // �󲢼�
    friend std::ostream &operator<<<T>(std::ostream &, const Set &); // ���� << , ֧��ֱ�� cout << Set
};

template <typename T>
Set<T>::Set(std::initializer_list<T> &&set) // ʹ�ó�Ա��ʼ���б�������
{
    data = vector<T>();
    for (const auto &i : set) // ��ֹ�����ظ���Ԫ��
        if (!this->contains(i))
            data.push_back(i);
}

template <typename T>
bool Set<T>::contains(T e) const // �жϼ����Ƿ����Ԫ�� e
{
    for (const auto &d : data)
        if (e == d)
            return true;
    return false;
}

template <typename T>
void Set<T>::append(T e) // �򼯺����Ԫ��
{
    if (!this->contains(e))
        data.push_back(e);
}

template <typename T>
bool Set<T>::operator==(const Set<T> &other) const // �����жϼ������
{
    if (this->card() != other.card())
        return false;

    for (const auto &e : data)
        if (!other.contains(e))
            return false;
    return true;
}

template <typename T>
Set<T> Set<T>::operator-(const Set<T> &other) const // ��
{
    Set<T> subset;
    for (const auto e : data)
        if (!other.contains(e))
            subset.append(e);
    return subset;
}

template <typename T>
Set<T> operator+(const Set<T> &me, const Set<T> &other) // �󲢼�
{
    Set<T> add_set;
    for (const auto e : me.data)
        add_set.append(e);
    for (const auto i : other.data)
        add_set.append(i);
    return add_set;
}

template <typename T>
Set<T> Set<T>::operator&(const Set<T> &other) const // �󽻼�
{
    Set<T> union_set;
    for (const auto e : data)
        if (other.contains(e))
            union_set.append(e);
    return union_set;
}

//  ����һ�� 2^n �� x n �е� bool ���, ����ɸѡ���Ӽ�
//    make_table(3)               {e1,e2,e3}
//     [                              ��
//         [0, 0, 0],           [false,true,true]
//         [0, 0, 1],                 ��
//         [0, 1, 0],              {e2,e3}
//         ...
//     ]

vector<bool *> make_table(int n)
{
    vector<bool *> table;
    for (int i = 0; i < pow(2, n); ++i) // ��ʮ���� i תΪ��������ʽ, �ŵ�������
    {
        int num = i;
        bool *row = new bool[n]{false}; // ����һ������Ϊ len, Ĭ��ֵΪ 0 ������
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
Set<Set<T>> Set<T>::sub_set(int n) const // �� n Ԫ�Ӽ�
{
    Set<Set<T>> subset;
    int card = this->card();
    for (const auto &row : make_table(card)) // ���������Ʊ���е�ÿһ��, �� [0,0,1,1,..]
    {
        int num_of_1 = 0; // ͳ�� 1 ������, ÿ�� 1 ��Ӧ��һ��Ԫ��
        for (int i = 0; i < card; ++i)
            num_of_1 += row[i];
        if (num_of_1 == n) // ��� 1 ��������Ҫ���һ��, �Ѹ��������� 1 ��Ӧ��Ԫ�����һ������
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
Set<Set<T>> Set<T>::pow_set() const // ���ݼ�
{
    Set<Set<T>> pow_set;
    int card = this->card(); // ��ǰ���ϵ�Ԫ�ظ���
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
    if (path.size() == elems.size()) // ·�����������鳤����ͬ, �ݹ����
    {
        result->push_back(path);
        return;
    }
    for (T i = 0; i < elems.size(); ++i) // ����ÿһ��Ҷ�ӽڵ�
    {
        // û�г����� path �е�Ԫ����Ϊ��һ����ѡ���·��
        if (find(path.begin(), path.end(), elems[i]) == path.end())
        {
            path.push_back(elems[i]);       // �ѵ�ǰ�ڵ�ŵ�·����
            backtrack(elems, path, result); // ������ȱ���
            path.pop_back();                // �������յ�, ���ݵ���һ���ڵ�
        }
    }
}

template <typename T>
vector<vector<T>> Set<T>::permute() const // ��ȫ����
{
    vector<vector<T>> result;
    vector<T> path;
    backtrack(data, path, &result);
    return result;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const Set<T> &set) // ֧�ּ���ֱ�����
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

    Set<int> a;                                          // �����ռ�
    Set<int> b = {10, 111, 55, 4, 6, 6, 6, 12, 88, 999}; // ���������� b (�Զ�ȥ��)
    Set<int> c = {55, 10, 88, 77, 44};                   // �������� c
    Set<Set<int>> x = {a, c, b};                         // ����Ԫ��Ϊ�������ļ���
    Set<string> s = {"cat", "dog", "bird"};              // �����ַ�������
    Set<float> f = {3.14, 2.33, 12.6};                   // ��������������

    cout << "���� a: " << a << endl
         << "���� b: " << b << endl
         << "���� x: " << x << endl
         << "���� s: " << s << endl
         << "���� f: " << f << endl
         << endl;

    cout << "b �� c �Ĳ: " << (b - c) << endl
         << "b �� c �Ĳ���: " << (b + c) << endl
         << "b �� c �Ľ���: " << (b & c) << endl
         << endl;

    cout << "c �� 2 Ԫ�Ӽ�: " << c.sub_set(2) << endl
         << "c ���ݼ�: " << c.pow_set() << endl
         << "s ���ݼ�: " << s.pow_set() << endl;

    vector<vector<int>> per_of_c = c.permute(); // c ��ȫ����
    cout << "c ��ȫ����:" << endl;
    for (auto &row : per_of_c)  // ���ȫ����
    {
        for (auto &i : row)
            cout << i << ',';
        cout << endl;
    }
}