//
// Created by bernardoct on 4/8/17.
//
// topologicalSort function written by Chirag Agarwal, found at:
// http://www.geeksforgeeks.org/topological-sorting-indegree-based-solution/
//

#include <iostream>
#include <queue>
#include <algorithm>
#include "WaterSourcesGraph.h"

WaterSourceGraph::WaterSourceGraph(int V) : Graph(V) {

}

/**
 * Add connection (stream) between water sources.
 * @param u Upstream source.
 * @param v Downstream source.
 */
void WaterSourceGraph::addEdge(int u, int v) {
    this->addEdgeToEdgesList(u, v);

    /// Sort graph based on the edges added to it so far.
    topological_order = topologicalSort();

    /// Figure out sources upstream of each vertex added so far.
    upstream_sources = *new vector<vector<int>>(topological_order.size(), vector<int>());
    for (int& i : topological_order) {
        upstream_sources[i] = findUpstreamSources(i);
    }

    vector<double> pipe_connectivity(V, 0);
    pipe_connectivity[u] = -1;
    pipe_connectivity[v] = 1;

    continuity_matrix_transpose.push_back(pipe_connectivity);
}

/**
 * Sort sources from upstream to downstream.
 * @return
 */
vector<int> WaterSourceGraph::topologicalSort() {
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
        cout << "There exists a cycle in the graph, and topological sort cannot handle that.\n";
        return top_order;
    }

    // Print topological order
//    for (int i = 0; i < top_order.size(); i++)
//        cout << top_order[i] << " ";
//    cout << endl;

    return top_order;
}

/**
 * Get all sources upstream water source <id>.
 * @param id
 * @return vector with id's of upstream sources.
 */
vector<int> WaterSourceGraph::findUpstreamSources(int id) const {
    vector<int> upstream_sources;
    for (int i = 0; i < V; ++i) {
        /// If find function does not reach the end of the list while searching
        /// for ID is because element ID exists in list.
        if (std::find(adj[i].begin(), adj[i].end(), id) != adj[i].end())
            upstream_sources.push_back(i);
    }

    return upstream_sources;
}

vector<int> &WaterSourceGraph::getUpstream_sources(int i) {
    return upstream_sources[i];
}

const vector<int> &WaterSourceGraph::getTopological_order() const {
    return topological_order;
}

/**
 * Returns a matrix in which rows correspond to vertexes. The first set of columns correspond to pipes and
 * the second set to flows in and out of vertexes.
 * @return Continuity Matrix
 */
const vector<vector<double>> WaterSourceGraph::getContinuityMatrix() const {
    vector<vector<double>> continuity_matrix(continuity_matrix_transpose[0].size(),
                                             *new vector<double>(continuity_matrix_transpose.size(), 0));

    unsigned long n_pipes = continuity_matrix_transpose.size();

    for (int i = 0; i < continuity_matrix.size(); ++i) {
        for (int j = 0; j < continuity_matrix[0].size(); ++j) {
            continuity_matrix[i][j] = continuity_matrix_transpose[j][i];
        }
    }

    for (int i = 0; i < continuity_matrix.size(); ++i) {
        for (int ii = 0; ii < continuity_matrix.size(); ++ii) {
            continuity_matrix[i].push_back(0);
        }
        continuity_matrix[i][n_pipes + i] = -1;
    }

    return continuity_matrix;
}