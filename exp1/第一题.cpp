#include "Vector.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>

// 复数类定义
class Complex {
private:
    double real;  // 实部
    double imag;  // 虚部

public:
    // 构造函数
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}

    // 获取模
    double modulus() const {
        return sqrt(real * real + imag * imag);
    }

    // 实部和虚部的getter
    double getReal() const { return real; }
    double getImag() const { return imag; }

    // 重载比较运算符(用于起泡排序)
    bool operator>(const Complex& other) const {
        if (modulus() != other.modulus()) {
            return modulus() > other.modulus();
        }
        return real > other.real;
    }

    bool operator<=(const Complex& other) const {
        return !(*this > other);
    }

    // 重载相等运算符(用于查找和去重)
    bool operator==(const Complex& other) const {
        return (real == other.real) && (imag == other.imag);
    }

    // 重载不等运算符
    bool operator!=(const Complex& other) const {
        return !(*this == other);
    }

    // 友元函数：输出复数
    friend std::ostream& operator<<(std::ostream& os, const Complex& c) {
        os << "(" << c.real << ", " << c.imag << ")";
        return os;
    }
};

// 生成随机复数
Complex randomComplex(double min, double max) {
    double r = min + (max - min) * (rand() / (RAND_MAX + 1.0));
    double i = min + (max - min) * (rand() / (RAND_MAX + 1.0));
    return Complex(r, i);
}

// 用于遍历打印复数的函数
void printComplex(Complex& c) {
    std::cout << c << " ";
}

// 测试无序向量的基本操作
void testBasicOperations(Vector<Complex>& vec) {
    std::cout << "\n=== 测试基本操作 ===" << std::endl;
    
    // 打印原始向量
    std::cout << "原始向量: ";
    vec.traverse(printComplex);
    std::cout << std::endl;

    // 测试置乱
    vec.unsort();
    std::cout << "置乱后: ";
    vec.traverse(printComplex);
    std::cout << std::endl;

    // 测试查找
    if (!vec.empty()) {
        Complex target = vec[0];
        Rank pos = vec.find(target);
        std::cout << "查找 " << target << " 的位置: " << pos << std::endl;
    }

    // 测试插入
    Complex insertVal(10.0, 10.0);
    vec.insert(2, insertVal);
    std::cout << "在位置2插入 " << insertVal << " 后: ";
    vec.traverse(printComplex);
    std::cout << std::endl;

    // 测试删除
    if (vec.size() > 3) {
        Complex removed = vec.remove(3);
        std::cout << "删除位置3的元素 " << removed << " 后: ";
        vec.traverse(printComplex);
        std::cout << std::endl;
    }

    // 测试唯一化
    int dupCount = vec.deduplicate();
    std::cout << "去重后删除了 " << dupCount << " 个元素: ";
    vec.traverse(printComplex);
    std::cout << std::endl;
}

// 测试排序效率
void testSortingEfficiency(Vector<Complex>& original) {
    std::cout << "\n=== 测试排序效率 ===" << std::endl;
    int n = original.size();
    
    // 复制三个向量用于不同排序测试
    Vector<Complex> vec1(original);
    Vector<Complex> vec2(original);
    Vector<Complex> vec3(original);

    // 生成有序向量(升序)
    vec1.sort();
    // 生成乱序向量
    vec2.unsort();
    // 生成逆序向量
    vec3.sort();
    Vector<Complex> vec3Rev;
    for (int i = vec3.size() - 1; i >= 0; --i) {
        vec3Rev.insert(vec3[i]);
    }

    // 测试起泡排序
    clock_t start, end;
    
    // 有序情况
    start = clock();
    vec1.bubbleSort(0, vec1.size());
    end = clock();
    std::cout << "起泡排序(有序): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

    // 乱序情况
    start = clock();
    vec2.bubbleSort(0, vec2.size());
    end = clock();
    std::cout << "起泡排序(乱序): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

    // 逆序情况
    start = clock();
    vec3Rev.bubbleSort(0, vec3Rev.size());
    end = clock();
    std::cout << "起泡排序(逆序): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

    // 重新准备向量用于归并排序测试
    vec1 = original; vec1.sort();
    vec2 = original; vec2.unsort();
    vec3Rev = Vector<Complex>();
    for (int i = vec3.size() - 1; i >= 0; --i) {
        vec3Rev.insert(vec3[i]);
    }

    // 测试归并排序
    // 有序情况
    start = clock();
    vec1.mergeSort(0, vec1.size());
    end = clock();
    std::cout << "归并排序(有序): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

    // 乱序情况
    start = clock();
    vec2.mergeSort(0, vec2.size());
    end = clock();
    std::cout << "归并排序(乱序): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

    // 逆序情况
    start = clock();
    vec3Rev.mergeSort(0, vec3Rev.size());
    end = clock();
    std::cout << "归并排序(逆序): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;
}

// 区间查找：查找模介于[m1, m2)的所有元素
Vector<Complex> rangeSearch(const Vector<Complex>& sortedVec, double m1, double m2) {
    Vector<Complex> result;
    int n = sortedVec.size();
    
    // 找到第一个模 >= m1的元素
    int left = 0, right = n;
    while (left < right) {
        int mid = (left + right) / 2;
        if (sortedVec[mid].modulus() >= m1) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    int start = left;

    // 找到第一个模 >= m2的元素
    left = 0; right = n;
    while (left < right) {
        int mid = (left + right) / 2;
        if (sortedVec[mid].modulus() >= m2) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    int end = left;

    // 收集结果
    for (int i = start; i < end; ++i) {
        result.insert(sortedVec[i]);
    }
    
    return result;
}

int main() {
    srand(time(0));  // 初始化随机数生成器

    // 生成随机复数向量(有重复项)
    const int VECTOR_SIZE = 1000;  // 向量大小，可修改以测试不同规模
    Vector<Complex> complexVec;
    for (int i = 0; i < VECTOR_SIZE; ++i) {
        // 生成[-100, 100)范围内的随机复数
        complexVec.insert(randomComplex(-100, 100));
        // 故意插入一些重复项
        if (i % 10 == 0 && i > 0) {
            complexVec.insert(complexVec[i-1]);
        }
    }

    // 测试基本操作
    testBasicOperations(complexVec);

    // 测试排序效率
    testSortingEfficiency(complexVec);

    // 测试区间查找
    std::cout << "\n=== 测试区间查找 ===" << std::endl;
    Vector<Complex> sortedVec(complexVec);
    sortedVec.sort();  // 确保向量有序
    
    double m1 = 30.0, m2 = 50.0;
    Vector<Complex> rangeResult = rangeSearch(sortedVec, m1, m2);
    
    std::cout << "模介于[" << m1 << ", " << m2 << ")的元素有 " << rangeResult.size() << " 个: " << std::endl;
    rangeResult.traverse(printComplex);
    std::cout << std::endl;

    return 0;
}
