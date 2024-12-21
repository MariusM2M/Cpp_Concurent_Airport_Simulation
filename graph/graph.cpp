#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <iterator>
#include <cmath>
#include "graph.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

Edge::Edge() {}

Edge::Edge(int destNodeID, int weight) {
    _destinationNodeID = destNodeID;
     _weight= weight;
}

void Edge::setDestinationNodeID(int destNodeID) {
    _destinationNodeID = destNodeID;
}

void Edge::setWeight(int weight) {
    _weight = weight;
}

int Edge::getDestinationNodeID() {
    return _destinationNodeID;
}
        
int Edge::getWeight() {
    return _weight;
}

void Edge::calculateWeight(int startCoordinateX, int startCoordinateY, int destCoordinateX, int destCoordinateY) {
     _weight = abs(startCoordinateX-destCoordinateX) + abs(startCoordinateY-destCoordinateY);
}


Node::Node() {
    _nodeID = 0;
    _point.x = 0;
    _point.y = 0;
}

Node::Node(int id, int xCoordinate, int yCoordinate, vector<int> neighbors, string type) {
    _nodeID = id;
    _point.x = xCoordinate;
    _point.y = yCoordinate;
    _neighbors = neighbors;
    if (type == "WAYPOINT") {
        _type = Type::WAYPOINT;
    }else if (type == "LANDINGPOINT_A320"){
        _type = Type::LANDINGPOINT_A320;
    }else if (type == "LANDINGPOINT_A380"){
        _type = Type::LANDINGPOINT_A380;
    }else if (type == "LANDINGPOINT_OTHERS"){
        _type = Type::LANDINGPOINT_OTHERS;
    } else if (type == "TERMINAL"){
        _type = Type::TERMINAL;
    } else if (type == "WAITINGPOINT"){
        _type = Type::WAITINGPOINT;
    } else if (type == "ENDPOINT"){
        _type = Type::ENDPOINT;
    }
}

void Node::setNodeID(int id) {
    _nodeID = id;
}

void Node::setNodeCoordinates(int xCoordinate, int yCoordinate) {
    _point.x = xCoordinate;
    _point.y = yCoordinate;
}

void Node::closeNode() {
    State _state = State::kCLOSED;
}

void Node::setNeighbors(vector<int> neighbors) {
    _neighbors = neighbors;
}

int Node::getNodeID() {
    return _nodeID;
}

Node::Point Node::getNodeCoordinates() {
    return _point;
}

vector<int> Node::getNeighbors() {
    return _neighbors;
}

list<Edge> Node::getEdgeList() {
    return _edgeList;
}

Node::State Node::getState() {
 return _state;
}

Node::Type Node::getType(){
    return _type;
};

void Node::addEdgeToEdgeList(Edge edge) {
    _edgeList.push_back(edge);
}


NavigationMesh::NavigationMesh() {
    // 1. open and parse json file 
    parseJson();
    // 2. calculate all edges of a node
    calculateEdges();
}

int NavigationMesh::getSize() {
    return _navigationMesh.size();
}

vector<Node> NavigationMesh::getNavMesh() {
    return _navigationMesh;
}

void NavigationMesh::parseJson() {
    // 1. open and parse json file 
  ifstream jsonfile("../resources/graph.json");
  json data = json::parse(jsonfile);
    
    // 2. get all defined nodes from graph.json and add each node to the navigationGraph
    for (int i=0; i < data.size(); i++) {
        vector<int> neighbors{};
        auto size = data[i]["neighbors"].size();
        for(int j=0; j < data[i]["neighbors"].size(); j++) {
            if(data[i]["neighbors"][j] == false) {
                break;
            }
            neighbors.push_back(data[i]["neighbors"][j]);
        }
        Node node(data[i]["id"], data[i]["x"], data[i]["y"], neighbors, data[i]["type"]);
        _navigationMesh.push_back(node);
    }
}

void NavigationMesh::calculateEdges() {
    for (int i=0; i < _navigationMesh.size(); i++) {
            vector<int> neighbors = _navigationMesh[i].getNeighbors();
            if (neighbors.size() == 0) {
                // node has no neighbours.
                continue;
            }
            Node::Point initialPoint = _navigationMesh[i].getNodeCoordinates();
            // Calculate weight of the edges
            for (int j=0; j < neighbors.size(); j++) {
                Edge edge;
                Node::Point neighborCoordinates = _navigationMesh[neighbors[j]-1].getNodeCoordinates();
                edge.calculateWeight(initialPoint.x, initialPoint.y, neighborCoordinates.x, neighborCoordinates.y);
                edge.setDestinationNodeID(neighbors[j]);
                _navigationMesh[i].addEdgeToEdgeList(edge);
            }
    }
}