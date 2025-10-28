#include "Stack.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// 计算柱状图中最大矩形面积（基于单调栈）
int largestRectangleArea(vector<int>& heights) {
    Stack<int> stk;  // 存储柱子索引的单调栈
    int maxArea = 0;
    int n = heights.size();

    for (int i = 0; i < n; ++i) {
        // 当当前柱子高度小于栈顶柱子高度时，计算栈顶柱子的最大面积
        while (!stk.empty() && heights[i] < heights[stk.top()]) {
            int topIdx = stk.top();  // 获取栈顶索引
            stk.pop();               // 弹出栈顶元素
            int height = heights[topIdx];
            int left = stk.empty() ? -1 : stk.top();  // 左边界
            int width = i - left - 1;
            maxArea = max(maxArea, height * width);
        }
        stk.push(i);  // 当前柱子索引入栈
    }

    // 处理栈中剩余的柱子（右边界为数组长度n）
    while (!stk.empty()) {
        int topIdx = stk.top();
        stk.pop();
        int height = heights[topIdx];
        int left = stk.empty() ? -1 : stk.top();
        int width = n - left - 1;
        maxArea = max(maxArea, height * width);
    }

    return maxArea;
}

// 随机生成10组测试数据并测试
void testRandomCases() {
    srand(time(0));
    cout << "随机生成10组测试数据（1≤长度≤105，0≤高度≤104）：" << endl;
    for (int t = 1; t <= 10; ++t) {
        int len = rand() % 105 + 1;  // 数组长度1~105
        vector<int> heights(len);
        for (int i = 0; i < len; ++i) {
            heights[i] = rand() % 105;  // 高度0~104
        }

        cout << "测试用例" << t << "：";
        if (len <= 10) {
            cout << "heights = [";
            for (int i = 0; i < len; ++i) {
                if (i > 0) cout << ",";
                cout << heights[i];
            }
            cout << "]";
        } else {
            cout << "长度 = " << len;
        }
        cout << endl;

        cout << "最大矩形面积：" << largestRectangleArea(heights) << endl << endl;
    }
}

int main() {
    // 示例1测试
    vector<int> heights1 = {2, 1, 5, 6, 2, 3};
    cout << "示例1：" << endl;
    cout << "输入：heights = [2,1,5,6,2,3]" << endl;
    cout << "输出：" << largestRectangleArea(heights1) << endl << endl;

    // 示例2测试
    vector<int> heights2 = {2, 4};
    cout << "示例2：" << endl;
    cout << "输入：heights = [2,4]" << endl;
    cout << "输出：" << largestRectangleArea(heights2) << endl << endl;

    // 随机测试
    testRandomCases();

    return 0;
}
