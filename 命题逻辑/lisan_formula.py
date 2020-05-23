import string


class Stack(object):
    """栈"""

    def __init__(self):
        self._items = []

    def is_empty(self):
        return self._items == []

    def push(self, item):
        self._items.append(item)

    def pop(self):
        return self._items.pop()

    def top(self):
        return self._items[-1]


class Formula(object):
    """命题公式类"""

    def __init__(self, expr_str: str):
        self._expr = self.beautify_expr(expr_str)  # 命题公式字符串
        self._var_list = self.extract_var(self._expr)  # 变元列表
        self._var_num = len(self._var_list)  # 变元数量
        self._result_list = None  # 结果列表, 减少重复计算

    def get_var_list(self) -> list:
        """获取变元列表"""
        return self._var_list

    def get_var_num(self) -> int:
        """获取变元数量"""
        return self._var_num

    def get_expr(self) -> str:
        """获取公式字符串"""
        return self._expr

    def __eq__(self, other):
        """用于两个公式比较是否等值"""
        if self._var_num != other.get_var_num():
            return False  # 变元数量都不一样, 不用算了
        return self.calculate_all() == other.calculate_all()

    @staticmethod
    def beautify_expr(expr: str) -> str:
        """美化公式, 去除空格"""
        table = str.maketrans(string.ascii_lowercase + '!^v', string.ascii_uppercase + '¬∧∨')  # 美化输入符号, 去除空格
        expr = expr.translate(table)
        return expr.replace(' ', '').replace('<->', '↔').replace('->', '→').replace('!', '¬')

    @staticmethod
    def extract_var(expr: str) -> list:
        """提取变元列表"""
        variables = set(i for i in expr if i in string.ascii_uppercase)  # 表达式中所有变元的集合 {'B', 'C', 'A', ...}, 利用集合去重
        return sorted(list(variables))  # 变元列表(安字母排序) ['A', 'B', 'C']

    def get_assign_list(self, value: int) -> list:
        """将十进制值转化为用于给公式赋值的二进制列表 (列表长度与变元数量一致)
        get_assign_list(6) -> [0, 0, 1, 1, 0]
        """
        bin_str = '{:0{width}b}'.format(value, width=self._var_num)
        return [int(i) for i in bin_str]

    @staticmethod
    def to_suffix_expr(expr: str) -> str:
        """将表达式转化为后缀表达式, 方便计算"""
        op_stack = Stack()
        out_expr = ""

        op_priority = {'(': 100, '¬': 5, '∧': 4, '∨': 3, '→': 2, '↔': 1, ')': -100}  # 算子优先级
        for char in expr:
            if char.isnumeric():  # 字符是数字(已经赋值的变元)
                out_expr += char
            elif op_stack.is_empty() or op_priority[char] > op_priority[op_stack.top()]:  # 栈为空或者当前操作符优先级比栈顶操作符高
                op_stack.push(char)
            elif char == ')':  # 遇到 ) 时弹出栈中操作符，直到碰到 (
                while op_stack.top() != '(':
                    out_expr += op_stack.pop()
                op_stack.pop()  # 丢弃栈中配对的 (
            else:  # 当前操作符优先级比栈顶操作符低
                while not op_stack.is_empty() and op_stack.top() != '(':  # 遇到 ) 才弹出 (
                    out_expr += op_stack.pop()
                op_stack.push(char)
        while not op_stack.is_empty():  # 处理完成，将栈中剩余符号弹出
            out_expr += op_stack.pop()
        return out_expr

    @staticmethod
    def calculate(suffix_expr: str) -> int:
        """计算后缀表达式的值"""
        data_stack = Stack()
        for char in suffix_expr:
            if char.isnumeric():
                data_stack.push(int(char))
            elif char == '¬':  # 取反只要一个操作数
                data = data_stack.pop()
                data_stack.push(int(not data))
            else:
                right = data_stack.pop()  # 操作符右边的数
                left = data_stack.pop()  # 操作符左边的数
                if char == '∧':
                    data_stack.push(left & right)
                elif char == '∨':
                    data_stack.push(left | right)
                elif char == '→':
                    value = int(not left) | right
                    data_stack.push(value)
                elif char == '↔':
                    value = (int(not left) | right) & (int(not right) | left)
                    data_stack.push(value)
        return data_stack.pop()

    def calculate_all(self):
        """计算所有赋值下公式的值"""
        if self._result_list is not None:
            return self._result_list

        result_list = []
        table_line = pow(2, self._var_num)  # 真值表行数, 2^n
        for i in range(table_line):
            assign_list = self.get_assign_list(i)  # 赋值列表
            assign_list_str = ''.join([str(i) for i in assign_list])  # 字符形式, 用于拼接替换
            tb = str.maketrans(''.join(self._var_list), assign_list_str)  # 替换变元为对应的值
            assigned_expr = self._expr.translate(tb)  # 经过赋值的公式
            suffix_expr = self.to_suffix_expr(assigned_expr)  # 转化为后缀表达式
            result = self.calculate(suffix_expr)  # 计算公式的值
            result_list.append(assign_list + [result])  # 结果(二维)列表为: 赋值列表 + 结果
        self._result_list = result_list  # 保存结果, 减少重复计算
        return result_list

    def get_disjunctive(self) -> str:
        """获取主析取范式"""
        min_expr_str = ''
        for line in self.calculate_all():
            if line[-1] == 0:
                continue  # 成假赋值不管
            min_expr_str += '('
            for i, var in enumerate(self._var_list):
                min_expr_str += f'{var}∧' if line[i] == 1 else f'¬{var}∧'
            min_expr_str = min_expr_str[:-1]  # 去除多余的 ^ 号
            min_expr_str += ')∨'
        min_expr_str = min_expr_str[:-1]  # 去除多余的 V 号
        return min_expr_str

    def get_conjunctive(self) -> str:
        """获取主合取范式"""
        max_expr_str = ''
        for line in self.calculate_all():
            if line[-1] == 1:
                continue  # 成真赋值不管
            max_expr_str += '('
            for i, var in enumerate(self._var_list):
                max_expr_str += f'{var}∨' if line[i] == 0 else f'¬{var}∨'
            max_expr_str = max_expr_str[:-1]  # 去除多余的 ∨ 号
            max_expr_str += ')∧'
        max_expr_str = max_expr_str[:-1]  # 去除多余的 ∧ 号
        return max_expr_str

    def print_table(self):
        """打印真值表"""
        print(' | '.join([*self.get_var_list(), self.get_expr()]))  # 输出真值表标题
        for line in self.calculate_all():
            print(' | '.join([str(i) for i in line]))


if __name__ == '__main__':
    print("\n支持符号: () ! v ^ ->  <-> (v是小写英文V, ^键盘按Shift+6)")
    print("符号为半角英文符号, 公式中的空格会被忽视, 字母不区分大小写\n")
    # raw_expr = input("输入逻辑表达式:")
    # f1 = Formula(raw_expr)

    f1 = Formula('(a^b->c)vd')
    f2 = Formula('a->b^c^(cvd)')
    input(f1.to_suffix_expr("(0∧0→1)∨1"))

    print("原命题公式: ", f1.get_expr())
    print("主析取范式: ", f1.get_disjunctive())
    print("主合取范式: ", f1.get_conjunctive())
    print("真值表如下: ")
    f1.print_table()

    if f1 == f2:
        print(f"{f1.get_expr()} 与 {f2.get_expr()} 等值")
    else:
        print(f"{f1.get_expr()} 与 {f2.get_expr()} 不等值")

    input('\n\n按回车退出')
