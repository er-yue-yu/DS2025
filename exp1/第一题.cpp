#include "Vector.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>

// �����ඨ��
class Complex {
private:
    double real;  // ʵ��
    double imag;  // �鲿

public:
    // ���캯��
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}

    // ��ȡģ
    double modulus() const {
        return sqrt(real * real + imag * imag);
    }

    // ʵ�����鲿��getter
    double getReal() const { return real; }
    double getImag() const { return imag; }

    // ���رȽ������(������������)
    bool operator>(const Complex& other) const {
        if (modulus() != other.modulus()) {
            return modulus() > other.modulus();
        }
        return real > other.real;
    }

    bool operator<=(const Complex& other) const {
        return !(*this > other);
    }

    // ������������(���ڲ��Һ�ȥ��)
    bool operator==(const Complex& other) const {
        return (real == other.real) && (imag == other.imag);
    }

    // ���ز��������
    bool operator!=(const Complex& other) const {
        return !(*this == other);
    }

    // ��Ԫ�������������
    friend std::ostream& operator<<(std::ostream& os, const Complex& c) {
        os << "(" << c.real << ", " << c.imag << ")";
        return os;
    }
};

// �����������
Complex randomComplex(double min, double max) {
    double r = min + (max - min) * (rand() / (RAND_MAX + 1.0));
    double i = min + (max - min) * (rand() / (RAND_MAX + 1.0));
    return Complex(r, i);
}

// ���ڱ�����ӡ�����ĺ���
void printComplex(Complex& c) {
    std::cout << c << " ";
}

// �������������Ļ�������
void testBasicOperations(Vector<Complex>& vec) {
    std::cout << "\n=== ���Ի������� ===" << std::endl;
    
    // ��ӡԭʼ����
    std::cout << "ԭʼ����: ";
    vec.traverse(printComplex);
    std::cout << std::endl;

    // ��������
    vec.unsort();
    std::cout << "���Һ�: ";
    vec.traverse(printComplex);
    std::cout << std::endl;

    // ���Բ���
    if (!vec.empty()) {
        Complex target = vec[0];
        Rank pos = vec.find(target);
        std::cout << "���� " << target << " ��λ��: " << pos << std::endl;
    }

    // ���Բ���
    Complex insertVal(10.0, 10.0);
    vec.insert(2, insertVal);
    std::cout << "��λ��2���� " << insertVal << " ��: ";
    vec.traverse(printComplex);
    std::cout << std::endl;

    // ����ɾ��
    if (vec.size() > 3) {
        Complex removed = vec.remove(3);
        std::cout << "ɾ��λ��3��Ԫ�� " << removed << " ��: ";
        vec.traverse(printComplex);
        std::cout << std::endl;
    }

    // ����Ψһ��
    int dupCount = vec.deduplicate();
    std::cout << "ȥ�غ�ɾ���� " << dupCount << " ��Ԫ��: ";
    vec.traverse(printComplex);
    std::cout << std::endl;
}

// ��������Ч��
void testSortingEfficiency(Vector<Complex>& original) {
    std::cout << "\n=== ��������Ч�� ===" << std::endl;
    int n = original.size();
    
    // ���������������ڲ�ͬ�������
    Vector<Complex> vec1(original);
    Vector<Complex> vec2(original);
    Vector<Complex> vec3(original);

    // ������������(����)
    vec1.sort();
    // ������������
    vec2.unsort();
    // ������������
    vec3.sort();
    Vector<Complex> vec3Rev;
    for (int i = vec3.size() - 1; i >= 0; --i) {
        vec3Rev.insert(vec3[i]);
    }

    // ������������
    clock_t start, end;
    
    // �������
    start = clock();
    vec1.bubbleSort(0, vec1.size());
    end = clock();
    std::cout << "��������(����): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

    // �������
    start = clock();
    vec2.bubbleSort(0, vec2.size());
    end = clock();
    std::cout << "��������(����): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

    // �������
    start = clock();
    vec3Rev.bubbleSort(0, vec3Rev.size());
    end = clock();
    std::cout << "��������(����): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

    // ����׼���������ڹ鲢�������
    vec1 = original; vec1.sort();
    vec2 = original; vec2.unsort();
    vec3Rev = Vector<Complex>();
    for (int i = vec3.size() - 1; i >= 0; --i) {
        vec3Rev.insert(vec3[i]);
    }

    // ���Թ鲢����
    // �������
    start = clock();
    vec1.mergeSort(0, vec1.size());
    end = clock();
    std::cout << "�鲢����(����): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

    // �������
    start = clock();
    vec2.mergeSort(0, vec2.size());
    end = clock();
    std::cout << "�鲢����(����): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;

    // �������
    start = clock();
    vec3Rev.mergeSort(0, vec3Rev.size());
    end = clock();
    std::cout << "�鲢����(����): " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;
}

// ������ң�����ģ����[m1, m2)������Ԫ��
Vector<Complex> rangeSearch(const Vector<Complex>& sortedVec, double m1, double m2) {
    Vector<Complex> result;
    int n = sortedVec.size();
    
    // �ҵ���һ��ģ >= m1��Ԫ��
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

    // �ҵ���һ��ģ >= m2��Ԫ��
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

    // �ռ����
    for (int i = start; i < end; ++i) {
        result.insert(sortedVec[i]);
    }
    
    return result;
}

int main() {
    srand(time(0));  // ��ʼ�������������

    // ���������������(���ظ���)
    const int VECTOR_SIZE = 1000;  // ������С�����޸��Բ��Բ�ͬ��ģ
    Vector<Complex> complexVec;
    for (int i = 0; i < VECTOR_SIZE; ++i) {
        // ����[-100, 100)��Χ�ڵ��������
        complexVec.insert(randomComplex(-100, 100));
        // �������һЩ�ظ���
        if (i % 10 == 0 && i > 0) {
            complexVec.insert(complexVec[i-1]);
        }
    }

    // ���Ի�������
    testBasicOperations(complexVec);

    // ��������Ч��
    testSortingEfficiency(complexVec);

    // �����������
    std::cout << "\n=== ����������� ===" << std::endl;
    Vector<Complex> sortedVec(complexVec);
    sortedVec.sort();  // ȷ����������
    
    double m1 = 30.0, m2 = 50.0;
    Vector<Complex> rangeResult = rangeSearch(sortedVec, m1, m2);
    
    std::cout << "ģ����[" << m1 << ", " << m2 << ")��Ԫ���� " << rangeResult.size() << " ��: " << std::endl;
    rangeResult.traverse(printComplex);
    std::cout << std::endl;

    return 0;
}
