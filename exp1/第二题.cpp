#include"C:\Users\������\DS2025\Stack.h"
#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>  // �����쳣����ͷ�ļ�

using namespace std;

// ��������ȼ��ж�
int precedence(char op) {
    switch(op) {
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        default: return 0; // �����������'('������0
    }
}

// ִ������
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

// �ַ���������������
double calculate(const string& expr) {
    Stack<double> numStack;  // ������ջ��ʹ��ģ��ջ��
    Stack<char> opStack;     // �����ջ��ʹ��ģ��ջ��

    int i = 0;
    int n = expr.size();
    
    while (i < n) {
        if (isspace(expr[i])) {  // �����ո�
            i++;
            continue;
        }
        
        // �������֣�������С����
        if (isdigit(expr[i]) || expr[i] == '.') {
            double num = 0;
            // ��������
            while (i < n && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            // С������
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
        // ����������
        else if (expr[i] == '(') {
            opStack.push(expr[i]);
            i++;
        }
        // ����������
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
            opStack.pop();  // ����������
            i++;
        }
        // ���������
        else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
            // �����ţ�һԪ�������
            if (expr[i] == '-' && (i == 0 || expr[i-1] == '(' || 
                expr[i-1] == '+' || expr[i-1] == '-' || 
                expr[i-1] == '*' || expr[i-1] == '/')) {
                numStack.push(0);  // ת��Ϊ 0 - ����
            }
            
            // �������ȼ����߻���ȵ������
            while (!opStack.empty() && precedence(opStack.top()) >= precedence(expr[i])) {
                char op = opStack.pop();
                double b = numStack.pop();
                double a = numStack.pop();
                numStack.push(operate(a, b, op));
            }
            opStack.push(expr[i]);
            i++;
        }
        // ��Ч�ַ�
        else {
            throw runtime_error("Invalid character: " + string(1, expr[i]));
        }
    }
    
    // ����ʣ�������
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
        "3 + 4 * 2 / (1 - 5)",    // Ԥ�ڽ����3 + 8 / (-4) = 3 - 2 = 1
        "10 + 20 * 30",           // Ԥ�ڽ����10 + 600 = 610
        "1.5 + 2.5 * 3",          // Ԥ�ڽ����1.5 + 7.5 = 9
        "(10 - 5) * (2 + 3)",     // Ԥ�ڽ����5 * 5 = 25
        "100 / (2 + 3) * 5",      // Ԥ�ڽ����20 * 5 = 100
        "5 - 3 * 2",              // Ԥ�ڽ����5 - 6 = -1
        "10 / 0",                 // �������
        "10 + (20 * 3",           // ���Ų�ƥ�����
        "10 $ 20"                 // ��Ч�ַ�����
    };
    
    int numTests = sizeof(testCases) / sizeof(testCases[0]);
    
    for (int i = 0; i < numTests; i++) {
        cout << "���ʽ: " << testCases[i] << endl;
        try {
            double result = calculate(testCases[i]);
            cout << "���: " << result << endl;
        } catch (const exception& e) {
            cout << "����: " << e.what() << endl;
        }
        cout << "-------------------------" << endl;
    }
    
    return 0;
}
