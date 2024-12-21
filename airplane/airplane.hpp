# ifndef _AIRPLANE_
# define _AIRPLANE_

#include <vector>
#include <memory>
#include <thread>
#include "graph.hpp"

// forward declaration
class Airport;
class Terminal;

class Airplane : public std::enable_shared_from_this<Airplane> {

    public:
        // public member variable
        enum Status {LANDING, LANDED, LEFTRUNWAY, OFFBOARDING, DEPARTURED, HASLEFT};
        enum Type {A320, A380, Boing777, Boing747};

    // private member variables
    private:
        Type _type;
        // every Airplane needs its own navigationMesh
        vector<Node> _navigationMesh;
        // individual path from start to destination
        vector<Node> _path;
        // shrd pointer to airport
        shared_ptr<Airport> _airport;
        // current position
        Node::Point _currentPosition;
        // position on current path segment
        double _posPathSegment;
        // current destination of the airplane:
        Node::Point _destination;
        // status of the airplane
        Status _status = Status::LANDING;
        // selected terminal for offboarding
        shared_ptr<Terminal> _terminal; 
        // number of passengers
        int _passenger;
        // path angle of the airplane in degree
        double _pathAngle;

    public:

        // public member functions
        Airplane(vector<Node> navigationMesh, shared_ptr<Airport> airport);
        ~Airplane();
        vector<Node> getPath();
        int getPathSize();
        double getPathAngle();
        void setNumberPassenger();
        Node::Point getCurrentPosition();
        Node::Point getDestination();
        double getLengthOfPathSegment(Node::Point p1, Node::Point p2);
        shared_ptr<Terminal> getFreeTerminal();
        Node::Point getPositionOfEndPoint();
        Node::Point getNodeCoordinates(int pointID);
        void calculatePathAngle(Node::Point p1, Node::Point p2);
        void chooseRandomAirplaneType();
        void setLandingPoint();
        Status getStatus();
        void simulation();
        void move();    //moves the airplane 
    
    protected:
        vector<thread> _threads; // holds all threads that have been launched within the airport object
};

# endif // _AIRPLANE_