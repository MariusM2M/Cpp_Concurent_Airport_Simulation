#ifndef _GRAPH_
#define _GRAPH_

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <iterator>
#include <cmath>

using namespace std;

class Edge
{

    private:
        int _destinationNodeID;
        int _weight;
    
    public:
        Edge(int destNodeID, int weight):_destinationNodeID(destNodeID), _weight(weight) {};
        void setDestinationNodeID(int destNodeID);
        void setWeight(int weight);
        int getDestinationNodeID();
        int getWeight();
        void calculateWeight(int startCoordinateX, int startCoordinateY, int destCoordinateX, int destCoordinateY);

};

class Node
{
    public:
        enum State {kOPEN, kCLOSED};
        enum Type {WAYPOINT, LANDINGPOINT_A320, LANDINGPOINT_A380, LANDINGPOINT_OTHERS, TERMINAL, WAITINGPOINT, ENDPOINT};
        struct Point {
            int x;
            int y;
        };

    private:
        int _nodeID;
        Point _point;
        vector<int> _neighbors;
        list<Edge> _edgeList;
        State _state = State::kOPEN;
        Type _type;

    public:
        Node();
        Node(int id, int xCoordinate, int yCoordinate, vector<int> neighbors, string type);
        void setNodeID( int id);
        void setNodeCoordinates(int x, int y);
        void setNeighbors(vector<int> neighbor);
        void closeNode();
        int getNodeID();
        Point getNodeCoordinates();
        vector<int> getNeighbors();
        list<Edge> getEdgeList();
        State getState();
        Type getType();
        void addEdgeToEdgeList(Edge edge);

};

class NavigationMesh
{
    private:
        vector<Node> _navigationMesh;

    public:
        NavigationMesh();
        int getSize();
        vector<Node> getNavMesh();
        void parseJson();
        void calculateEdges();

};

#endif // _GRAPH_