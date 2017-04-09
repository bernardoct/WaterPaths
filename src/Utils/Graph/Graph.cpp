//
// Created by bernardo on 2/2/17.
//

#include "Graph.h"

// A C++ program to print topological sorting of a graph
// using indegrees.
#include<bits/stdc++.h>
#include "../Constants.h"

using namespace std;

// Class to represent a graph


/**
 * Constructor.
 * @param V Number of water sources.
 */
Graph::Graph(int V) : V(V) {
    adj = new list<int>[V];
}

/**
 * Add connection (stream) between water sources.
 * @param u Upstream source.
 * @param v Downstream source.
 */
void Graph::addEdgeToEdgesList(int u, int v) {
    adj[u].push_back(v);
    n_edges++;
}
