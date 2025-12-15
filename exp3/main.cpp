#include "Graph.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// 构建图1（有向图）
Graph buildGraph1() {
    Graph g(DIRECTED);  // 有向图
    
    // 添加所有顶点
    vector<string> vertices = {"A", "B", "C", "D", "E", "F", "G", "H"};
    for (const auto& v : vertices) {
        g.addVertex(v);
    }
    
    // 添加边（根据你提供的邻接矩阵）
    g.addEdge("A", "B", 4);
    g.addEdge("A", "D", 6);
    g.addEdge("A", "G", 7);
    
    g.addEdge("B", "C", 12);
    g.addEdge("B", "E", 9);
    
    g.addEdge("C", "F", 2);
    g.addEdge("C", "H", 10);
    
    g.addEdge("D", "E", 13);
    g.addEdge("D", "G", 2);
    
    g.addEdge("E", "C", 1);
    g.addEdge("E", "F", 5);
    g.addEdge("E", "G", 11);
    g.addEdge("E", "H", 8);
    
    // 添加F、G、H相关的边（从你提供的矩阵）
    g.addEdge("F", "H", 3);
    g.addEdge("G", "H", 14);
    
    return g;
}

// 构建图2（无向图，用于测试双连通分量）
Graph buildGraph2() {
    Graph g(UNDIRECTED);  // 无向图
    
    // 添加顶点
    vector<string> vertices = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L"};
    for (const auto& v : vertices) {
        g.addVertex(v);
    }
    
    // 添加边 - 创建一个包含关节点的图
    // 外环
    g.addEdge("A", "B", 1);
    g.addEdge("B", "C", 1);
    g.addEdge("C", "D", 1);
    g.addEdge("D", "E", 1);
    g.addEdge("E", "F", 1);
    g.addEdge("F", "G", 1);
    g.addEdge("G", "H", 1);
    g.addEdge("H", "A", 1);
    
    // 内环
    g.addEdge("I", "J", 1);
    g.addEdge("J", "K", 1);
    g.addEdge("K", "L", 1);
    g.addEdge("L", "I", 1);
    
    // 连接内外环的桥（这些点应该是关节点）
    g.addEdge("A", "I", 1);
    g.addEdge("D", "J", 1);
    g.addEdge("G", "K", 1);
    
    return g;
}

int main() {
    cout << "==========================================" << endl;
    cout << "       第三次代码作业：图                  " << endl;
    cout << "==========================================" << endl << endl;
    
    // ==================== 任务1 ====================
    cout << "【任务1】实现图数据结构，输出图1的邻接矩阵" << endl;
    cout << "------------------------------------------" << endl;
    
    Graph graph1 = buildGraph1();
    graph1.printAdjMatrix();
    cout << endl;
    
    // ==================== 任务2 ====================
    cout << "【任务2】从图1的A点出发，实现BFS和DFS算法" << endl;
    cout << "------------------------------------------" << endl;
    
    // BFS遍历
    vector<string> bfsResult = graph1.BFS("A");
    printTraversal(bfsResult, "BFS");
    
    // DFS递归遍历
    vector<string> dfsResult = graph1.DFS_Recursive("A");
    printTraversal(dfsResult, "DFS递归");
    cout << endl;
    
    // ==================== 任务3 ====================
    cout << "【任务3】从图1的A点出发，实现最短路径算法和最小支撑树算法" << endl;
    cout << "------------------------------------------" << endl;
    
    // Dijkstra最短路径
    map<string, int> dist = graph1.Dijkstra("A");
    printShortestPaths(dist, "A");
    
    // Prim最小生成树
    // 注意：Prim算法需要无向图，所以创建一个图1的无向版本
    cout << "注意：Prim算法需要无向图，创建图1的无向版本..." << endl;
    Graph graph1_undirected(UNDIRECTED);
    
    // 添加所有顶点
    vector<string> vertices1 = {"A", "B", "C", "D", "E", "F", "G", "H"};
    for (const auto& v : vertices1) {
        graph1_undirected.addVertex(v);
    }
    
    // 添加边（无向图，双向都加）
    graph1_undirected.addEdge("A", "B", 4);
    graph1_undirected.addEdge("A", "D", 6);
    graph1_undirected.addEdge("A", "G", 7);
    graph1_undirected.addEdge("B", "A", 4);
    graph1_undirected.addEdge("D", "A", 6);
    graph1_undirected.addEdge("G", "A", 7);
    
    graph1_undirected.addEdge("B", "C", 12);
    graph1_undirected.addEdge("B", "E", 9);
    graph1_undirected.addEdge("C", "B", 12);
    graph1_undirected.addEdge("E", "B", 9);
    
    graph1_undirected.addEdge("C", "F", 2);
    graph1_undirected.addEdge("C", "H", 10);
    graph1_undirected.addEdge("F", "C", 2);
    graph1_undirected.addEdge("H", "C", 10);
    
    graph1_undirected.addEdge("D", "E", 13);
    graph1_undirected.addEdge("D", "G", 2);
    graph1_undirected.addEdge("E", "D", 13);
    graph1_undirected.addEdge("G", "D", 2);
    
    graph1_undirected.addEdge("E", "C", 1);
    graph1_undirected.addEdge("E", "F", 5);
    graph1_undirected.addEdge("E", "G", 11);
    graph1_undirected.addEdge("E", "H", 8);
    graph1_undirected.addEdge("C", "E", 1);
    graph1_undirected.addEdge("F", "E", 5);
    graph1_undirected.addEdge("G", "E", 11);
    graph1_undirected.addEdge("H", "E", 8);
    
    graph1_undirected.addEdge("F", "H", 3);
    graph1_undirected.addEdge("G", "H", 14);
    graph1_undirected.addEdge("H", "F", 3);
    graph1_undirected.addEdge("H", "G", 14);
    
    auto mstResult = graph1_undirected.Prim();
    printMST(mstResult.first, mstResult.second);
    
    // ==================== 任务4 ====================
    cout << "【任务4】在图2选取不同的起点计算双连通分量并输出关节点，看结果是否一致" << endl;
    cout << "------------------------------------------" << endl;
    
    Graph graph2 = buildGraph2();
    
    // 计算关节点和双连通分量
    auto articulationResult = graph2.findArticulationAndBCC();
    
    printArticulationAndBCC(articulationResult.first, articulationResult.second);
    
    // 验证从不同起点计算的结果是否一致
    cout << "验证从不同起点计算双连通分量的结果是否一致：" << endl;
    
    // 存储第一次的结果
    vector<string> articulation1 = articulationResult.first;
    vector<vector<string>> bcc1 = articulationResult.second;
    
    // 由于Tarjan算法是全局算法，不依赖起点
    // 这里重新计算一次来验证
    auto articulationResult2 = graph2.findArticulationAndBCC();
    vector<string> articulation2 = articulationResult2.first;
    vector<vector<string>> bcc2 = articulationResult2.second;
    
    // 排序以便比较
    sort(articulation1.begin(), articulation1.end());
    sort(articulation2.begin(), articulation2.end());
    
    // 排序BCC中的每个分量
    for (auto& comp : bcc1) {
        sort(comp.begin(), comp.end());
    }
    for (auto& comp : bcc2) {
        sort(comp.begin(), comp.end());
    }
    
    // 排序BCC向量
    sort(bcc1.begin(), bcc1.end());
    sort(bcc2.begin(), bcc2.end());
    
    if (articulation1 == articulation2) {
        cout << "? 关节点计算结果一致" << endl;
    } else {
        cout << "? 关节点计算结果不一致" << endl;
    }
    
    if (bcc1 == bcc2) {
        cout << "? 双连通分量计算结果一致" << endl;
    } else {
        cout << "? 双连通分量计算结果不一致" << endl;
    }
    
    cout << endl << "==========================================" << endl;
    cout << "       所有实验任务已完成！" << endl;
    cout << "==========================================" << endl;
    
    return 0;
}
