#ifndef GRAPH_H
#define GRAPH_H
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <climits>
#include <set>
#include <map>
#include <tuple>
#include <string>

// 图类型枚举
enum GraphType {
    UNDIRECTED,  // 无向图
    DIRECTED     // 有向图
};

// 边结构体（固定int权重）
struct Edge {
    int to;   // 目标顶点索引
    int w;    // 边权重
    Edge(int t = -1, int weight = 1) : to(t), w(weight) {}
};

// 图类
class Graph {
private:
    GraphType type;                          // 图类型
    std::vector<std::string> vertices;       // 顶点集合
    std::vector<std::vector<Edge>> adjList;   // 邻接表
    std::vector<std::vector<int>> adjMatrix;  // 邻接矩阵
    int vertexCount;                         // 顶点数
    int edgeCount;                           // 边数
    const int INF = 0x3f3f3f3f;              // 无符号大整数，避免溢出

    // 查找顶点索引（不存在返回-1）
    int findVertexIndex(const std::string& v) const {
        for (int i = 0; i < vertexCount; ++i) {
            if (vertices[i] == v) return i;
        }
        return -1;
    }

    // DFS递归辅助函数（修复：移除结构化绑定，修正变量名）
    void dfsRecursive(int startIdx, std::vector<bool>& visited, std::vector<std::string>& result) const {
        visited[startIdx] = true;
        result.push_back(vertices[startIdx]);
        // 邻接顶点排序，确保遍历顺序稳定
        std::vector<std::pair<int, int>> adjVerts; // <顶点索引, 权重>
        for (const auto& edge : adjList[startIdx]) {
            adjVerts.emplace_back(edge.to, edge.w);
        }
        sort(adjVerts.begin(), adjVerts.end()); // 按顶点索引排序
        // 修复：用普通pair访问，替代结构化绑定
        for (const auto& pair : adjVerts) {
            int vIdx = pair.first; // 修正变量名：vIdx（非vldx）
            int w = pair.second;
            if (!visited[vIdx]) {
                dfsRecursive(vIdx, visited, result);
            }
        }
    }

    // Tarjan算法辅助函数（const修饰）
    void tarjan(int u, int parent, int& time, std::vector<int>& disc, std::vector<int>& low,
                std::vector<bool>& visited, std::vector<bool>& isArticulation,
                std::stack<std::pair<int, int>>& edgeStack, std::vector<std::vector<std::string>>& bcc) const {
        int children = 0;
        disc[u] = low[u] = ++time;
        visited[u] = true;
        for (const auto& edge : adjList[u]) {
            int v = edge.to;
            if (!visited[v]) {
                children++;
                edgeStack.push(std::make_pair(u, v));
                tarjan(v, u, time, disc, low, visited, isArticulation, edgeStack, bcc);
                low[u] = std::min(low[u], low[v]);
                // 根节点关节点判定
                if (parent == -1 && children > 1) {
                    isArticulation[u] = true;
                    extractBCC(u, v, edgeStack, bcc);
                }
                // 非根节点关节点判定
                if (parent != -1 && low[v] >= disc[u]) {
                    isArticulation[u] = true;
                    extractBCC(u, v, edgeStack, bcc);
                }
            }
            // 回边处理
            else if (v != parent && disc[v] < low[u]) {
                low[u] = disc[v];
                edgeStack.push(std::make_pair(u, v));
            }
        }
    }

    // 提取双连通分量函数（const修饰）
    void extractBCC(int u, int v, std::stack<std::pair<int, int>>& edgeStack,
                    std::vector<std::vector<std::string>>& bcc) const {
        std::set<std::string> component;
        while (true) {
            if (edgeStack.empty()) break;
            std::pair<int, int> topEdge = edgeStack.top();
            edgeStack.pop();
            int a = topEdge.first;
            int b = topEdge.second;
            component.insert(vertices[a]);
            component.insert(vertices[b]);
            if (v != -1 && a == u && b == v) break;
        }
        if (!component.empty()) {
            bcc.push_back(std::vector<std::string>(component.begin(), component.end()));
        }
    }

public:
    // 空图构造函数
    Graph(GraphType t = UNDIRECTED) : type(t), vertexCount(0), edgeCount(0) {}

    // 添加顶点（修复邻接矩阵初始化逻辑）
    int addVertex(const std::string& v) {
        int idx = findVertexIndex(v);
        if (idx != -1) return idx;
        // 新增顶点时同步扩容邻接表和邻接矩阵
        vertices.push_back(v);
        vertexCount++;
        adjList.resize(vertexCount);
        // 重新初始化邻接矩阵，避免内存垃圾值
        adjMatrix.resize(vertexCount);
        for (int i = 0; i < vertexCount; ++i) {
            adjMatrix[i].resize(vertexCount, INF);
            adjMatrix[i][i] = 0; // 自身到自身权重为0
        }
        return vertexCount - 1;
    }

    // 添加边（无向图自动添加反向边）
    bool addEdge(const std::string& from, const std::string& to, int weight = 1) {
        int u = findVertexIndex(from);
        int v = findVertexIndex(to);
        // 检查顶点有效性和边是否已存在
        if (u == -1 || v == -1 || u == v || adjMatrix[u][v] != INF) {
            return false;
        }
        // 更新邻接表
        adjList[u].emplace_back(v, weight);
        if (type == UNDIRECTED) {
            adjList[v].emplace_back(u, weight);
        }
        // 更新邻接矩阵
        adjMatrix[u][v] = weight;
        if (type == UNDIRECTED) {
            adjMatrix[v][u] = weight;
        }
        edgeCount++;
        return true;
    }

    // 输出邻接矩阵（修复输出格式）
    void printAdjMatrix() const {
        std::cout << "=== 邻接矩阵 ===" << std::endl;
        // 打印表头（对齐优化）
        std::cout << "   ";
        for (const auto& v : vertices) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
        // 打印每行数据（格式化输出，避免乱码）
        for (int i = 0; i < vertexCount; ++i) {
            std::cout << vertices[i] << "  ";
            for (int j = 0; j < vertexCount; ++j) {
                if (adjMatrix[i][j] == INF) {
                    std::cout << "∞ ";
                } else {
                    // 占2位对齐，确保矩阵整齐
                    printf("%2d ", adjMatrix[i][j]);
                }
            }
            std::cout << std::endl;
        }
    }

    // BFS遍历（任务2）
    std::vector<std::string> BFS(const std::string& start) const {
        std::vector<std::string> result;
        int startIdx = findVertexIndex(start);
        if (startIdx == -1) return result;
        std::vector<bool> visited(vertexCount, false);
        std::queue<int> q;
        q.push(startIdx);
        visited[startIdx] = true;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            result.push_back(vertices[u]);
            // 邻接顶点排序，确保遍历顺序稳定
            std::vector<int> adjVerts;
            for (const auto& edge : adjList[u]) {
                adjVerts.push_back(edge.to);
            }
            sort(adjVerts.begin(), adjVerts.end());
            for (int vIdx : adjVerts) {
                if (!visited[vIdx]) {
                    visited[vIdx] = true;
                    q.push(vIdx);
                }
            }
        }
        return result;
    }

    // DFS递归遍历（任务2）
    std::vector<std::string> DFS_Recursive(const std::string& start) const {
        std::vector<std::string> result;
        int startIdx = findVertexIndex(start);
        if (startIdx == -1) return result;
        std::vector<bool> visited(vertexCount, false);
        dfsRecursive(startIdx, visited, result);
        return result;
    }

    // Dijkstra最短路径（任务3，修复结构化绑定）
    std::map<std::string, int> Dijkstra(const std::string& start) const {
        std::map<std::string, int> dist;
        int startIdx = findVertexIndex(start);
        if (startIdx == -1) return dist;
        // 初始化距离数组
        std::vector<int> distance(vertexCount, INF);
        distance[startIdx] = 0;
        // 优先队列（小根堆）
        std::priority_queue<std::pair<int, int>, 
                            std::vector<std::pair<int, int>>, 
                            std::greater<std::pair<int, int>>> pq;
        pq.push(std::make_pair(0, startIdx));
        while (!pq.empty()) {
            // 修复：用普通pair访问，替代结构化绑定
            std::pair<int, int> topPair = pq.top();
            pq.pop();
            int currDist = topPair.first;
            int u = topPair.second;
            // 剪枝：已找到更短路径
            if (currDist > distance[u]) continue;
            // 更新邻接顶点距离
            for (const auto& edge : adjList[u]) {
                int v = edge.to;
                int newDist = currDist + edge.w;
                if (newDist < distance[v]) {
                    distance[v] = newDist;
                    pq.push(std::make_pair(newDist, v));
                }
            }
        }
        // 转换为顶点-距离映射
        for (int i = 0; i < vertexCount; ++i) {
            dist[vertices[i]] = (distance[i] == INF) ? -1 : distance[i];
        }
        return dist;
    }

    // Prim最小生成树（任务3，修复结构化绑定）
    std::pair<std::vector<std::tuple<std::string, std::string, int>>, int> Prim() const {
        std::vector<std::tuple<std::string, std::string, int>> mstEdges;
        int totalWeight = 0;
        if (vertexCount <= 1 || type == DIRECTED) {
            // 修复：用make_pair返回，替代结构化绑定初始化
            return std::make_pair(mstEdges, totalWeight);
        }
        std::vector<bool> inMST(vertexCount, false);
        std::vector<int> key(vertexCount, INF);
        std::vector<int> parent(vertexCount, -1);
        // 从A（索引0）开始
        key[0] = 0;
        for (int i = 0; i < vertexCount; ++i) {
            // 找到未加入MST且key最小的顶点
            int u = -1;
            for (int j = 0; j < vertexCount; ++j) {
                if (!inMST[j] && (u == -1 || key[j] < key[u])) {
                    u = j;
                }
            }
            if (u == -1) break; // 图不连通
            inMST[u] = true;
            // 添加边到MST
            if (parent[u] != -1) {
                mstEdges.emplace_back(vertices[parent[u]], vertices[u], key[u]);
                totalWeight += key[u];
            }
            // 更新邻接顶点key值
            for (const auto& edge : adjList[u]) {
                int v = edge.to;
                if (!inMST[v] && edge.w < key[v]) {
                    key[v] = edge.w;
                    parent[v] = u;
                }
            }
        }
        // 修复：用make_pair返回，替代结构化绑定初始化
        return std::make_pair(mstEdges, totalWeight);
    }

    // 查找关节点和双连通分量（任务4，修复结构化绑定）
    std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>> findArticulationAndBCC() const {
        std::vector<std::string> articulationPoints;
        std::vector<std::vector<std::string>> bcc;
        if (vertexCount == 0 || type == DIRECTED) {
            // 修复：用make_pair返回，替代结构化绑定初始化
            return std::make_pair(articulationPoints, bcc);
        }
        std::vector<int> disc(vertexCount, -1);
        std::vector<int> low(vertexCount, -1);
        std::vector<bool> visited(vertexCount, false);
        std::vector<bool> isArticulation(vertexCount, false);
        std::stack<std::pair<int, int>> edgeStack;
        int time = 0;
        // 处理非连通图
        for (int i = 0; i < vertexCount; ++i) {
            if (!visited[i]) {
                tarjan(i, -1, time, disc, low, visited, isArticulation, edgeStack, bcc);
                // 处理栈中剩余边
                if (!edgeStack.empty()) {
                    extractBCC(i, -1, edgeStack, bcc);
                }
            }
        }
        // 收集关节点
        for (int i = 0; i < vertexCount; ++i) {
            if (isArticulation[i]) {
                articulationPoints.push_back(vertices[i]);
            }
        }
        // 修复：用make_pair返回，替代结构化绑定初始化
        return std::make_pair(articulationPoints, bcc);
    }
};

// 辅助打印：遍历序列（修复结构化绑定）
void printTraversal(const std::vector<std::string>& seq, const std::string& name) {
    std::cout << "=== " << name << " 遍历序列 ===" << std::endl;
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << seq[i];
        if (i != seq.size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
}

// 辅助打印：最短路径（修复结构化绑定）
void printShortestPaths(const std::map<std::string, int>& dist, const std::string& start) {
    std::cout << "=== 从 " << start << " 出发的最短路径 ===" << std::endl;
    // 修复：用普通迭代器访问，替代结构化绑定
    for (std::map<std::string, int>::const_iterator it = dist.begin(); it != dist.end(); ++it) {
        const std::string& v = it->first;
        int d = it->second;
        if (d == -1) {
            std::cout << start << " -> " << v << " : 不可达" << std::endl;
        } else {
            std::cout << start << " -> " << v << " : " << d << std::endl;
        }
    }
}

// 辅助打印：最小生成树
void printMST(const std::vector<std::tuple<std::string, std::string, int>>& edges, int totalWeight) {
    std::cout << "=== 最小生成树（MST） ===" << std::endl;
    std::cout << "总权重：" << totalWeight << std::endl;
    std::cout << "边列表：" << std::endl;
    for (const auto& edge : edges) {
        std::cout << std::get<0>(edge) << " - " << std::get<1>(edge) 
                  << " (权重：" << std::get<2>(edge) << ")" << std::endl;
    }
}

// 辅助打印：关节点和双连通分量
void printArticulationAndBCC(const std::vector<std::string>& articulation, const std::vector<std::vector<std::string>>& bcc) {
    std::cout << "=== 关节点（割点） ===" << std::endl;
    for (const auto& v : articulation) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    std::cout << "=== 双连通分量（BCC） ===" << std::endl;
    for (size_t i = 0; i < bcc.size(); ++i) {
        std::cout << "分量 " << i + 1 << " : ";
        for (const auto& v : bcc[i]) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
}

#endif // GRAPH_H
