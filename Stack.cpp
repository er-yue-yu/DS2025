#include <iostream>
#include <stdexcept>
using namespace std;

// 栈节点结构体
template <typename T>
struct StackNode {
    T data;               // 节点存储的数据
    StackNode<T>* next;   // 指向栈中下一个节点的指针
    // 构造函数，初始化数据和指针
    StackNode(T e = T(), StackNode<T>* n = nullptr) : data(e), next(n) {}
};

// 栈类
template <typename T>
class Stack {
private:
    int _size;            // 栈的大小（元素个数）
    StackNode<T>* topNode; // 指向栈顶节点的指针
public:
    // 构造函数，初始化栈为空
    Stack() : _size(0), topNode(nullptr) {}
    // 析构函数，清空栈
    ~Stack() { clear(); }

    // 入栈操作
    void push(const T& e) {
        // 创建新节点，将其作为新的栈顶
        topNode = new StackNode<T>(e, topNode);
        ++_size; // 栈大小加1
    }

    // 出栈操作
    T pop() {
        if (empty()) throw std::out_of_range("Stack is empty!"); // 栈空时抛出异常
        StackNode<T>* node = topNode; // 保存当前栈顶节点
        T e = node->data;             // 获取栈顶数据
        topNode = topNode->next;      // 更新栈顶指针
        delete node;                  // 释放原栈顶节点内存
        --_size;                      // 栈大小减1
        return e;                     // 返回出栈的数据
    }

    // 获取栈顶元素的引用
    T& top() {
        if (empty()) throw std::out_of_range("Stack is empty!"); // 栈空时抛出异常
        return topNode->data;
    }

    // 判断栈是否为空
    bool empty() const { return _size == 0; }
    // 获取栈的大小
    int size() const { return _size; }
    // 清空栈
    void clear() { while (!empty()) pop(); }

    // 遍历栈（函数指针版本）
    void traverse(void (*visit)(T&)) {
        for (StackNode<T>* p = topNode; p; p = p->next)
            visit(p->data);
    }

    // 遍历栈（函数对象版本）
    template <typename VST>
    void traverse(VST& visit) {
        for (StackNode<T>* p = topNode; p; p = p->next)
            visit(p->data);
    }
};
