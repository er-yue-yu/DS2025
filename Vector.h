#ifndef VECTOR_H
#define VECTOR_H

#include <cstdlib>
#include <algorithm>
#include <stdexcept>

using Rank = int;
const int DEFAULT_CAPACITY = 3;

template <typename T>
class Vector {
protected:
    Rank _size;       // 当前元素个数
    int _capacity;    // 容量
    T* _elem;         // 元素数组

    // 复制数组A的[lo, hi)区间元素
    void copyFrom(const T* A, Rank lo, Rank hi) {
        _capacity = 2 * (hi - lo);
        _elem = new T[_capacity];
        _size = 0;
        while (lo < hi) {
            _elem[_size++] = A[lo++];
        }
    }

    // 扩容
    void expand() {
        if (_size < _capacity) return;
        _capacity = std::max(_capacity, DEFAULT_CAPACITY);
        T* oldElem = _elem;
        _elem = new T[_capacity <<= 1];  // 容量翻倍
        for (Rank i = 0; i < _size; ++i) {
            _elem[i] = oldElem[i];
        }
        delete[] oldElem;
    }

    // 缩容
    void shrink() {
        if (_capacity <= DEFAULT_CAPACITY || _size * 4 > _capacity) return;
        T* oldElem = _elem;
        _elem = new T[_capacity >>= 1];  // 容量减半
        for (Rank i = 0; i < _size; ++i) {
            _elem[i] = oldElem[i];
        }
        delete[] oldElem;
    }

    // 冒泡排序一趟扫描
    bool bubble(Rank lo, Rank hi) {
        bool sorted = true;
        while (++lo < hi) {
            if (_elem[lo - 1] > _elem[lo]) {
                std::swap(_elem[lo - 1], _elem[lo]);
                sorted = false;
            }
        }
        return sorted;
    }

    // 冒泡排序（保护成员）
    void bubbleSort(Rank lo, Rank hi) {
        while (!bubble(lo, hi--));
    }

    // 归并排序合并操作
    void merge(Rank lo, Rank mi, Rank hi) {
        T* temp = new T[hi - lo];
        Rank i = lo, j = mi, k = 0;
        while (i < mi && j < hi) {
            temp[k++] = (_elem[i] <= _elem[j]) ? _elem[i++] : _elem[j++];
        }
        while (i < mi) temp[k++] = _elem[i++];
        while (j < hi) temp[k++] = _elem[j++];
        for (i = lo, k = 0; i < hi; ++i) {
            _elem[i] = temp[k++];
        }
        delete[] temp;
    }

    // 归并排序（保护成员）
    void mergeSort(Rank lo, Rank hi) {
        if (hi - lo < 2) return;
        Rank mi = (lo + hi) / 2;
        mergeSort(lo, mi);
        mergeSort(mi, hi);
        merge(lo, mi, hi);
    }

public:
    // 构造函数
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0) {
        if (s > c) throw std::invalid_argument("Initial size exceeds capacity");
        _capacity = c;
        _elem = new T[_capacity];
        _size = 0;
        while (_size < s) {
            _elem[_size++] = v;
        }
    }

    Vector(const T* A, Rank n) { copyFrom(A, 0, n); }
    Vector(const T* A, Rank lo, Rank hi) {
        if (lo < 0 || hi <= lo) throw std::invalid_argument("Invalid range");
        copyFrom(A, lo, hi);
    }
    Vector(const Vector<T>& V) { copyFrom(V._elem, 0, V._size); }
    Vector(const Vector<T>& V, Rank lo, Rank hi) {
        if (lo < 0 || hi > V._size || lo >= hi) throw std::invalid_argument("Invalid range");
        copyFrom(V._elem, lo, hi);
    }

    // 析构函数
    ~Vector() { delete[] _elem; }

    // 基本属性访问
    Rank size() const { return _size; }
    bool empty() const { return _size == 0; }

    // 元素访问
    T& operator[](Rank r) const {
        if (r < 0 || r >= _size) throw std::out_of_range("Index out of range");
        return _elem[r];
    }

    // 查找操作（核心补充）
    // 查找[lo, hi)区间内的元素e，返回索引（未找到返回-1）
    Rank find(const T& e, Rank lo, Rank hi) const {
        if (lo < 0 || hi > _size || lo >= hi) return -1;
        for (Rank i = lo; i < hi; ++i) {
            if (_elem[i] == e) return i;  // 依赖T的==运算符
        }
        return -1;
    }
    // 查找整个向量中的元素e
    Rank find(const T& e) const {
        return find(e, 0, _size);
    }

    // 插入操作
    Rank insert(Rank r, const T& e) {
        if (r < 0 || r > _size) throw std::out_of_range("Insert position out of range");
        expand();
        for (Rank i = _size; i > r; --i) {
            _elem[i] = _elem[i - 1];
        }
        _elem[r] = e;
        _size++;
        return r;
    }
    Rank insert(const T& e) { return insert(_size, e); }

    // 删除操作
    T remove(Rank r) {
        if (r < 0 || r >= _size) throw std::out_of_range("Remove index out of range");
        T e = _elem[r];
        for (Rank i = r; i < _size - 1; ++i) {
            _elem[i] = _elem[i + 1];
        }
        _size--;
        shrink();
        return e;
    }

    // 去重操作（核心补充）
    int deduplicate() {
        int oldSize = _size;
        Rank i = 1;
        while (i < _size) {
            // 查找[0, i)区间是否有重复元素
            if (find(_elem[i], 0, i) == -1) {
                i++;  // 无重复则继续
            } else {
                remove(i);  // 有重复则删除
            }
        }
        return oldSize - _size;  // 返回删除的元素个数
    }

    // 置乱操作
    void unsort(Rank lo, Rank hi) {
        if (lo < 0 || hi > _size || lo >= hi) return;
        for (Rank i = hi - 1; i > lo; --i) {
            std::swap(_elem[i], _elem[lo + rand() % (i - lo + 1)]);
        }
    }
    void unsort() { unsort(0, _size); }

    // 排序公有接口
    void bubbleSortPublic(Rank lo, Rank hi) {
        if (lo < 0 || hi > _size || lo >= hi) return;
        bubbleSort(lo, hi);
    }
    void mergeSortPublic(Rank lo, Rank hi) {
        if (lo < 0 || hi > _size || lo >= hi) return;
        mergeSort(lo, hi);
    }

    // 清空操作
    void clear() { _size = 0; }

    // 遍历操作
    void traverse(void (*visit)(T&)) {
        for (Rank i = 0; i < _size; ++i) {
            visit(_elem[i]);
        }
    }
    template <typename VST>
    void traverse(VST& visit) {
        for (Rank i = 0; i < _size; ++i) {
            visit(_elem[i]);
        }
    }
};

#endif // VECTOR_H
