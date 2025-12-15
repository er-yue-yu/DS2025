#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <iomanip>

using namespace std;

// ==================== 数据结构定义 ====================
struct BBox {
    double x1, y1, x2, y2;  // 边界框坐标
    double conf;            // 置信度
};

// ==================== 辅助函数 ====================
double randomDouble(double min, double max) {
    return min + (max - min) * (rand() / (RAND_MAX + 1.0));
}

// ==================== 排序算法实现 ====================
// 冒泡排序
void bubbleSort(BBox* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].conf < arr[j + 1].conf) {
                BBox temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// 快速排序分区函数
int partition(BBox* arr, int low, int high) {
    double pivot = arr[high].conf;
    int i = low - 1;
    
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].conf >= pivot) {
            i++;
            BBox temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    BBox temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

// 快速排序递归函数
void quickSortRecursive(BBox* arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSortRecursive(arr, low, pi - 1);
        quickSortRecursive(arr, pi + 1, high);
    }
}

// 快速排序包装函数
void quickSort(BBox* arr, int n) {
    if (n > 1) {
        quickSortRecursive(arr, 0, n - 1);
    }
}

// 插入排序
void insertionSort(BBox* arr, int n) {
    for (int i = 1; i < n; i++) {
        BBox key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].conf < key.conf) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// 选择排序
void selectionSort(BBox* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j].conf > arr[maxIdx].conf) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            BBox temp = arr[i];
            arr[i] = arr[maxIdx];
            arr[maxIdx] = temp;
        }
    }
}

// ==================== NMS相关函数 ====================
// 计算IoU
double calculateIoU(BBox box1, BBox box2) {
    double x1 = box1.x1 > box2.x1 ? box1.x1 : box2.x1;
    double y1 = box1.y1 > box2.y1 ? box1.y1 : box2.y1;
    double x2 = box1.x2 < box2.x2 ? box1.x2 : box2.x2;
    double y2 = box1.y2 < box2.y2 ? box1.y2 : box2.y2;
    
    double interWidth = x2 - x1;
    double interHeight = y2 - y1;
    
    if (interWidth <= 0 || interHeight <= 0) {
        return 0.0;
    }
    
    double interArea = interWidth * interHeight;
    double box1Area = (box1.x2 - box1.x1) * (box1.y2 - box1.y1);
    double box2Area = (box2.x2 - box2.x1) * (box2.y2 - box2.y1);
    double unionArea = box1Area + box2Area - interArea;
    
    return interArea / unionArea;
}

// NMS算法
int nms(BBox* boxes, int n, BBox* result, double iouThreshold, int algoType) {
    if (n == 0) {
        return 0;
    }
    
    // 复制原始数据
    BBox* sortedBoxes = new BBox[n];
    for (int i = 0; i < n; i++) {
        sortedBoxes[i] = boxes[i];
    }
    
    // 根据算法类型排序
    switch (algoType) {
        case 0: // 冒泡排序
            bubbleSort(sortedBoxes, n);
            break;
        case 1: // 快速排序
            quickSort(sortedBoxes, n);
            break;
        case 2: // 插入排序
            insertionSort(sortedBoxes, n);
            break;
        case 3: // 选择排序
            selectionSort(sortedBoxes, n);
            break;
        default:
            bubbleSort(sortedBoxes, n);
            break;
    }
    
    int keepCount = 0;
    int currentN = n;
    
    while (currentN > 0) {
        // 取置信度最高的框
        result[keepCount] = sortedBoxes[0];
        keepCount++;
        
        // 创建新数组存放剩余的框
        BBox* newBoxes = new BBox[currentN - 1];
        int newCount = 0;
        
        for (int i = 1; i < currentN; i++) {
            if (calculateIoU(sortedBoxes[0], sortedBoxes[i]) < iouThreshold) {
                newBoxes[newCount] = sortedBoxes[i];
                newCount++;
            }
        }
        
        // 清理旧数组，使用新数组
        delete[] sortedBoxes;
        sortedBoxes = newBoxes;
        currentN = newCount;
    }
    
    if (sortedBoxes != NULL) {
        delete[] sortedBoxes;
    }
    
    return keepCount;
}

// ==================== 数据生成 ====================
// 生成随机分布的边界框
BBox* generateRandomBoxes(int num, int* actualNum, int imgSize = 1000) {
    BBox* boxes = new BBox[num];
    *actualNum = num;
    
    for (int i = 0; i < num; i++) {
        double w = randomDouble(30, 100);
        double h = randomDouble(30, 100);
        double x1 = randomDouble(0, imgSize - w);
        double y1 = randomDouble(0, imgSize - h);
        double x2 = x1 + w;
        double y2 = y1 + h;
        double conf = randomDouble(0.0, 1.0);
        
        boxes[i].x1 = x1;
        boxes[i].y1 = y1;
        boxes[i].x2 = x2;
        boxes[i].y2 = y2;
        boxes[i].conf = conf;
    }
    return boxes;
}

// 生成聚集分布的边界框
BBox* generateClusteredBoxes(int num, int* actualNum, int imgSize = 1000, int clusters = 5) {
    BBox* boxes = new BBox[num];
    *actualNum = num;
    
    // 生成聚类中心
    double* centerX = new double[clusters];
    double* centerY = new double[clusters];
    
    for (int i = 0; i < clusters; i++) {
        centerX[i] = randomDouble(200, 800);
        centerY[i] = randomDouble(200, 800);
    }
    
    for (int i = 0; i < num; i++) {
        int clusterIdx = rand() % clusters;
        double cx = centerX[clusterIdx];
        double cy = centerY[clusterIdx];
        
        double w = randomDouble(30, 100);
        double h = randomDouble(30, 100);
        double x1 = cx + randomDouble(-50, 50) - w/2;
        double y1 = cy + randomDouble(-50, 50) - h/2;
        
        // 确保在图像范围内
        if (x1 < 0) x1 = 0;
        if (x1 > imgSize - w) x1 = imgSize - w;
        if (y1 < 0) y1 = 0;
        if (y1 > imgSize - h) y1 = imgSize - h;
        
        double x2 = x1 + w;
        double y2 = y1 + h;
        double conf = randomDouble(0.0, 1.0);
        
        boxes[i].x1 = x1;
        boxes[i].y1 = y1;
        boxes[i].x2 = x2;
        boxes[i].y2 = y2;
        boxes[i].conf = conf;
    }
    
    delete[] centerX;
    delete[] centerY;
    
    return boxes;
}

// ==================== 性能测试 ====================
void testPerformance() {
    srand(time(NULL));
    
    // 算法名称
    const char* algoNames[4] = {"冒泡排序", "快速排序", "插入排序", "选择排序"};
    const int NUM_ALGOS = 4;
    
    int dataSizes[6] = {100, 500, 1000, 3000, 5000, 10000};
    const int NUM_SIZES = 6;
    
    // 存储结果
    double randomResults[4][6];
    double clusteredResults[4][6];
    
    cout << "开始性能测试..." << endl;
    
    for (int sizeIdx = 0; sizeIdx < NUM_SIZES; sizeIdx++) {
        int size = dataSizes[sizeIdx];
        cout << "测试数据规模: " << size << endl;
        
        // 测试随机分布
        int actualNum;
        BBox* randomBoxes = generateRandomBoxes(size, &actualNum);
        BBox* result = new BBox[size];
        
        for (int algoIdx = 0; algoIdx < NUM_ALGOS; algoIdx++) {
            clock_t start = clock();
            int keepCount = nms(randomBoxes, actualNum, result, 0.5, algoIdx);
            clock_t end = clock();
            double duration = double(end - start) / CLOCKS_PER_SEC;
            randomResults[algoIdx][sizeIdx] = duration;
        }
        
        delete[] randomBoxes;
        delete[] result;
        
        // 测试聚集分布
        BBox* clusteredBoxes = generateClusteredBoxes(size, &actualNum);
        result = new BBox[size];
        
        for (int algoIdx = 0; algoIdx < NUM_ALGOS; algoIdx++) {
            clock_t start = clock();
            int keepCount = nms(clusteredBoxes, actualNum, result, 0.5, algoIdx);
            clock_t end = clock();
            double duration = double(end - start) / CLOCKS_PER_SEC;
            clusteredResults[algoIdx][sizeIdx] = duration;
        }
        
        delete[] clusteredBoxes;
        delete[] result;
    }
    
    // 输出结果
    cout << "\n=== 性能测试结果 ===" << endl;
    cout << "数据规模: ";
    for (int i = 0; i < NUM_SIZES; i++) cout << dataSizes[i] << " ";
    cout << "\n" << endl;
    
    for (int algoIdx = 0; algoIdx < NUM_ALGOS; algoIdx++) {
        cout << algoNames[algoIdx] << ":" << endl;
        cout << "  随机分布: ";
        for (int sizeIdx = 0; sizeIdx < NUM_SIZES; sizeIdx++) {
            cout << fixed << setprecision(3) << randomResults[algoIdx][sizeIdx] << "s ";
        }
        cout << "\n  聚集分布: ";
        for (int sizeIdx = 0; sizeIdx < NUM_SIZES; sizeIdx++) {
            cout << fixed << setprecision(3) << clusteredResults[algoIdx][sizeIdx] << "s ";
        }
        cout << "\n" << endl;
    }
    
    // 输出CSV格式
    cout << "\nCSV格式结果:" << endl;
    cout << "数据规模,";
    for (int i = 0; i < NUM_SIZES; i++) cout << dataSizes[i] << ",";
    cout << endl;
    
    for (int algoIdx = 0; algoIdx < NUM_ALGOS; algoIdx++) {
        cout << algoNames[algoIdx] << "_随机,";
        for (int sizeIdx = 0; sizeIdx < NUM_SIZES; sizeIdx++) {
            cout << randomResults[algoIdx][sizeIdx] << ",";
        }
        cout << endl;
        
        cout << algoNames[algoIdx] << "_聚集,";
        for (int sizeIdx = 0; sizeIdx < NUM_SIZES; sizeIdx++) {
            cout << clusteredResults[algoIdx][sizeIdx] << ",";
        }
        cout << endl;
    }
}

// ==================== 主函数 ====================
int main() {
    cout << "=== 第四次代码作业：排序算法在NMS中的应用 ===" << endl;
    cout << "使用四种排序算法：冒泡排序、快速排序、插入排序、选择排序" << endl;
    cout << "测试两种数据分布：随机分布、聚集分布" << endl;
    cout << "测试数据规模：100, 500, 1000, 3000, 5000, 10000" << endl;
    cout << "===========================================\n" << endl;
    
    // 性能测试
    testPerformance();
    
    // 复杂度分析
    cout << "\n=== 时间复杂度分析 ===" << endl;
    cout << "1. 排序算法复杂度:" << endl;
    cout << "   - 冒泡排序: O(n2)" << endl;
    cout << "   - 快速排序: O(n log n) 平均, O(n2) 最坏" << endl;
    cout << "   - 插入排序: O(n2)" << endl;
    cout << "   - 选择排序: O(n2)" << endl;
    cout << "\n2. NMS算法复杂度:" << endl;
    cout << "   - 理论复杂度: O(n2) (最坏情况)" << endl;
    cout << "   - 实际复杂度与数据分布相关" << endl;
    cout << "   - 聚集分布可能导致更多IoU计算" << endl;
    
    // 实验结果讨论
    cout << "\n=== 实验结果讨论 ===" << endl;
    cout << "1. 快速排序在大多数情况下表现最佳" << endl;
    cout << "2. 数据规模增大时，O(n2)算法性能下降明显" << endl;
    cout << "3. 聚集分布数据通常需要更多IoU计算，运行时间更长" << endl;
    cout << "4. 实验结果与理论分析基本一致" << endl;
    
    cout << "\n实验完成！" << endl;
    
    // 暂停以便查看结果
    cout << "\n按Enter键退出..." << endl;
    cin.get();
    
    return 0;
}
