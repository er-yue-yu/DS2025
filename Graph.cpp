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
#include <functional>

// 图的类型枚举
enum GraphType {
    UNDIRECTED,  // 无向图
    DIRECTED     // 有向图
};

// 边结构体（存储顶点索引和权重）
template <typename WeightType = int>
struct Edge {
    int to;         // 目标顶点索引（从0开始）
    WeightType w;   // 边权重（默认int类型）
    Edge(int t = -1, WeightType weight = 1) : to(t), w(weight) {}
};

// 图类（模板类，支持自定义顶点数据类型和边权类型）
template <typename VertexType = std::string, typename WeightType = int>
class Graph {
private:
    GraphType type;                          // 图的类型（有向/无向）
    std::vector<VertexType> vertices;        // 顶点集合（索引对应顶点编号）
    std::vector<std::vector<Edge<WeightType>>> adjList;  // 邻接表（核心存储）
    std::vector<std::vector<WeightType>> adjMatrix;       // 邻接矩阵（辅助存储）
    int vertexCount;                         // 顶点数
    int edgeCount;                           // 边数
    const WeightType INF;                    // 无穷大（避免溢出）

    // -------------------------- 私有辅助函数 --------------------------
    // 查找顶点对应的索引（不存在返回-1）
    int findVertexIndex(const VertexType& v) const {
        auto it = std::find(vertices.begin(), vertices.end(), v);
        return it != vertices.end() ? static_cast<int>(it - vertices.begin()) : -1;
    }

    // DFS递归辅助（遍历）
    void dfsRecursive(int startIdx, std::vector<bool>& visited, std::vector<VertexType>& result) const {
        visited[startIdx] = true;
        result.push_back(vertices[startIdx]);
        for (const auto& edge : adjList[startIdx]) {
            if (!visited[edge.to]) {
                dfsRecursive(edge.to, visited, result);
            }
        }
    }

    // Tarjan算法辅助（找关节点和双连通分量）
    void tarjan(int u, int parent, int& time, std::vector<int>& disc, std::vector<int>& low,
                std::vector<bool>& visited, std::vector<bool>& isArticulation,
                std::stack<std::pair<int, int>>& edgeStack, std::vector<std::vector<VertexType>>& bcc) {
        int children = 0;
        disc[u] = low[u] = ++time;
        visited[u] = true;

        for (const auto& edge : adjList[u]) {
            int v = edge.to;
            if (!visited[v]) {
                children++;
                edgeStack.push(std::make_pair(u, v));
                tarjan(v, u, time, disc, low, visited, isArticulation, edgeStack, bcc);

                // 更新low[u]
                low[u] = std::min(low[u], low[v]);

                // 情况1：根节点且子节点数>=2
                if (parent == -1 && children > 1) {
                    isArticulation[u] = true;
                    extractBCC(u, v, edgeStack, bcc);
                }

                // 情况2：非根节点，low[v] >= disc[u]
                if (parent != -1 && low[v] >= disc[u]) {
                    isArticulation[u] = true;
                    extractBCC(u, v, edgeStack, bcc);
                }
            }
            // 回边（避免重复处理父节点）
            else if (v != parent && disc[v] < low[u]) {
                low[u] = disc[v];
                edgeStack.push(std::make_pair(u, v));
            }
        }
    }

    // 提取双连通分量（辅助Tarjan算法）
    void extractBCC(int u, int v, std::stack<std::pair<int, int>>& edgeStack,
                    std::vector<std::vector<VertexType>>& bcc) {
        std::set<VertexType> component;
        while (true) {
            std::pair<int, int> topEdge = edgeStack.top();
            edgeStack.pop();
            int a = topEdge.first;
            int b = topEdge.second;
            component.insert(vertices[a]);
            component.insert(vertices[b]);
            if (a == u && b == v) break;
        }
        bcc.push_back(std::vector<VertexType>(component.begin(), component.end()));
    }

    // Kruskal算法：边排序辅助
    static bool compareEdges(const std::tuple<int, int, WeightType>& a,
                             const std::tuple<int, int, WeightType>& b) {
        return std::get<2>(a) < std::get<2>(b);
    }

    // 并查集查找（路径压缩）
    int findUnion(int x, std::vector<int>& parent) {
        if (parent[x] != x) parent[x] = findUnion(parent[x], parent);
        return parent[x];
    }

    // 并查集合并（按秩合并）
    void unionSet(int x, int y, std::vector<int>& parent, std::vector<int>& rank) {
        int xRoot = findUnion(x, parent);
        int yRoot = findUnion(y, parent);
        if (xRoot == yRoot) return;
        if (rank[xRoot] < rank[yRoot]) {
            parent[xRoot] = yRoot;
        } else {
            parent[yRoot] = xRoot;
            if (rank[xRoot] == rank[yRoot]) rank[xRoot]++;
        }
    }

public:
    // -------------------------- 构造函数 --------------------------
    // 空图构造
    Graph(GraphType t = UNDIRECTED) : type(t), vertexCount(0), edgeCount(0), INF(static_cast<WeightType>(INT_MAX / 2)) {}

    // 已知顶点集合构造
    Graph(const std::vector<VertexType>& vs, GraphType t = UNDIRECTED)
        : type(t), vertices(vs), vertexCount(static_cast<int>(vs.size())), edgeCount(0), INF(static_cast<WeightType>(INT_MAX / 2)) {
        adjList.resize(vertexCount);
        // 初始化邻接矩阵（无穷大表示无边）
        adjMatrix.resize(vertexCount, std::vector<WeightType>(vertexCount, INF));
        for (int i = 0; i < vertexCount; i++) adjMatrix[i][i] = 0;  // 自身到自身权重为0
    }

    // -------------------------- 基础操作：添加/删除顶点/边 --------------------------
    // 添加顶点（返回顶点索引，已存在返回原索引）
    int addVertex(const VertexType& v) {
        int idx = findVertexIndex(v);
        if (idx != -1) return idx;
        vertices.push_back(v);
        vertexCount++;
        adjList.resize(vertexCount);
        // 更新邻接矩阵
        adjMatrix.resize(vertexCount, std::vector<WeightType>(vertexCount, INF));
        for (int i = 0; i < vertexCount; i++) {
            adjMatrix[i][vertexCount - 1] = INF;
            adjMatrix[vertexCount - 1][i] = INF;
            adjMatrix[i][i] = 0;
        }
        return vertexCount - 1;
    }

    // 添加边（from -> to，无向图自动添加反向边）
    bool addEdge(const VertexType& from, const VertexType& to, WeightType weight = 1) {
        int u = findVertexIndex(from);
        int v = findVertexIndex(to);
        if (u == -1 || v == -1) return false;  // 顶点不存在
        if (adjMatrix[u][v] != INF) return false;  // 边已存在

        // 更新邻接表
        adjList[u].emplace_back(v, weight);
        if (type == UNDIRECTED) adjList[v].emplace_back(u, weight);

        // 更新邻接矩阵
        adjMatrix[u][v] = weight;
        if (type == UNDIRECTED) adjMatrix[v][u] = weight;

        edgeCount++;
        return true;
    }

    // 删除边
    bool removeEdge(const VertexType& from, const VertexType& to) {
        int u = findVertexIndex(from);
        int v = findVertexIndex(to);
        if (u == -1 || v == -1 || adjMatrix[u][v] == INF) return false;

        // 更新邻接表
        adjList[u].erase(std::remove_if(adjList[u].begin(), adjList[u].end(),
                                        [v](const Edge<WeightType>& e) { return e.to == v; }),
                        adjList[u].end());
        if (type == UNDIRECTED) {
            adjList[v].erase(std::remove_if(adjList[v].begin(), adjList[v].end(),
                                            [u](const Edge<WeightType>& e) { return e.to == u; }),
                            adjList[v].end());
        }

        // 更新邻接矩阵
        adjMatrix[u][v] = INF;
        if (type == UNDIRECTED) adjMatrix[v][u] = INF;

        edgeCount--;
        return true;
    }

    // -------------------------- 图的存储表示：输出邻接矩阵/邻接表 --------------------------
    // 输出邻接矩阵
    void printAdjMatrix() const {
        std::cout << "=== 邻接矩阵 ===" << std::endl;
        std::cout << "   ";
        for (const auto& v : vertices) std::cout << v << " ";
        std::cout << std::endl;
        for (int i = 0; i < vertexCount; i++) {
            std::cout << vertices[i] << "  ";
            for (int j = 0; j < vertexCount; j++) {
                if (adjMatrix[i][j] == INF) std::cout << "∞ ";
                else std::cout << adjMatrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    // 输出邻接表
    void printAdjList() const {
        std::cout << "=== 邻接表 ===" << std::endl;
        for (int i = 0; i < vertexCount; i++) {
            std::cout << vertices[i] << " -> ";
            for (const auto& edge : adjList[i]) {
                std::cout << "(" << vertices[edge.to] << ", " << edge.w << ") ";
            }
            std::cout << std::endl;
        }
    }

    // -------------------------- 图的遍历：BFS/DFS --------------------------
    // BFS遍历（从指定顶点出发，返回遍历序列）
    std::vector<VertexType> BFS(const VertexType& start) const {
        std::vector<VertexType> result;
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

            for (const auto& edge : adjList[u]) {
                if (!visited[edge.to]) {
                    visited[edge.to] = true;
                    q.push(edge.to);
                }
            }
        }
        return result;
    }

    // DFS遍历（递归版，返回遍历序列）
    std::vector<VertexType> DFS_Recursive(const VertexType& start) const {
        std::vector<VertexType> result;
        int startIdx = findVertexIndex(start);
        if (startIdx == -1) return result;

        std::vector<bool> visited(vertexCount, false);
        dfsRecursive(startIdx, visited, result);
        return result;
    }

    // DFS遍历（迭代版，返回遍历序列）
    std::vector<VertexType> DFS_Iterative(const VertexType& start) const {
        std::vector<VertexType> result;
        int startIdx = findVertexIndex(start);
        if (startIdx == -1) return result;

        std::vector<bool> visited(vertexCount, false);
        std::stack<int> st;
        st.push(startIdx);
        visited[startIdx] = true;

        while (!st.empty()) {
            int u = st.top();
            st.pop();
            result.push_back(vertices[u]);

            // 逆序入栈，保证遍历顺序与递归版一致
            for (auto it = adjList[u].rbegin(); it != adjList[u].rend(); ++it) {
                if (!visited[it->to]) {
                    visited[it->to] = true;
                    st.push(it->to);
                }
            }
        }
        return result;
    }

    // -------------------------- 最短路径算法 --------------------------
    // Dijkstra算法（单源最短路径，无负权边）
    std::map<VertexType, WeightType> Dijkstra(const VertexType& start) const {
        std::map<VertexType, WeightType> dist;
        int startIdx = findVertexIndex(start);
        if (startIdx == -1) return dist;

        // 初始化距离
        std::vector<WeightType> distance(vertexCount, INF);
        distance[startIdx] = 0;
        // 优先队列（小根堆，存储{当前距离, 顶点索引}）
        std::priority_queue<std::pair<WeightType, int>,
                            std::vector<std::pair<WeightType, int>>,
                            std::greater<std::pair<WeightType, int>>> pq;
        pq.push(std::make_pair(0, startIdx));

        while (!pq.empty()) {
            std::pair<WeightType, int> topPair = pq.top();
            pq.pop();
            WeightType currDist = topPair.first;
            int u = topPair.second;

            if (currDist > distance[u]) continue;  // 已找到更短路径，跳过

            for (const auto& edge : adjList[u]) {
                int v = edge.to;
                WeightType newDist = currDist + edge.w;
                if (newDist < distance[v]) {
                    distance[v] = newDist;
                    pq.push(std::make_pair(newDist, v));
                }
            }
        }

        // 转换为顶点-距离映射（-1表示不可达）
        for (int i = 0; i < vertexCount; ++i) {
            dist[vertices[i]] = (distance[i] == INF) ? static_cast<WeightType>(-1) : distance[i];
        }
        return dist;
    }

    // Floyd-Warshall算法（多源最短路径，支持负权边，无负环）
    std::map<VertexType, std::map<VertexType, WeightType>> Floyd() const {
        std::map<VertexType, std::map<VertexType, WeightType>> distMap;
        // 初始化距离矩阵为邻接矩阵的副本
        std::vector<std::vector<WeightType>> dist = adjMatrix;

        // 动态规划更新最短路径
        for (int k = 0; k < vertexCount; ++k) {
            for (int i = 0; i < vertexCount; ++i) {
                for (int j = 0; j < vertexCount; ++j) {
                    if (dist[i][k] != INF && dist[k][j] != INF) {
                        dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
                    }
                }
            }
        }

        // 转换为双层映射（-1表示不可达）
        for (int i = 0; i < vertexCount; ++i) {
            for (int j = 0; j < vertexCount; ++j) {
                distMap[vertices[i]][vertices[j]] = (dist[i][j] == INF) ? static_cast<WeightType>(-1) : dist[i][j];
            }
        }
        return distMap;
    }

    // -------------------------- 最小生成树（MST）算法 --------------------------
    // Prim算法（适合稠密图，返回MST的边集合和总权重）
    std::pair<std::vector<std::tuple<VertexType, VertexType, WeightType>>, WeightType> Prim() const {
        std::vector<std::tuple<VertexType, VertexType, WeightType>> mstEdges;
        WeightType totalWeight = 0;
        if (vertexCount <= 1 || type == DIRECTED) return std::make_pair(mstEdges, totalWeight);  // MST仅适用于无向图

        std::vector<bool> inMST(vertexCount, false);
        std::vector<WeightType> key(vertexCount, INF);
        std::vector<int> parent(vertexCount, -1);

        // 从第一个顶点开始（可修改为指定起点）
        key[0] = 0;
        for (int i = 0; i < vertexCount; ++i) {
            // 找到未加入MST且key值最小的顶点
            int u = -1;
            for (int j = 0; j < vertexCount; ++j) {
                if (!inMST[j] && (u == -1 || key[j] < key[u])) {
                    u = j;
                }
            }

            if (u == -1) break;  // 图不连通，提前退出
            inMST[u] = true;
            if (parent[u] != -1) {  // 非起点，添加边到MST
                mstEdges.emplace_back(vertices[parent[u]], vertices[u], key[u]);
                totalWeight += key[u];
            }

            // 更新邻接顶点的key值
            for (const auto& edge : adjList[u]) {
                int v = edge.to;
                if (!inMST[v] && edge.w < key[v]) {
                    key[v] = edge.w;
                    parent[v] = u;
                }
            }
        }

        return std::make_pair(mstEdges, totalWeight);
    }

    // Kruskal算法（适合稀疏图，返回MST的边集合和总权重）
    std::pair<std::vector<std::tuple<VertexType, VertexType, WeightType>>, WeightType> Kruskal() const {
        std::vector<std::tuple<VertexType, VertexType, WeightType>> mstEdges;
        WeightType totalWeight = 0;
        if (vertexCount <= 1 || type == DIRECTED) return std::make_pair(mstEdges, totalWeight);  // MST仅适用于无向图

        // 收集所有边（去重，无向图只存一次）
        std::vector<std::tuple<int, int, WeightType>> edges;
        for (int u = 0; u < vertexCount; ++u) {
            for (const auto& edge : adjList[u]) {
                int v = edge.to;
                if (u < v) {  // 避免重复添加无向边
                    edges.emplace_back(u, v, edge.w);
                }
            }
        }

        // 按权重升序排序
        std::sort(edges.begin(), edges.end(), compareEdges);

        // 初始化并查集
        std::vector<int> parent(vertexCount);
        std::vector<int> rank(vertexCount, 0);
        for (int i = 0; i < vertexCount; ++i) parent[i] = i;

        // 依次选边，避免环
        for (const auto& edge : edges) {
            int u = std::get<0>(edge);
            int v = std::get<1>(edge);
            WeightType w = std::get<2>(edge);

            if (findUnion(u, parent) != findUnion(v, parent)) {
                unionSet(u, v, parent, rank);
                mstEdges.emplace_back(vertices[u], vertices[v], w);
                totalWeight += w;
                if (mstEdges.size() == static_cast<size_t>(vertexCount - 1)) break;  // MST已生成，提前退出
            }
        }

        return std::make_pair(mstEdges, totalWeight);
    }

    // -------------------------- 连通性分析：关节点/双连通分量 --------------------------
    // 查找关节点（割点）和双连通分量
    std::pair<std::vector<VertexType>, std::vector<std::vector<VertexType>>> findArticulationAndBCC() const {
        std::vector<VertexType> articulationPoints;  // 关节点集合
        std::vector<std::vector<VertexType>> bcc;     // 双连通分量集合
        if (vertexCount == 0 || type == DIRECTED) return std::make_pair(articulationPoints, bcc);  // 仅支持无向图

        std::vector<int> disc(vertexCount, -1);       // 发现时间
        std::vector<int> low(vertexCount, -1);        // 最低发现时间
        std::vector<bool> visited(vertexCount, false);
        std::vector<bool> isArticulation(vertexCount, false);
        std::stack<std::pair<int, int>> edgeStack;
        int time = 0;

        // 处理非连通图
        for (int i = 0; i < vertexCount; ++i) {
            if (!visited[i]) {
                tarjan(i, -1, time, disc, low, visited, isArticulation, edgeStack, bcc);
                // 处理栈中剩余的边（根节点的双连通分量）
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

        return std::make_pair(articulationPoints, bcc);
    }

    // -------------------------- 工具函数 --------------------------
    // 获取顶点数
    int getVertexCount() const { return vertexCount; }

    // 获取边数
    int getEdgeCount() const { return edgeCount; }

    // 清空图
    void clear() {
        vertices.clear();
        adjList.clear();
        adjMatrix.clear();
        vertexCount = 0;
        edgeCount = 0;
    }
};

// 辅助打印函数：输出遍历序列
template <typename T>
void printTraversal(const std::vector<T>& seq, const std::string& name) {
    std::cout << "=== " << name << " 遍历序列 ===" << std::endl;
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << seq[i];
        if (i != seq.size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
}

// 辅助打印函数：输出最短路径（Dijkstra结果）
template <typename V, typename W>
void printShortestPaths(const std::map<V, W>& dist, const V& start) {
    std::cout << "=== 从 " << start << " 出发的最短路径 ===" << std::endl;
    for (const auto& pair : dist) {
        const V& v = pair.first;
        const W& d = pair.second;
        if (d == static_cast<W>(-1)) {
            std::cout << start << " -> " << v << " : 不可达" << std::endl;
        } else {
            std::cout << start << " -> " << v << " : " << d << std::endl;
        }
    }
}

// 辅助打印函数：输出MST结果
template <typename V, typename W>
void printMST(const std::vector<std::tuple<V, V, W>>& edges, W totalWeight) {
    std::cout << "=== 最小生成树（MST） ===" << std::endl;
    std::cout << "总权重：" << totalWeight << std::endl;
    std::cout << "边列表：" << std::endl;
    for (const auto& edge : edges) {
        const V& u = std::get<0>(edge);
        const V& v = std::get<1>(edge);
        const W& w = std::get<2>(edge);
        std::cout << u << " - " << v << " (权重：" << w << ")" << std::endl;
    }
}

// 辅助打印函数：输出关节点和双连通分量
template <typename V>
void printArticulationAndBCC(const std::vector<V>& articulation, const std::vector<std::vector<V>>& bcc) {
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
