#include"C:\Users\何雅琴\DS2025\Stack.h"
#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>  // 补充异常处理头文件

using namespace std;

// 运算符优先级判断
int precedence(char op) {
    switch(op) {
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        default: return 0; // 非运算符（如'('）返回0
    }
}

// 执行运算
double operate(double a, double b, char op) {
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) throw runtime_error("Division by zero");
            return a / b;
        default: throw runtime_error("Invalid operator");
    }
}

// 字符串计算器主函数
double calculate(const string& expr) {
    Stack<double> numStack;  // 操作数栈（使用模板栈）
    Stack<char> opStack;     // 运算符栈（使用模板栈）

    int i = 0;
    int n = expr.size();
    
    while (i < n) {
        if (isspace(expr[i])) {  // 跳过空格
            i++;
            continue;
        }
        
        // 处理数字（整数和小数）
        if (isdigit(expr[i]) || expr[i] == '.') {
            double num = 0;
            // 整数部分
            while (i < n && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            // 小数部分
            if (i < n && expr[i] == '.') {
                i++;
                double fraction = 0.1;
                while (i < n && isdigit(expr[i])) {
                    num += (expr[i] - '0') * fraction;
                    fraction *= 0.1;
                    i++;
                }
            }
            numStack.push(num);
        }
        // 处理左括号
        else if (expr[i] == '(') {
            opStack.push(expr[i]);
            i++;
        }
        // 处理右括号
        else if (expr[i] == ')') {
            while (!opStack.empty() && opStack.top() != '(') {
                char op = opStack.pop();
                double b = numStack.pop();
                double a = numStack.pop();
                numStack.push(operate(a, b, op));
            }
            if (opStack.empty()) {
                throw runtime_error("Mismatched parentheses (missing '(')");
            }
            opStack.pop();  // 弹出左括号
            i++;
        }
        // 处理运算符
        else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
            // 处理负号（一元运算符）
            if (expr[i] == '-' && (i == 0 || expr[i-1] == '(' || 
                expr[i-1] == '+' || expr[i-1] == '-' || 
                expr[i-1] == '*' || expr[i-1] == '/')) {
                numStack.push(0);  // 转换为 0 - 数字
            }
            
            // 弹出优先级更高或相等的运算符
            while (!opStack.empty() && precedence(opStack.top()) >= precedence(expr[i])) {
                char op = opStack.pop();
                double b = numStack.pop();
                double a = numStack.pop();
                numStack.push(operate(a, b, op));
            }
            opStack.push(expr[i]);
            i++;
        }
        // 无效字符
        else {
            throw runtime_error("Invalid character: " + string(1, expr[i]));
        }
    }
    
    // 处理剩余运算符
    while (!opStack.empty()) {
        char op = opStack.pop();
        if (op == '(') {
            throw runtime_error("Mismatched parentheses (missing ')')");
        }
        double b = numStack.pop();
        double a = numStack.pop();
        numStack.push(operate(a, b, op));
    }
    
    if (numStack.size() != 1) {
        throw runtime_error("Invalid expression");
    }
    
    return numStack.pop();
}

int main() {
    string testCases[] = {
        "3 + 4 * 2 / (1 - 5)",    // 预期结果：3 + 8 / (-4) = 3 - 2 = 1
        "10 + 20 * 30",           // 预期结果：10 + 600 = 610
        "1.5 + 2.5 * 3",          // 预期结果：1.5 + 7.5 = 9
        "(10 - 5) * (2 + 3)",     // 预期结果：5 * 5 = 25
        "100 / (2 + 3) * 5",      // 预期结果：20 * 5 = 100
        "5 - 3 * 2",              // 预期结果：5 - 6 = -1
        "10 / 0",                 // 除零错误
        "10 + (20 * 3",           // 括号不匹配错误
        "10 $ 20"                 // 无效字符错误
    };
    
    int numTests = sizeof(testCases) / sizeof(testCases[0]);
    
    for (int i = 0; i < numTests; i++) {
        cout << "表达式: " << testCases[i] << endl;
        try {
            double result = calculate(testCases[i]);
            cout << "结果: " << result << endl;
        } catch (const exception& e) {
            cout << "错误: " << e.what() << endl;
        }
        cout << "-------------------------" << endl;
    }
    
    return 0;
}
