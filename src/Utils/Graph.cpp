//
// Created by bernardo on 2/2/17.
//

#include "Graph.h"

// A C++ program to print topological sorting of a graph
// using indegrees.
#include<bits/stdc++.h>

using namespace std;

// Class to represent a graph


/**
 * Constructor.
 * @param V Number of water sources.
 */
Graph::Graph(int V) {
    this->V = V;
    adj = new list<int>[V];
}

/**
 * Add connection (stream) between water sources.
 * @param u Upstream source.
 * @param v Downstream source.
 */
void Graph::addEdge(int u, int v) {
    adj[u].push_back(v);
    edges.push_back(vector<int>{u, v});

    /// Sort graph based on the edges added to it so far.
    topologial_order = topologicalSort();

    /// Figure out sources upstream of each vertex added so far.
    upstream_sources = *new vector<vector<int>>(topologial_order.size(), vector<int>());
    for (int& i : topologial_order) {
        upstream_sources[i] = findUpstreamSources(i);
    }
}

/**
 * Returns graph's triangular adjacency matrix multiplier by a constant.
 * @param index multiplying constant.
 * @return adjacency matrix.
 */
vector<vector<int>> Graph::getTriangularAdjacencyMatrix(int multiplier) {
    vector<vector<int>> adj_matrix((unsigned long) V, vector<int>((unsigned long) V));

    for (int u = 0; u < adj->size(); ++u) {
        for (int & v : adj[u]) {
            adj_matrix[u][v] = multiplier;
        }
    }

    return adj_matrix;
}

/**
 * Get all sources upstream water source <id>.
 * @param id
 * @return vector with id's of upstream sources.
 */
vector<int> Graph::findUpstreamSources(int id) const {
    vector<int> upstream_sources;
    for (int i = 0; i < V; ++i) {
        if (std::find(adj[i].begin(), adj[i].end(), id) != adj[i].end())
            upstream_sources.push_back(i);
    }

    return upstream_sources;
}

/**
 * Sort sources from upstream to downsteram.
 * @return
 */
vector<int> Graph::topologicalSort() {
    // Create a vector to store indegrees of all
    // vertices. Initialize all indegrees as 0.
    vector<int> in_degree((unsigned long) V, 0);

    // Traverse adjacency lists to fill indegrees of
    // vertices.  This step takes O(V+E) time
    for (int u = 0; u < V; u++) {
        list<int>::iterator itr;
        for (itr = adj[u].begin(); itr != adj[u].end(); itr++)
            in_degree[*itr]++;
    }

    // Create an queue and enqueue all vertices with
    // indegree 0
    queue<int> q;
    for (int i = 0; i < V; i++)
        if (in_degree[i] == 0)
            q.push(i);

    // Initialize count of visited vertices
    int cnt = 0;

    // Create a vector to store result (A topological
    // ordering of the vertices)
    vector<int> top_order;

    // One by one dequeue vertices from queue and enqueue
    // adjacents if indegree of adjacent becomes 0
    while (!q.empty()) {
        // Extract front of queue (or perform dequeue)
        // and add it to topological order
        int u = q.front();
        q.pop();
        top_order.push_back(u);

        // Iterate through all its neighbouring nodes
        // of dequeued node u and decrease their in-degree
        // by 1
        list<int>::iterator itr;
        for (itr = adj[u].begin(); itr != adj[u].end(); itr++)

            // If in-degree becomes zero, add it to queue
            if (--in_degree[*itr] == 0)
                q.push(*itr);

        cnt++;
    }

    // Check if there was a cycle
    if (cnt != V) {
        cout << "There exists a cycle in the graph\n";
        return top_order;
    }

    // Print topological order
    for (int i = 0; i < top_order.size(); i++)
        cout << top_order[i] << " ";
    cout << endl;

    return top_order;
}

const vector<vector<int>> &Graph::getEdges() const {
    return edges;
}

vector<int> &Graph::getUpstream_sourses(int i) {
    return upstream_sources[i];
}

const vector<int> &Graph::getTopologial_order() const {
    return topologial_order;
}
