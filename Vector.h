#ifndef VECTOR_H
#define VECTOR_H

#include <cstdlib>
#include <algorithm> // 用于std::swap
#include <stdexcept> // 用于异常处理

typedef int Rank;
#define DEFAULT_CAPACITY 3

template <typename T>
class Vector {
protected:
    Rank _size;       // 当前元素个数
    int _capacity;    // 容量
    T* _elem;         // 元素数组

    // 从数组A的[lo, hi)区间复制元素
    void copyFrom(const T* A, Rank lo, Rank hi) {
        _capacity = 2 * (hi - lo);  // 初始容量设为元素个数的2倍
        _elem = new T[_capacity];
        _size = 0;
        while (lo < hi) {
            _elem[_size++] = A[lo++];  // 复制元素
        }
    }

    // 扩容操作（容量不足时）
    void expand() {
        if (_size < _capacity) return;  // 无需扩容
        _capacity = std::max(_capacity, DEFAULT_CAPACITY);  // 确保最小容量
        T* oldElem = _elem;
        _elem = new T[_capacity <<= 1];  // 容量翻倍
        for (Rank i = 0; i < _size; ++i) {
            _elem[i] = oldElem[i];  // 复制旧元素
        }
        delete[] oldElem;  // 释放旧空间
    }

    // 缩容操作（容量过大时）
    void shrink() {
        if (_capacity <= DEFAULT_CAPACITY || _size * 4 > _capacity) return;  // 无需缩容
        T* oldElem = _elem;
        _elem = new T[_capacity >>= 1];  // 容量减半
        for (Rank i = 0; i < _size; ++i) {
            _elem[i] = oldElem[i];  // 复制旧元素
        }
        delete[] oldElem;  // 释放旧空间
    }

    // 冒泡排序的一趟扫描（[lo, hi)）
    bool bubble(Rank lo, Rank hi) {
        bool sorted = true;
        while (++lo < hi) {
            if (_elem[lo - 1] > _elem[lo]) {
                std::swap(_elem[lo - 1], _elem[lo]);  // 交换逆序对
                sorted = false;
            }
        }
        return sorted;
    }

    // 冒泡排序（[lo, hi)）
    void bubbleSort(Rank lo, Rank hi) {
        while (!bubble(lo, hi--));  // 逐趟扫描，直到有序
    }

    // 查找[lo, hi)区间的最大元素索引
    Rank max(Rank lo, Rank hi) {
        Rank maxIdx = lo;
        for (Rank i = lo + 1; i < hi; ++i) {
            if (_elem[i] > _elem[maxIdx]) {
                maxIdx = i;
            }
        }
        return maxIdx;
    }

    // 选择排序（[lo, hi)）
    void selectionSort(Rank lo, Rank hi) {
        while (lo < --hi) {
            std::swap(_elem[max(lo, hi)], _elem[hi]);  // 将最大元素放到末尾
        }
    }

    // 归并操作（合并[lo, mi)和[mi, hi)）
    void merge(Rank lo, Rank mi, Rank hi) {
        T* temp = new T[hi - lo];  // 临时数组
        Rank i = lo, j = mi, k = 0;
        while (i < mi && j < hi) {  // 合并两个有序区间
            temp[k++] = (_elem[i] <= _elem[j]) ? _elem[i++] : _elem[j++];
        }
        while (i < mi) temp[k++] = _elem[i++];  // 处理剩余元素
        while (j < hi) temp[k++] = _elem[j++];
        for (i = lo, k = 0; i < hi; ++i) {  // 复制回原数组
            _elem[i] = temp[k++];
        }
        delete[] temp;  // 释放临时空间
    }

    // 归并排序（[lo, hi)）
    void mergeSort(Rank lo, Rank hi) {
        if (hi - lo < 2) return;  // 单个元素自然有序
        Rank mi = (lo + hi) / 2;
        mergeSort(lo, mi);        // 左半部分排序
        mergeSort(mi, hi);        // 右半部分排序
        merge(lo, mi, hi);        // 归并
    }

    // 快速排序的划分操作（[lo, hi)）
    Rank partition(Rank lo, Rank hi) {
        std::swap(_elem[lo], _elem[lo + rand() % (hi - lo)]);  // 随机选择基准元素
        T pivot = _elem[lo];
        --hi;  // 从hi-1开始
        while (lo < hi) {
            while (lo < hi && pivot <= _elem[hi]) --hi;  // 从右向左找小于基准的元素
            _elem[lo] = _elem[hi];
            while (lo < hi && _elem[lo] <= pivot) ++lo;  // 从左向右找大于基准的元素
            _elem[hi] = _elem[lo];
        }
        _elem[lo] = pivot;  // 基准元素归位
        return lo;          // 返回基准位置
    }

    // 快速排序（[lo, hi)）
    void quickSort(Rank lo, Rank hi) {
        if (hi - lo < 2) return;  // 单个元素自然有序
        Rank pivot = partition(lo, hi);
        quickSort(lo, pivot);     // 左半部分排序
        quickSort(pivot + 1, hi); // 右半部分排序
    }

    // 堆排序辅助：下滤操作
    void heapify(Rank i, Rank n) {
        while (true) {
            Rank left = 2 * i + 1;
            Rank right = 2 * i + 2;
            Rank largest = i;
            if (left < n && _elem[left] > _elem[largest]) largest = left;
            if (right < n && _elem[right] > _elem[largest]) largest = right;
            if (largest != i) {
                std::swap(_elem[i], _elem[largest]);
                i = largest;
            } else {
                break;
            }
        }
    }

    // 堆排序（[lo, hi)）
    void heapSort(Rank lo, Rank hi) {
        int n = hi - lo;
        // 构建最大堆
        for (int i = n / 2 - 1; i >= 0; --i) {
            heapify(lo + i, lo + n);
        }
        // 提取最大元素并调整堆
        for (int i = n - 1; i > 0; --i) {
            std::swap(_elem[lo], _elem[lo + i]);
            heapify(lo, lo + i);
        }
    }

public:
    // 构造函数：指定容量、初始元素个数和初始值
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0) {
        if (s > c) throw std::invalid_argument("初始元素个数不能超过容量");
        _capacity = c;
        _elem = new T[_capacity];
        _size = 0;
        while (_size < s) {
            _elem[_size++] = v;
        }
    }

    // 从数组构造（n个元素）
    Vector(const T* A, Rank n) {
        copyFrom(A, 0, n);
    }

    // 从数组的[lo, hi)区间构造
    Vector(const T* A, Rank lo, Rank hi) {
    if (lo < 0 || hi <= lo) throw std::invalid_argument("无效区间");
    copyFrom(A, lo, hi);
}

    // 复制构造（整个向量）
    Vector(const Vector<T>& V) {
        copyFrom(V._elem, 0, V._size);
    }

    // 复制构造（向量的[lo, hi)区间）
    Vector(const Vector<T>& V, Rank lo, Rank hi) {
        if (lo < 0 || hi > V._size || lo >= hi) throw std::invalid_argument("无效的区间");
        copyFrom(V._elem, lo, hi);
    }

    // 析构函数
    ~Vector() {
        delete[] _elem;
    }

    // 获取当前元素个数
    Rank size() const {
        return _size;
    }

    // 判断是否为空
    bool empty() const {
        return _size == 0;
    }

    // 检查向量是否有序（返回逆序对数量）
    int disordered() const {
        int cnt = 0;
        for (Rank i = 1; i < _size; ++i) {
            if (_elem[i - 1] > _elem[i]) cnt++;
        }
        return cnt;
    }

    // 查找元素e（整个向量）
    Rank find(const T& e) const {
        return find(e, 0, _size);
    }

    // 查找[lo, hi)区间的元素e（返回首次出现的索引，未找到返回-1）
    Rank find(const T& e, Rank lo, Rank hi) const {
        if (lo < 0 || hi > _size || lo >= hi) return -1;
        while (lo < hi) {
            if (_elem[lo] == e) return lo;
            lo++;
        }
        return -1;
    }

    // 有序向量中查找元素e（整个向量）
    Rank search(const T& e) const {
        return (0 >= _size) ? -1 : search(e, 0, _size);
    }

    // 有序向量中查找[lo, hi)区间的元素e（二分查找）
    Rank search(const T& e, Rank lo, Rank hi) const {
        if (lo < 0 || hi > _size || lo >= hi) return -1;
        while (lo < hi) {
            Rank mi = (lo + hi) / 2;
            if (e < _elem[mi]) {
                hi = mi;
            } else {
                lo = mi + 1;
            }
        }
        return --lo;  // 返回不大于e的最大元素索引
    }

    // 重载[]运算符（访问元素）
    T& operator[](Rank r) const {
        if (r < 0 || r >= _size) throw std::out_of_range("索引越界");
        return _elem[r];
    }

    // 重载=运算符（向量赋值）
    Vector<T>& operator=(const Vector<T>& V) {
        if (_elem) delete[] _elem;  // 释放当前空间
        copyFrom(V._elem, 0, V._size);  // 复制新内容
        return *this;
    }

    // 删除索引r处的元素（返回被删除元素）
    T remove(Rank r) {
        if (r < 0 || r >= _size) throw std::out_of_range("索引越界");
        T e = _elem[r];
        for (Rank i = r; i < _size - 1; ++i) {
            _elem[i] = _elem[i + 1];  // 元素前移
        }
        _size--;
        shrink();  // 检查是否需要缩容
        return e;
    }

    // 删除[lo, hi)区间的元素（返回删除的元素个数）
    int remove(Rank lo, Rank hi) {
        if (lo < 0 || hi > _size || lo >= hi) return 0;
        int cnt = hi - lo;
        while (hi < _size) {
            _elem[lo++] = _elem[hi++];  // 元素前移
        }
        _size -= cnt;
        shrink();  // 检查是否需要缩容
        return cnt;
    }

    // 在索引r处插入元素e（返回插入位置）
    Rank insert(Rank r, const T& e) {
        if (r < 0 || r > _size) throw std::out_of_range("插入位置越界");
        expand();  // 检查是否需要扩容
        for (Rank i = _size; i > r; --i) {
            _elem[i] = _elem[i - 1];  // 元素后移
        }
        _elem[r] = e;
        _size++;
        return r;
    }

    // 在末尾插入元素e（返回插入位置）
    Rank insert(const T& e) {
        return insert(_size, e);
    }

    // 排序[lo, hi)区间（默认使用快速排序）
    void sort(Rank lo, Rank hi) {
        if (lo < 0 || hi > _size || lo >= hi) return;
        // 可根据需要切换排序算法
        quickSort(lo, hi);
        // 其他可选算法：
        // mergeSort(lo, hi);
        // bubbleSort(lo, hi);
        // selectionSort(lo, hi);
        // heapSort(lo, hi);
    }

    // 排序整个向量
    void sort() {
        sort(0, _size);
    }

    // 打乱[lo, hi)区间的元素
    void unsort(Rank lo, Rank hi) {
        if (lo < 0 || hi > _size || lo >= hi) return;
        for (Rank i = hi - 1; i > lo; --i) {
            std::swap(_elem[i], _elem[lo + rand() % (i - lo + 1)]);  // 随机交换
        }
    }

    // 打乱整个向量
    void unsort() {
        unsort(0, _size);
    }

    // 无序向量去重（返回删除的元素个数）
    int deduplicate() {
        int oldSize = _size;
        Rank i = 1;
        while (i < _size) {
            if (find(_elem[i], 0, i) == -1) {
                i++;  // 未找到重复元素，继续
            } else {
                remove(i);  // 找到重复元素，删除
            }
        }
        return oldSize - _size;
    }

    // 有序向量去重（返回删除的元素个数）
    int uniquify() {
        if (_size < 2) return 0;
        Rank i = 0, j = 1;
        while (j < _size) {
            if (_elem[i] != _elem[j]) {
                _elem[++i] = _elem[j];  // 保留不重复元素
            }
            j++;
        }
        int cnt = _size - (i + 1);
        _size = i + 1;
        shrink();  // 检查是否需要缩容
        return cnt;
    }

    // 遍历向量（函数指针版本）
    void traverse(void (*visit)(T&)) {
        for (Rank i = 0; i < _size; ++i) {
            visit(_elem[i]);
        }
    }

    // 遍历向量（函数对象版本）
    template <typename VST>
    void traverse(VST& visit) {
        for (Rank i = 0; i < _size; ++i) {
            visit(_elem[i]);
        }
    }
};

#endif // VECTOR_H

