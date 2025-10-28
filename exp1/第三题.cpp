#include "Stack.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// ������״ͼ����������������ڵ���ջ��
int largestRectangleArea(vector<int>& heights) {
    Stack<int> stk;  // �洢���������ĵ���ջ
    int maxArea = 0;
    int n = heights.size();

    for (int i = 0; i < n; ++i) {
        // ����ǰ���Ӹ߶�С��ջ�����Ӹ߶�ʱ������ջ�����ӵ�������
        while (!stk.empty() && heights[i] < heights[stk.top()]) {
            int topIdx = stk.top();  // ��ȡջ������
            stk.pop();               // ����ջ��Ԫ��
            int height = heights[topIdx];
            int left = stk.empty() ? -1 : stk.top();  // ��߽�
            int width = i - left - 1;
            maxArea = max(maxArea, height * width);
        }
        stk.push(i);  // ��ǰ����������ջ
    }

    // ����ջ��ʣ������ӣ��ұ߽�Ϊ���鳤��n��
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

// �������10��������ݲ�����
void testRandomCases() {
    srand(time(0));
    cout << "�������10��������ݣ�1�ܳ��ȡ�105��0�ܸ߶ȡ�104����" << endl;
    for (int t = 1; t <= 10; ++t) {
        int len = rand() % 105 + 1;  // ���鳤��1~105
        vector<int> heights(len);
        for (int i = 0; i < len; ++i) {
            heights[i] = rand() % 105;  // �߶�0~104
        }

        cout << "��������" << t << "��";
        if (len <= 10) {
            cout << "heights = [";
            for (int i = 0; i < len; ++i) {
                if (i > 0) cout << ",";
                cout << heights[i];
            }
            cout << "]";
        } else {
            cout << "���� = " << len;
        }
        cout << endl;

        cout << "�����������" << largestRectangleArea(heights) << endl << endl;
    }
}

int main() {
    // ʾ��1����
    vector<int> heights1 = {2, 1, 5, 6, 2, 3};
    cout << "ʾ��1��" << endl;
    cout << "���룺heights = [2,1,5,6,2,3]" << endl;
    cout << "�����" << largestRectangleArea(heights1) << endl << endl;

    // ʾ��2����
    vector<int> heights2 = {2, 4};
    cout << "ʾ��2��" << endl;
    cout << "���룺heights = [2,4]" << endl;
    cout << "�����" << largestRectangleArea(heights2) << endl << endl;

    // �������
    testRandomCases();

    return 0;
}
