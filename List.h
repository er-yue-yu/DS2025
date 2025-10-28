#include <iostream>
#include <cstdlib>  // 用于 rand() 函数（排序时随机选择算法）
using namespace std;

// 定义秩（Rank）：用于表示链表中节点的位置索引（类似数组下标）
typedef int Rank;

// 链表节点结构模板
template <typename T> 
struct ListNode {
    T data;               // 节点存储的数据
    ListNode<T>* pred;    // 指向前驱节点的指针
    ListNode<T>* succ;    // 指向后继节点的指针

    // 节点构造函数
    // 参数：e（数据）、p（前驱指针）、s（后继指针），均有默认值
    ListNode(T e = T(), ListNode<T>* p = nullptr, ListNode<T>* s = nullptr)
        : data(e), pred(p), succ(s) {}
};

// 定义节点位置的别名：简化 ListNode<T>* 的书写
// 模板别名需 C++11 及以上标准支持（编译时需加 -std=c++11）
template <typename T> 
using ListNodePosi = ListNode<T>*;

// 双向链表类模板（带哨兵节点，简化边界处理）
template <typename T> 
class List {
private:
    int _size;               // 链表实际节点数量（不包含哨兵）
    ListNodePosi<T> header;  // 头哨兵节点（不存储数据，简化头部操作）
    ListNodePosi<T> trailer; // 尾哨兵节点（不存储数据，简化尾部操作）

protected:
    // 初始化链表：创建哨兵节点并建立初始连接
    void init() {
        header = new ListNode<T>();  // 创建头哨兵（默认构造）
        trailer = new ListNode<T>(); // 创建尾哨兵（默认构造）
        header->succ = trailer;      // 头哨兵的后继指向尾哨兵
        trailer->pred = header;      // 尾哨兵的前驱指向头哨兵
        _size = 0;                   // 初始长度为 0
    }

    // 清空链表：删除所有实际节点（保留哨兵）
    int clear() {
        int oldSize = _size;         // 记录原始长度
        while (_size > 0) {
            remove(header->succ);    // 从第一个实际节点开始删除
        }
        return oldSize;              // 返回删除的节点数
    }

    // 复制节点：从 p 开始复制 n 个节点到当前链表
    void copyNodes(ListNodePosi<T> p, int n) {
        init();                      // 先初始化当前链表
        while (n-- > 0) {
            insertAsLast(p->data);   // 复制数据到尾部
            p = p->succ;             // 移动到下一个节点
        }
    }

    // 归并算法：将当前链表中 p 开始的 n 个节点与 L 中 q 开始的 m 个节点归并
    // 前提：两部分都是有序的，归并后整体有序
    void merge(ListNodePosi<T>& p, int n, List<T>& L, ListNodePosi<T> q, int m) {
        ListNodePosi<T> pp = p->pred;  // 记录 p 的前驱（用于归并后连接）
        while (m > 0 && n > 0) {       // 两边都有节点时循环
            // 取较小的节点插入到当前链表
            if (q->data < p->data) {
                insertB(p, L.remove(q));  // 将 L 中 q 节点移到 p 之前
                q = L.first();            // 更新 q 为 L 的新首节点
                m--;
            } else {
                p = p->succ;  // 当前节点较小，直接后移
                n--;
            }
        }
        // 若 L 中还有剩余节点，全部插入当前链表
        if (m > 0) {
            while (m-- > 0) {
                ListNodePosi<T> next = q->succ;  // 提前记录下一个节点
                insertB(p, L.remove(q));
                q = next;
            }
        }
        _size += m;    // 更新当前链表长度（此时 m 已为 0，不影响）
        L._size -= m;  // 更新 L 的长度
    }

    // 归并排序：对 p 开始的 n 个节点进行排序（分治思想）
    void mergeSort(ListNodePosi<T> p, int n) {
        if (n < 2) return;  // 单个节点无需排序
        int mid = n >> 1;   // 取中点（等价于 n/2，效率更高）
        ListNodePosi<T> q = p;
        for (int i = 0; i < mid; i++) q = q->succ;  // 找到后半段起点
        mergeSort(p, mid);         // 递归排序前半段
        mergeSort(q, n - mid);     // 递归排序后半段
        merge(p, mid, *this, q, n - mid);  // 归并两段
    }

    // 选择排序：对 p 开始的 n 个节点进行排序（每次选最大元素放尾部）
    void selectionSort(ListNodePosi<T> p, int n) {
        ListNodePosi<T> tail = p;
        // 找到排序区间的尾后位置（初始为第 n 个节点的后继）
        for (int i = 0; i < n; i++) tail = tail->succ;
        while (n > 1) {  // 直到只剩一个节点
            ListNodePosi<T> max = selectMax(p, n);  // 找到最大节点
            // 交换最大节点与当前尾节点的数据
            swap(max->data, (tail->pred)->data);
            tail = tail->pred;  // 尾节点前移
            n--;
        }
    }

    // 插入排序：对 p 开始的 n 个节点进行排序（逐个插入到前面的有序区间）
    void insertionSort(ListNodePosi<T> p, int n) {
        for (int i = 0; i < n; i++) {
            // 找到插入位置并插入当前节点数据
            insertA(search(p->data, i, p), p->data);
            p = p->succ;       // 处理下一个节点
            remove(p->pred);   // 删除原位置的节点
        }
    }

public:
    // 无参构造函数：初始化空链表
    List() { init(); }

    // 复制构造函数：复制整个链表 L
    List(List<T> const& L) { copyNodes(L.first(), L._size); }

    // 部分复制构造：复制 L 中从秩 r 开始的 n 个节点
    List(List<T> const& L, Rank r, int n) {
        ListNodePosi<T> p = L.first();
        for (int i = 0; i < r; i++) p = p->succ;  // 移动到秩 r 对应的节点
        copyNodes(p, n);
    }

    // 从节点复制构造：复制从节点 p 开始的 n 个节点
    List(ListNodePosi<T> p, int n) { copyNodes(p, n); }

    // 析构函数：释放所有节点（包括哨兵）
    ~List() {
        clear();         // 先删除所有实际节点
        delete header;   // 释放头哨兵
        delete trailer;  // 释放尾哨兵
    }

    // 返回链表长度
    Rank size() const { return _size; }

    // 判断链表是否为空
    bool empty() const { return _size <= 0; }

    // 重载 [] 运算符：通过秩访问节点数据（类似数组）
    // 时间复杂度 O(n)，链表不适合随机访问，仅为方便使用
    T& operator[](Rank r) const {
        ListNodePosi<T> p = first();
        for (int i = 0; i < r; i++) p = p->succ;  // 移动到第 r 个节点
        return p->data;
    }

    // 返回第一个实际节点（头哨兵的后继）
    ListNodePosi<T> first() const { return header->succ; }

    // 返回最后一个实际节点（尾哨兵的前驱）
    ListNodePosi<T> last() const { return trailer->pred; }

    // 判断节点 p 是否为有效节点（非哨兵且非空）
    bool valid(ListNodePosi<T> p) {
        return p != nullptr && p != header && p != trailer;
    }

    // 计算链表中逆序对的数量（用于判断链表是否有序）
    // 若返回 0，说明链表完全有序
    int disordered() const {
        int cnt = 0;
        ListNodePosi<T> p = first();
        for (int i = 1; i < _size; i++) {
            p = p->succ;
            if (p->pred->data > p->data) cnt++;  // 前驱 > 后继，形成逆序
        }
        return cnt;
    }

    // 查找节点：在 p 的前 n 个节点中查找值为 e 的节点（从 p 向前找）
    // 返回找到的节点指针，未找到返回 nullptr
    ListNodePosi<T> find(T const& e, int n, ListNodePosi<T> p) const {
        while (n-- > 0) {
            p = p->pred;
            if (e == p->data) return p;
        }
        return nullptr;
    }

    // 搜索插入位置：在 p 的前 n 个节点中查找 <= e 的最大节点（用于插入排序）
    ListNodePosi<T> search(T const& e, int n, ListNodePosi<T> p) const {
        while (n-- > 0) {
            p = p->pred;
            if (p->data <= e) break;  // 找到符合条件的节点
        }
        return p;  // 返回插入位置的前驱
    }

    // 查找最大值节点：在 p 开始的 n 个节点中找到数据最大的节点
    ListNodePosi<T> selectMax(ListNodePosi<T> p, int n) {
        ListNodePosi<T> max = p;  // 初始假设第一个节点是最大的
        for (ListNodePosi<T> cur = p; n > 1; n--) {
            cur = cur->succ;
            if (cur->data >= max->data) max = cur;  // 更新最大值节点
        }
        return max;
    }

    // 在链表头部插入节点，返回新节点的指针
    ListNodePosi<T> insertAsFirst(T const& e) {
        _size++;  // 长度 +1
        // 新节点的前驱是头哨兵，后继是原首节点
        // 同时更新头哨兵的后继和原首节点的前驱指向新节点
        return header->succ = header->succ->pred = new ListNode<T>(e, header, header->succ);
    }

    // 在链表尾部插入节点，返回新节点的指针
    ListNodePosi<T> insertAsLast(T const& e) {
        _size++;  // 长度 +1
        // 新节点的前驱是原尾节点，后继是尾哨兵
        // 同时更新原尾节点的后继和尾哨兵的前驱指向新节点
        return trailer->pred = trailer->pred->succ = new ListNode<T>(e, trailer->pred, trailer);
    }

    // 在节点 p 之后插入节点，返回新节点的指针
    ListNodePosi<T> insertA(ListNodePosi<T> p, T const& e) {
        _size++;  // 长度 +1
        // 新节点的前驱是 p，后继是 p 的原后继
        // 同时更新 p 的后继和 p 原后继的前驱指向新节点
        return p->succ = p->succ->pred = new ListNode<T>(e, p, p->succ);
    }

    // 在节点 p 之前插入节点，返回新节点的指针
    ListNodePosi<T> insertB(ListNodePosi<T> p, T const& e) {
        _size++;  // 长度 +1
        // 新节点的前驱是 p 的原前驱，后继是 p
        // 同时更新 p 原前驱的后继和 p 的前驱指向新节点
        return p->pred = p->pred->succ = new ListNode<T>(e, p->pred, p);
    }

    // 删除节点 p，返回节点中存储的数据
    T remove(ListNodePosi<T> p) {
        T e = p->data;  // 保存节点数据
        // 断开 p 与前后节点的连接：p 的前驱指向 p 的后继，p 的后继指向 p 的前驱
        p->pred->succ = p->succ;
        p->succ->pred = p->pred;
        delete p;       // 释放 p 的内存
        _size--;        // 长度 -1
        return e;       // 返回删除的数据
    }

    // 归并当前链表与链表 L（归并后 L 会被清空）
    void merge(List<T>& L) {
        merge(first(), _size, L, L.first(), L._size);
    }

    // 排序：对 p 开始的 n 个节点排序（随机选择三种算法之一）
    void sort(ListNodePosi<T> p, int n) {
        switch (rand() % 3) {
            case 0: insertionSort(p, n); break;  // 插入排序（适合小数据或近似有序）
            case 1: selectionSort(p, n); break;  // 选择排序（简单但效率低）
            default: mergeSort(p, n);            // 归并排序（效率高，O(nlogn)）
        }
    }

    // 对整个链表排序
    void sort() { sort(first(), _size); }

    // 去重：删除链表中所有重复的节点（保留第一个出现的节点）
    // 时间复杂度 O(n2)，适合无序链表
    int deduplicate() {
        int oldSize = _size;
        ListNodePosi<T> p = first();
        for (Rank i = 0; i < _size; p = p->succ, i++) {
            // 在 p 之前的 i 个节点中查找是否有重复
            ListNodePosi<T> q = find(p->data, i, p);
            if (q) remove(q);  // 有重复则删除
        }
        return oldSize - _size;  // 返回删除的节点数
    }

    // 有序去重：删除有序链表中连续重复的节点（仅适用于有序链表）
    // 时间复杂度 O(n)，效率更高
    int uniquify() {
        if (_size < 2) return 0;  // 空链表或单节点无需去重
        int oldSize = _size;
        ListNodePosi<T> p = first(), q;
        while ((q = p->succ) != trailer) {  // 遍历到尾哨兵为止
            if (p->data != q->data) {
                p = q;  // 不重复则后移
            } else {
                remove(q);  // 重复则删除 q
            }
        }
        return oldSize - _size;  // 返回删除的节点数
    }

    // 反转链表：交换每个节点的前驱和后继指针
    void reverse() {
        ListNodePosi<T> p = header;
        // 交换每个实际节点的前驱和后继
        for (int i = 0; i < _size; i++) {
            p = p->succ;
            swap(p->pred, p->succ);  // 交换指针
        }
        // 交换头哨兵和尾哨兵的指向
        swap(header->succ, trailer->pred);
        // 修复最后一个节点与尾哨兵的连接
        swap(p->pred, p->succ);
    }

    // 遍历链表（函数指针版本）：对每个节点执行 visit 操作
    void traverse(void (*visit)(T&)) {
        for (ListNodePosi<T> p = first(); p != trailer; p = p->succ) {
            visit(p->data);
        }
    }

    // 遍历链表（函数对象版本）：支持更灵活的操作（如带状态的函数）
    template <typename VST>
    void traverse(VST& visit) {
        for (ListNodePosi<T> p = first(); p != trailer; p = p->succ) {
            visit(p->data);
        }
    }
};

// 测试函数：打印节点数据（用于遍历测试）
template <typename T>
void print(T& data) {
    cout << data << " ";
}


