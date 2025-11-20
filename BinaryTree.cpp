#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 二叉树节点结构定义（通用数据类型，可通过typedef修改）
typedef int BTDataType;  // 数据类型，实验时可改为char、float等
typedef struct BinaryTreeNode {
    BTDataType data;                  // 节点数据
    struct BinaryTreeNode* left;      // 左孩子指针
    struct BinaryTreeNode* right;     // 右孩子指针
} BTNode;

// ========================= 基础操作 =========================

/**
 * @brief  创建一个新节点
 * @param  data: 节点存储的数据
 * @return 成功返回节点指针，失败返回NULL（内存分配失败）
 */
BTNode* BTNodeCreate(BTDataType data);

/**
 * @brief  销毁二叉树（递归实现，释放所有节点内存）
 * @param  root: 二叉树根节点的指针的指针（需修改实参为NULL）
 * @note   调用后原根节点指针会被置为NULL，避免野指针
 */
void BTDestroy(BTNode** root);

/**
 * @brief  计算二叉树节点总数
 * @param  root: 二叉树根节点
 * @return 节点总数（空树返回0）
 */
int BTSize(BTNode* root);

/**
 * @brief  计算二叉树高度（深度）
 * @param  root: 二叉树根节点
 * @return 树的高度（空树返回0，单个节点返回1）
 */
int BTHeight(BTNode* root);

/**
 * @brief  查找值为data的节点
 * @param  root: 二叉树根节点
 * @param  data: 要查找的数据
 * @return 找到返回节点指针，未找到返回NULL
 */
BTNode* BTFind(BTNode* root, BTDataType data);

// ========================= 遍历操作 =========================

/**
 * @brief  前序遍历（根->左->右）- 递归实现
 * @param  root: 二叉树根节点
 * @param  visit: 回调函数，用于处理节点数据（如打印、存储等）
 */
void BTPreOrder(BTNode* root, void (*visit)(BTDataType));

/**
 * @brief  中序遍历（左->根->右）- 递归实现
 * @param  root: 二叉树根节点
 * @param  visit: 回调函数，用于处理节点数据
 */
void BTInOrder(BTNode* root, void (*visit)(BTDataType));

/**
 * @brief  后序遍历（左->右->根）- 递归实现
 * @param  root: 二叉树根节点
 * @param  visit: 回调函数，用于处理节点数据
 */
void BTPostOrder(BTNode* root, void (*visit)(BTDataType));

/**
 * @brief  层序遍历（按层次从上到下、从左到右）- 队列实现
 * @param  root: 二叉树根节点
 * @param  visit: 回调函数，用于处理节点数据
 * @note   内部使用动态队列，自动释放内存，无需外部管理
 */
void BTLevelOrder(BTNode* root, void (*visit)(BTDataType));

// ========================= 插入操作 =========================

/**
 * @brief  按二叉搜索树规则插入节点（左小右大，不允许重复值）
 * @param  root: 二叉树根节点的指针的指针
 * @param  data: 要插入的数据
 * @return 成功返回true，失败返回false（重复值或内存分配失败）
 * @note   若树为空，会创建新根节点
 */
bool BTSearchInsert(BTNode** root, BTDataType data);

/**
 * @brief  在指定节点的左子树插入新节点
 * @param  parent: 父节点指针（不能为NULL）
 * @param  data: 要插入的数据
 * @return 成功返回true，失败返回false（内存分配失败）
 */
bool BTInsertLeft(BTNode* parent, BTDataType data);

/**
 * @brief  在指定节点的右子树插入新节点
 * @param  parent: 父节点指针（不能为NULL）
 * @param  data: 要插入的数据
 * @return 成功返回true，失败返回false（内存分配失败）
 */
bool BTInsertRight(BTNode* parent, BTDataType data);

// ========================= 删除操作 =========================

/**
 * @brief  按二叉搜索树规则删除节点（左小右大）
 * @param  root: 二叉树根节点的指针的指针
 * @param  data: 要删除的数据
 * @return 成功返回true，失败返回false（未找到节点）
 * @note   处理三种情况：叶子节点、单孩子节点、双孩子节点（找中序后继）
 */
bool BTSearchDelete(BTNode** root, BTDataType data);

// ========================= 辅助函数（可选） =========================

/**
 * @brief  打印节点数据（供遍历函数调用的示例回调函数）
 * @param  data: 节点数据
 */
void BTPrint(BTDataType data);

/**
 * @brief  判断二叉树是否为空
 * @param  root: 二叉树根节点
 * @return 空树返回true，非空返回false
 */
static inline bool BTIsEmpty(BTNode* root) {
    return root == NULL;
}

// ========================= 函数实现 =========================

// 创建节点
BTNode* BTNodeCreate(BTDataType data) {
    BTNode* newNode = (BTNode*)malloc(sizeof(BTNode));
    if (newNode == NULL) {
        perror("BTNodeCreate: malloc failed");  // 打印错误信息
        return NULL;
    }
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// 销毁树（递归释放）
void BTDestroy(BTNode** root) {
    if (root == NULL || *root == NULL) {
        return;
    }
    // 后序遍历释放：先释放左右子树，再释放根节点
    BTDestroy(&((*root)->left));
    BTDestroy(&((*root)->right));
    free(*root);
    *root = NULL;  // 置空，避免野指针
}

// 节点总数
int BTSize(BTNode* root) {
    return root == NULL ? 0 : 1 + BTSize(root->left) + BTSize(root->right);
}

// 树的高度
int BTHeight(BTNode* root) {
    if (root == NULL) {
        return 0;
    }
    int leftH = BTHeight(root->left);
    int rightH = BTHeight(root->right);
    return (leftH > rightH ? leftH : rightH) + 1;
}

// 查找节点
BTNode* BTFind(BTNode* root, BTDataType data) {
    if (root == NULL) {
        return NULL;
    }
    if (root->data == data) {
        return root;
    }
    // 递归查找左子树
    BTNode* leftFind = BTFind(root->left, data);
    if (leftFind != NULL) {
        return leftFind;
    }
    // 递归查找右子树
    return BTFind(root->right, data);
}

// 前序遍历
void BTPreOrder(BTNode* root, void (*visit)(BTDataType)) {
    if (root == NULL || visit == NULL) {
        return;
    }
    visit(root->data);          // 根
    BTPreOrder(root->left, visit);  // 左
    BTPreOrder(root->right, visit); // 右
}

// 中序遍历
void BTInOrder(BTNode* root, void (*visit)(BTDataType)) {
    if (root == NULL || visit == NULL) {
        return;
    }
    BTInOrder(root->left, visit);   // 左
    visit(root->data);          // 根
    BTInOrder(root->right, visit);  // 右
}

// 后序遍历
void BTPostOrder(BTNode* root, void (*visit)(BTDataType)) {
    if (root == NULL || visit == NULL) {
        return;
    }
    BTPostOrder(root->left, visit);  // 左
    BTPostOrder(root->right, visit); // 右
    visit(root->data);           // 根
}

// 层序遍历（队列实现）
void BTLevelOrder(BTNode* root, void (*visit)(BTDataType)) {
    if (root == NULL || visit == NULL) {
        return;
    }

    // 动态队列：存储节点指针
    BTNode** queue = (BTNode**)malloc(sizeof(BTNode*) * BTSize(root));
    if (queue == NULL) {
        perror("BTLevelOrder: malloc failed");
        return;
    }

    int front = 0;  // 队头
    int rear = 0;   // 队尾（下一个插入位置）

    queue[rear++] = root;  // 根节点入队

    while (front < rear) {
        BTNode* cur = queue[front++];  // 出队
        visit(cur->data);              // 处理当前节点

        // 左孩子入队
        if (cur->left != NULL) {
            queue[rear++] = cur->left;
        }
        // 右孩子入队
        if (cur->right != NULL) {
            queue[rear++] = cur->right;
        }
    }

    free(queue);  // 释放队列内存
}

// 二叉搜索树插入（左小右大，无重复）
bool BTSearchInsert(BTNode** root, BTDataType data) {
    if (root == NULL) {
        return false;
    }
    // 树为空，创建根节点
    if (*root == NULL) {
        *root = BTNodeCreate(data);
        return *root != NULL;
    }
    // 找到重复值，插入失败
    if ((*root)->data == data) {
        fprintf(stderr, "BTSearchInsert: data %d already exists\n", data);
        return false;
    }
    // 小于当前节点，插入左子树
    if (data < (*root)->data) {
        return BTSearchInsert(&((*root)->left), data);
    }
    // 大于当前节点，插入右子树
    else {
        return BTSearchInsert(&((*root)->right), data);
    }
}

// 插入左孩子
bool BTInsertLeft(BTNode* parent, BTDataType data) {
    if (parent == NULL) {
        fprintf(stderr, "BTInsertLeft: parent is NULL\n");
        return false;
    }
    // 若左孩子已存在，提示并覆盖（可根据需求修改为不允许覆盖）
    if (parent->left != NULL) {
        fprintf(stderr, "BTInsertLeft: parent's left child already exists, overwrite it\n");
        free(parent->left);  // 释放原有节点
    }
    parent->left = BTNodeCreate(data);
    return parent->left != NULL;
}

// 插入右孩子
bool BTInsertRight(BTNode* parent, BTDataType data) {
    if (parent == NULL) {
        fprintf(stderr, "BTInsertRight: parent is NULL\n");
        return false;
    }
    // 若右孩子已存在，提示并覆盖
    if (parent->right != NULL) {
        fprintf(stderr, "BTInsertRight: parent's right child already exists, overwrite it\n");
        free(parent->right);  // 释放原有节点
    }
    parent->right = BTNodeCreate(data);
    return parent->right != NULL;
}

// 二叉搜索树删除（内部辅助：找中序后继节点）
static BTNode* BTFindMin(BTNode* root) {
    while (root != NULL && root->left != NULL) {
        root = root->left;
    }
    return root;  // 返回右子树中最小的节点（中序后继）
}

// 二叉搜索树删除
bool BTSearchDelete(BTNode** root, BTDataType data) {
    if (root == NULL || *root == NULL) {
        return false;  // 未找到节点
    }

    // 1. 查找要删除的节点
    if (data < (*root)->data) {
        return BTSearchDelete(&((*root)->left), data);
    } else if (data > (*root)->data) {
        return BTSearchDelete(&((*root)->right), data);
    } else {
        // 找到要删除的节点，分三种情况处理
        BTNode* temp = *root;

        // 情况1：叶子节点（无孩子）
        if ((*root)->left == NULL && (*root)->right == NULL) {
            *root = NULL;
        }
        // 情况2：只有右孩子
        else if ((*root)->left == NULL) {
            *root = (*root)->right;
        }
        // 情况3：只有左孩子
        else if ((*root)->right == NULL) {
            *root = (*root)->left;
        }
        // 情况4：有两个孩子（找中序后继节点替换）
        else {
            BTNode* minNode = BTFindMin((*root)->right);  // 右子树最小节点
            (*root)->data = minNode->data;                // 替换数据
            // 删除中序后继节点
            BTSearchDelete(&((*root)->right), minNode->data);
            temp = NULL;  // 无需释放当前节点（已被后继节点替换）
        }

        if (temp != NULL) {
            free(temp);  // 释放被删除的节点
        }
        return true;
    }
}

// 打印节点数据（示例回调函数）
void BTPrint(BTDataType data) {
    printf("%d ", data);
}

#endif  // BINARY_TREE_H
