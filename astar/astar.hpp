#ifndef _ASTAR_
#define _ASTAR_

#include "graph.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <iterator>
#include <cmath>
#include <algorithm>

class AStar{

    public:
        vector<Node> static search(vector<Node> navigationMesh, Node::Point start, Node::Point destination);
        int static getNodeID(Node::Point point, vector<Node> &navigationMesh);
        Node static getNode(int nodeID, vector<Node> &navigationMesh);
        void static ExpandNeighbors(vector<int> &current, Node::Point destination, vector<vector<int>> &openList, vector<Node> &_graph);
        int static calcHeuristic(Node::Point point1, Node::Point point2);
        void static addToOpenList(int nodeID, Node::Point point, int gValue, int hValue, vector<vector<int>> &openList, vector<Node> &_graph);
        void static CellSort(vector<vector<int>> *v);
        bool static Compare(const vector<int> a, const vector<int> b);
        bool static CheckValidNode(const Node node);
        int static CalcGValue(Node node, int neighbor);

};

#endif // _ASTAR_