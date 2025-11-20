#include "BinaryTree.h"
#include <new>
#include <cstring>
#include <cctype>
#include <cstdio>

// ========================= 修复后的Bitmap类 =========================
class Bitmap {
private:
    mutable unsigned char* M;
    mutable int N;
    mutable int _sz;

    void expand(int k) const {
        if (k < 8 * N || M == nullptr) return;
        int newN = (2 * k + 7) / 8;
        unsigned char* newM = new (std::nothrow) unsigned char[newN]();
        if (newM == nullptr) return;
        memcpy(newM, M, N);
        delete[] M;
        M = newM;
        N = newN;
    }

    void init(int n) {
        N = (n + 7) / 8;
        M = new (std::nothrow) unsigned char[N]();
        if (M == nullptr) N = 0;
        _sz = 0;
    }

public:
    Bitmap(int n = 32) { init(n); }
    ~Bitmap() { 
        delete[] M; 
        M = nullptr; 
        N = 0; 
        _sz = 0; 
    }

    // 禁用拷贝构造（避免值传递导致的双重释放）
    Bitmap(const Bitmap& other) = delete;
    Bitmap& operator=(const Bitmap& other) = delete;

    bool is_valid() const { return M != nullptr; }
    int size() const { return _sz; }

    void set(int k) {
        if (!is_valid()) return;
        expand(k);
        M[k >> 3] |= (0x80 >> (k & 0x07));
        _sz++;
    }

    void clear(int k) {
        if (!is_valid()) return;
        expand(k);
        M[k >> 3] &= ~(0x80 >> (k & 0x07));
        _sz--;
    }

    bool test(int k) const {
        if (!is_valid() || k >= 8 * N) return false;
        expand(k);
        return (M[k >> 3] & (0x80 >> (k & 0x07))) != 0;
    }

    char* bits2string(int n) const {
        if (!is_valid() || n <= 0) return new char[1]{'\0'};
        expand(n - 1);
        char* s = new (std::nothrow) char[n + 1];
        if (s == nullptr) return new char[1]{'\0'};
        s[n] = '\0';
        for (int i = 0; i < n; i++) {
            s[i] = test(i) ? '1' : '0';
        }
        return s;
    }
};

// ========================= Huffman编码类型 =========================
typedef struct {
    Bitmap bits;
    int length;
} HuffCode;

// ========================= 节点信息链表 =========================
typedef struct NodeInfo {
    BTNode* node;
    int freq;
    char type;
    struct NodeInfo* next;
} NodeInfo;
static NodeInfo* g_infoHead = nullptr;

bool addNodeInfo(BTNode* node, int freq, char type) {
    if (node == nullptr) return false;
    NodeInfo* cur = g_infoHead;
    while (cur != nullptr) {
        if (cur->node == node) {
            cur->freq = freq;
            cur->type = type;
            return true;
        }
        cur = cur->next;
    }
    NodeInfo* newInfo = new (std::nothrow) NodeInfo();
    if (newInfo == nullptr) return false;
    newInfo->node = node;
    newInfo->freq = freq;
    newInfo->type = type;
    newInfo->next = g_infoHead;
    g_infoHead = newInfo;
    return true;
}

NodeInfo* getNodeInfo(BTNode* node) {
    if (node == nullptr) return nullptr;
    NodeInfo* cur = g_infoHead;
    while (cur != nullptr) {
        if (cur->node == node) return cur;
        cur = cur->next;
    }
    return nullptr;
}

void freeNodeInfoList() {
    NodeInfo* cur = g_infoHead;
    while (cur != nullptr) {
        NodeInfo* temp = cur;
        cur = cur->next;
        delete temp;
    }
    g_infoHead = nullptr;
}

// ========================= 辅助函数 =========================
void countLetterFreq(const char* text, int freq[]) {
    memset(freq, 0, 26 * sizeof(int));
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        char ch = tolower(text[i]);
        if (ch >= 'a' && ch <= 'z') {
            freq[ch - 'a']++;
        }
    }
}

#define HEAP_SIZE 26
typedef struct {
    BTNode* data[HEAP_SIZE];
    int size;
} MinHeap;

void MinHeapInit(MinHeap* heap) {
    heap->size = 0;
    memset(heap->data, 0, HEAP_SIZE * sizeof(BTNode*));
}

void MinHeapAdjust(MinHeap* heap, int parent) {
    int child = 2 * parent + 1;
    BTNode* temp = heap->data[parent];
    while (child < heap->size) {
        NodeInfo* leftInfo = getNodeInfo(heap->data[child]);
        NodeInfo* rightInfo = (child + 1 < heap->size) ? getNodeInfo(heap->data[child + 1]) : leftInfo;
        int leftFreq = (leftInfo != nullptr) ? leftInfo->freq : 0;
        int rightFreq = (rightInfo != nullptr) ? rightInfo->freq : leftFreq + 1;

        if (child + 1 < heap->size && rightFreq < leftFreq) child++;

        NodeInfo* tempInfo = getNodeInfo(temp);
        int tempFreq = (tempInfo != nullptr) ? tempInfo->freq : 0;
        if (tempFreq <= leftFreq) break;

        heap->data[parent] = heap->data[child];
        parent = child;
        child = 2 * parent + 1;
    }
    heap->data[parent] = temp;
}

bool MinHeapInsert(MinHeap* heap, BTNode* node, int freq, char type) {
    if (heap->size >= HEAP_SIZE || node == nullptr) return false;
    if (!addNodeInfo(node, freq, type)) return false;

    int i = heap->size++;
    while (i > 0) {
        BTNode* parentNode = heap->data[(i - 1) / 2];
        NodeInfo* parentInfo = getNodeInfo(parentNode);
        NodeInfo* nodeInfo = getNodeInfo(node);
        if (parentInfo == nullptr || nodeInfo == nullptr) break;

        if (nodeInfo->freq >= parentInfo->freq) break;

        heap->data[i] = parentNode;
        i = (i - 1) / 2;
    }
    heap->data[i] = node;
    return true;
}

BTNode* MinHeapExtractMin(MinHeap* heap) {
    if (heap->size == 0) return nullptr;
    BTNode* minNode = heap->data[0];
    heap->data[0] = heap->data[--heap->size];
    MinHeapAdjust(heap, 0);
    return minNode;
}

// ========================= Huffman核心功能（位图指针传递） =========================
BTNode* buildHuffTree(int freq[]) {
    freeNodeInfoList();
    MinHeap heap;
    MinHeapInit(&heap);

    for (int i = 0; i < 26; i++) {
        if (freq[i] > 0) {
            char ch = 'a' + i;
            BTNode* leaf = BTNodeCreate(ch);
            if (leaf == nullptr) continue;
            if (!MinHeapInsert(&heap, leaf, freq[i], ch)) {
                BTDestroy(&leaf);
            }
        }
    }

    while (heap.size > 1) {
        BTNode* left = MinHeapExtractMin(&heap);
        BTNode* right = MinHeapExtractMin(&heap);
        if (left == nullptr || right == nullptr) {
            if (left != nullptr) BTDestroy(&left);
            if (right != nullptr) BTDestroy(&right);
            freeNodeInfoList();
            return nullptr;
        }

        NodeInfo* leftInfo = getNodeInfo(left);
        NodeInfo* rightInfo = getNodeInfo(right);
        if (leftInfo == nullptr || rightInfo == nullptr) {
            BTDestroy(&left);
            BTDestroy(&right);
            freeNodeInfoList();
            return nullptr;
        }
        int mergeFreq = leftInfo->freq + rightInfo->freq;

        BTNode* merge = BTNodeCreate('#');
        if (merge == nullptr) {
            BTDestroy(&left);
            BTDestroy(&right);
            freeNodeInfoList();
            return nullptr;
        }
        merge->left = left;
        merge->right = right;

        if (!MinHeapInsert(&heap, merge, mergeFreq, '#')) {
            BTDestroy(&merge);
            BTDestroy(&left);
            BTDestroy(&right);
            freeNodeInfoList();
            return nullptr;
        }
    }

    return MinHeapExtractMin(&heap);
}

// 关键修复：tempBits改为指针传递，避免值传递的双重释放
void generateHuffCode(BTNode* root, HuffCode codeTable[], Bitmap* tempBits, int tempLen) {
    if (root == nullptr || tempBits == nullptr) return;

    NodeInfo* nodeInfo = getNodeInfo(root);
    if (nodeInfo == nullptr) return;

    // 叶子节点处理
    if (nodeInfo->type != '#' && root->left == nullptr && root->right == nullptr) {
        int idx = nodeInfo->type - 'a';
        codeTable[idx].length = tempLen;
        if (tempBits->is_valid()) {
            for (int i = 0; i < tempLen; i++) {
                if (tempBits->test(i)) {
                    codeTable[idx].bits.set(i);
                } else {
                    codeTable[idx].bits.clear(i);
                }
            }
        }
        return;
    }

    // 左子树：编码加0
    tempBits->clear(tempLen);
    generateHuffCode(root->left, codeTable, tempBits, tempLen + 1);

    // 右子树：编码加1
    tempBits->set(tempLen);
    generateHuffCode(root->right, codeTable, tempBits, tempLen + 1);
}

void initHuffCodeTable(HuffCode codeTable[]) {
    for (int i = 0; i < 26; i++) {
        codeTable[i].length = 0;
        new (&codeTable[i].bits) Bitmap(32);
    }
}

char* encodeWord(const char* word, HuffCode codeTable[]) {
    if (word == nullptr) return nullptr;
    int wordLen = strlen(word);
    if (wordLen == 0) return nullptr;

    int totalLen = 0;
    for (int i = 0; i < wordLen; i++) {
        char ch = tolower(word[i]);
        if (ch < 'a' || ch > 'z') {
            fprintf(stderr, "encodeWord: 无效字符 '%c'\n", ch);
            return nullptr;
        }
        int idx = ch - 'a';
        if (codeTable[idx].length == 0 || !codeTable[idx].bits.is_valid()) {
            fprintf(stderr, "encodeWord: 未找到 '%c' 的编码\n", ch);
            return nullptr;
        }
        totalLen += codeTable[idx].length;
    }

    char* result = new (std::nothrow) char[totalLen + 1];
    if (result == nullptr) return nullptr;
    result[totalLen] = '\0';
    int pos = 0;

    for (int i = 0; i < wordLen; i++) {
        char ch = tolower(word[i]);
        int idx = ch - 'a';
        char* bitStr = codeTable[idx].bits.bits2string(codeTable[idx].length);
        if (bitStr == nullptr) {
            delete[] result;
            return nullptr;
        }
        strcpy(result + pos, bitStr);
        pos += codeTable[idx].length;
        delete[] bitStr;
    }

    return result;
}

// ========================= 主函数 =========================
int main() {
    printf("=== 开始执行《I have a dream》Huffman编码实验 ===\n\n");

    const char* dreamText = "I have a dream that one day this nation will rise up and live out the true meaning of its creed "
                           "We hold these truths to be self-evident that all men are created equal "
                           "I have a dream that one day on the red hills of Georgia the sons of former slaves and the sons of former slave owners will be able to sit down together at the table of brotherhood "
                           "I have a dream that one day even the state of Mississippi a state sweltering with the heat of injustice sweltering with the heat of oppression will be transformed into an oasis of freedom and justice "
                           "I have a dream that my four little children will one day live in a nation where they will not be judged by the color of their skin but by the content of their character "
                           "I have a dream today";

    int letterFreq[26] = {0};
    countLetterFreq(dreamText, letterFreq);
    printf("=== 26个字母出现频率 ===\n");
    for (int i = 0; i < 26; i++) {
        if (letterFreq[i] > 0) {
            printf("%c: %d次\n", 'a' + i, letterFreq[i]);
        }
    }
    printf("\n");

    printf("=== 开始构建Huffman树 ===\n");
    BTNode* huffRoot = buildHuffTree(letterFreq);
    if (huffRoot == nullptr) {
        fprintf(stderr, "错误：Huffman树构建失败\n");
        freeNodeInfoList();
        return 1;
    }
    printf("Huffman树构建成功！\n\n");

    printf("=== 开始生成Huffman编码表 ===\n");
    HuffCode huffCodeTable[26];
    initHuffCodeTable(huffCodeTable);
    Bitmap tempBits(32); // 单个位图对象，避免复制

    if (!tempBits.is_valid()) {
        fprintf(stderr, "错误：临时位图初始化失败\n");
        BTDestroy(&huffRoot);
        freeNodeInfoList();
        return 1;
    }

    // 传入位图指针，而非值传递
    generateHuffCode(huffRoot, huffCodeTable, &tempBits, 0);
    printf("Huffman编码表生成成功！\n\n");

    printf("=== Huffman编码表 ===\n");
    int codeCount = 0;
    for (int i = 0; i < 26; i++) {
        if (letterFreq[i] > 0 && huffCodeTable[i].length > 0 && huffCodeTable[i].bits.is_valid()) {
            char* codeStr = huffCodeTable[i].bits.bits2string(huffCodeTable[i].length);
            printf("%c: %s （长度：%d）\n", 'a' + i, codeStr, huffCodeTable[i].length);
            delete[] codeStr;
            codeCount++;
        }
    }
    if (codeCount == 0) {
        fprintf(stderr, "警告：未生成任何编码\n");
    } else {
        printf("共生成 %d 个字母的编码\n", codeCount);
    }
    printf("\n");

    printf("=== 单词Huffman编码结果 ===\n");
    const char* testWords[] = {"dream", "equal", "freedom", "justice", "character"};
    int wordCount = sizeof(testWords) / sizeof(testWords[0]);
    for (int i = 0; i < wordCount; i++) {
        char* code = encodeWord(testWords[i], huffCodeTable);
        if (code != nullptr) {
            printf("%s: %s\n", testWords[i], code);
            delete[] code;
        } else {
            printf("%s: 编码失败\n", testWords[i]);
        }
    }

    BTDestroy(&huffRoot);
    freeNodeInfoList();
    printf("\n=== 实验结束，所有资源已释放 ===\n");
    return 0;
}
