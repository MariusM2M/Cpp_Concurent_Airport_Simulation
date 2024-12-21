#include "astar.hpp"
#include "graph.hpp"
#include "exception.hpp"

vector<Node> AStar::search(vector<Node> navigationMesh, Node::Point start, Node::Point destination){

    // Initialize openList
    vector<vector<int>> openList {};
    // Initialize path from start to destination
    vector<Node> path{};

    // initialize the starting point
    int x =  start.x;
    int y = start.y;
    int g = 0;
    int h = calcHeuristic(start, destination);
    int nodeID = getNodeID(start, navigationMesh);
    addToOpenList(nodeID, start, g, h, openList, navigationMesh);

    while(openList.size() > 0) {
        CellSort(&openList);
        auto current = openList.back();
        Node::Point pathPoint;
        // delete last element of the openList
        openList.pop_back();
        pathPoint.x = current[1];
        pathPoint.y = current[2];

        // get the corresponding node and push the node to the vector
        // int nodeID = getNodeID(pathPoint, navigationMesh);
        Node node = getNode(current[0], navigationMesh);
        path.push_back(node);

        // check if we're done
        if(pathPoint.x == destination.x && pathPoint.y == destination.y){
            return path;
        }

        // If we are not done, expand search to current node's neighbors.
        ExpandNeighbors(current, destination, openList, navigationMesh);
    }

    cout << " No Path found" << "\n";
    return vector<Node>{};
}

int AStar::calcHeuristic(Node::Point point1, Node::Point point2){
    return abs(point2.x - point1.x) + abs(point2.y - point1.y);
}

void AStar::addToOpenList(int nodeID, Node::Point point, int gValue, int hValue, vector<vector<int>> &openList, vector<Node> &navigationMesh){
    // add to open List
    openList.push_back(vector<int>{nodeID, point.x, point.y, gValue, hValue});
    // set node to CLOSED
    navigationMesh[nodeID].closeNode();
}

int AStar::getNodeID(Node::Point point, vector<Node> &navigationMesh){
    for(int i = 0; i < navigationMesh.size(); i++) { 
        if (navigationMesh[i].getNodeCoordinates().x == point.x && navigationMesh[i].getNodeCoordinates().y == point.y) {
            return navigationMesh[i].getNodeID();
        }
    }
    throw Exception ("getNodeID() could not identify a node");
}

Node AStar::getNode(int nodeID, vector<Node> &navigationMesh){
    return navigationMesh[nodeID-1];
}

bool AStar::Compare(const vector<int> a, const vector<int> b) {
  int f1 = a[3] + a[4]; // f1 = g1 + h1
  int f2 = b[3] + b[4]; // f2 = g2 + h2
  return f1 > f2; 
}

void AStar::CellSort(vector<vector<int>> *v) {
  sort(v->begin(), v->end(), Compare);
}

void AStar::ExpandNeighbors(vector<int> &current, Node::Point destination, vector<vector<int>> &openList, vector<Node> &navigationMesh){

    vector<int> neighbors = navigationMesh[current[0]-1].getNeighbors();

    int size = neighbors.size();
    for(int i = 0; i < neighbors.size(); i++){
        if(CheckValidNode(navigationMesh[neighbors[i]-1])){
            // calculate g-value: weight of Edge from current node to the neighbor
            int gValue = CalcGValue(navigationMesh[current[0]-1], neighbors[i]);
            // calculate h-value: distance of neighbor to destination
            Node::Point neighbor = navigationMesh[neighbors[i]-1].getNodeCoordinates();
            int hValue = calcHeuristic(neighbor, destination);
            // add neighbor to openList
            addToOpenList(neighbors[i], neighbor, gValue, hValue, openList, navigationMesh);
        }
    }
}

bool AStar::CheckValidNode(Node node){
    if(node.getState() == Node::State::kOPEN){
        return true;
    }
    return false;
}

int AStar::CalcGValue(Node node, int neighbor){
    list<Edge> edges = node.getEdgeList();
    list<Edge>::iterator it;
    for( it = edges.begin(); it != edges.end(); ++it){
        if(it->getDestinationNodeID() == neighbor){
            return it->getWeight();
        }
    }
    return 0;
}