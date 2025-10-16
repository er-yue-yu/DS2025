#include <iostream>
#include <stdexcept>
using namespace std;

// ջ�ڵ�ṹ��
template <typename T>
struct StackNode {
    T data;               // �ڵ�洢������
    StackNode<T>* next;   // ָ��ջ����һ���ڵ��ָ��
    // ���캯������ʼ�����ݺ�ָ��
    StackNode(T e = T(), StackNode<T>* n = nullptr) : data(e), next(n) {}
};

// ջ��
template <typename T>
class Stack {
private:
    int _size;            // ջ�Ĵ�С��Ԫ�ظ�����
    StackNode<T>* topNode; // ָ��ջ���ڵ��ָ��
public:
    // ���캯������ʼ��ջΪ��
    Stack() : _size(0), topNode(nullptr) {}
    // �������������ջ
    ~Stack() { clear(); }

    // ��ջ����
    void push(const T& e) {
        // �����½ڵ㣬������Ϊ�µ�ջ��
        topNode = new StackNode<T>(e, topNode);
        ++_size; // ջ��С��1
    }

    // ��ջ����
    T pop() {
        if (empty()) throw std::out_of_range("Stack is empty!"); // ջ��ʱ�׳��쳣
        StackNode<T>* node = topNode; // ���浱ǰջ���ڵ�
        T e = node->data;             // ��ȡջ������
        topNode = topNode->next;      // ����ջ��ָ��
        delete node;                  // �ͷ�ԭջ���ڵ��ڴ�
        --_size;                      // ջ��С��1
        return e;                     // ���س�ջ������
    }

    // ��ȡջ��Ԫ�ص�����
    T& top() {
        if (empty()) throw std::out_of_range("Stack is empty!"); // ջ��ʱ�׳��쳣
        return topNode->data;
    }

    // �ж�ջ�Ƿ�Ϊ��
    bool empty() const { return _size == 0; }
    // ��ȡջ�Ĵ�С
    int size() const { return _size; }
    // ���ջ
    void clear() { while (!empty()) pop(); }

    // ����ջ������ָ��汾��
    void traverse(void (*visit)(T&)) {
        for (StackNode<T>* p = topNode; p; p = p->next)
            visit(p->data);
    }

    // ����ջ����������汾��
    template <typename VST>
    void traverse(VST& visit) {
        for (StackNode<T>* p = topNode; p; p = p->next)
            visit(p->data);
    }
};
