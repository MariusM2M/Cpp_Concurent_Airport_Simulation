#include <thread>
#include <future>
#include <chrono>
#include <mutex>

#include "airplane.hpp"
#include "astar.hpp"
#include "airport.hpp"
#include "exception.hpp"

#define PI 3.141592654

Airplane::Airplane(vector<Node> navigationMesh, shared_ptr<Airport> airport) {
    // initialize start position which is always node1 (beginning of the runway) 
    Node::Point start;
    start.x = 29;
    start.y = 324;
    // set start position to current position of the airplane
    _currentPosition = start;
    
    // each Airplane needs its own navigationMesh
    _navigationMesh = navigationMesh;

    // set pointer to airport:
    _airport = airport;

    // set airplane type
    chooseRandomAirplaneType();
    // set number of passengers (depends on airplane type)
    setNumberPassenger();
    
    // set landing point depending on airplane Type: A380, A320, ...
    setLandingPoint();

    // call AStar::search to calculate the path to destination
    _path = AStar::search(_navigationMesh, start, _destination);
};

vector<Node> Airplane::getPath(){
    return _path;
};

int Airplane::getPathSize(){
    return _path.size();
};

double Airplane::getPathAngle(){
    return _pathAngle;
}

Node::Point Airplane::getCurrentPosition(){
    return _currentPosition;
};

Node::Point Airplane::getDestination(){
    return _destination;
};

double Airplane::getLengthOfPathSegment(Node::Point p1, Node::Point p2){
    return  sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
};

void Airplane::calculatePathAngle(Node::Point p1, Node::Point p2){

    double hy = sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    double gk = p1.y - p2.y;

    _pathAngle = abs(asin(gk/hy)*180/PI);

    if (p1.x >= p2.x && p1.y >= p2.y) {
        _pathAngle = 180 - _pathAngle;
    } else if (p1.x >= p2.x && p1.y <= p2.y) {
        _pathAngle = _pathAngle + 180;
    }  else if (p1.x >= p2.x && p1.y <= p2.y) {
        _pathAngle = _pathAngle + 180;
    } else if(p1.x <= p2.x && p1.y <= p2.y) {
         _pathAngle = 360 - _pathAngle;
    }
    
}

shared_ptr<Terminal> Airplane::getFreeTerminal(){
    for(shared_ptr<Terminal> terminal : _airport->getTerminals()){
        if (terminal->getTerminalStatus() == false) {
            // change terminal status to blocked
            terminal->setTerminalStatus(true);
            return terminal;
        }
    }
    // return an exeption that no terminal is free!!!
    throw Exception ("No free Terminal for the airplane found");
};

void Airplane::chooseRandomAirplaneType(){
    int random = rand() % 4;
    _type = Type(random);
 };

void Airplane::setNumberPassenger(){
    if(_type == Airplane::A320){
        _passenger = rand() % 100 + 80;  // random number between 80 - which is the min number of passengers - and 180 which is the max. possible number of passengers in an A320.  
    } else if (_type == Airplane::A380)
    {
        _passenger = rand() % 409 + 100; // random number between 100 - which is the min number of passengers - and 509 which is the max. possible number of passengers in an A380.
    } else if (_type == Airplane::Boing777)
    {
        _passenger = rand() % 300 + 100; // random number between 100 - which is the min number of passengers - and 451 which is the max. possible number of passengers in a Boeing 777.
    } else if (_type == Airplane::Boing747)
    {
        _passenger = rand() % 200 + 100; // random number between 100 - which is the min number of passengers - and 300 which is the max. possible number of passengers in a Boeing 747.
    }
 };

Node::Point Airplane::getPositionOfEndPoint(){
    Node::Point endpoint;
    endpoint.x = 1388;
    endpoint.y = 324;
    return endpoint;
};

Airplane::Status Airplane::getStatus() {
    return _status;
}

void Airplane::simulation(){
    
    _threads.emplace_back(thread(&Airplane::move, this));

};

void Airplane::setLandingPoint(){

    if(_type == Airplane::A320){
        // Node 8 -> LANDINGPOINT_A320 smallest airplane which needs the shortest braking distance
        _destination.x = 480;
        _destination.y = 324;  
    } else if(_type == Airplane::A380){
        // Node 17 -> LANDINGPOINT_A380 because its the biggest airplane which needs the longest braking distance
        _destination.x = 965;
        _destination.y = 324;
    } else {       
        // Node 14 -> LANDINGPOINT_OTHERS all Boeing airplanes have to same braking distance
        _destination.x = 715;
        _destination.y = 324;  
    }

};

Node::Point Airplane::getNodeCoordinates(int pointID) {
    Node::Point point = _navigationMesh[pointID-1].getNodeCoordinates();
    return _navigationMesh[pointID-1].getNodeCoordinates();
};

void Airplane::move(){

    bool move = true;
    
    double cycleDuration = 5; // duration of a single simulation cycle in ms
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    // init stop watch
    lastUpdate = std::chrono::system_clock::now();

    while(move) {
        //std::cout << "DEBUG: Airplane is starting to drive along its path" <<  endl;
        // parse all nodes in _path
        int pathSize = _path.size();
        for (int i = 0; i < pathSize-1; i++) {
            this_thread::sleep_for(chrono::milliseconds(1));
            double completion = 0;
            double lengthOfPathSegment = getLengthOfPathSegment(_path[i].getNodeCoordinates(), _path[i+1].getNodeCoordinates());
            _posPathSegment = 0;
            // calculate _pathAngle of airplane
            calculatePathAngle(_path[i].getNodeCoordinates(),_path[i+1].getNodeCoordinates());
            // move airplane between two nodes in _path
            while(completion < 1.0 && move) {
                long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
                if(timeSinceLastUpdate >= cycleDuration) {
                    // update position with a constant velocity of 1 pixel per cycle which corresponds ~1ms
                    _posPathSegment += 1;
                    // calculate completion rate on the current path segment
                    completion = _posPathSegment / lengthOfPathSegment;
                    int dx = _path[i+1].getNodeCoordinates().x - _path[i].getNodeCoordinates().x; 
                    int dy = _path[i+1].getNodeCoordinates().y - _path[i].getNodeCoordinates().y;
                    double xnew = _path[i].getNodeCoordinates().x + dx * completion; 
                    double ynew = _path[i].getNodeCoordinates().y + dy * completion; 
                    _currentPosition.x = xnew;
                    _currentPosition.y = ynew;
                     
                    if (_path[i+1].getType() == Node::Type::LANDINGPOINT_A320 && _type == Type::A320 && _status == Status::LANDING && _currentPosition.x == _path[i+1].getNodeCoordinates().x && _currentPosition.y == _path[i+1].getNodeCoordinates().y) {
                        // reduce cylce duration to simulate that airplane has been landed and therefore moves with lower velocity.
                        cycleDuration = 25;
                        // calculate a new path from landing point to node 36 (WAITINGPOINT)
                        _path = AStar::search(_navigationMesh, _currentPosition, getNodeCoordinates(36));
                        // change status of airplane to landed
                        _status = Status::LANDED;
                    };
                    if (_path[i+1].getType() == Node::Type::LANDINGPOINT_A380 && _type == Type::A380 && _status == Status::LANDING && _currentPosition.x == _path[i+1].getNodeCoordinates().x && _currentPosition.y == _path[i+1].getNodeCoordinates().y) {
                        // reduce cylce duration to simulate that airplane has been landed and therefore moves with lower velocity.
                        cycleDuration = 25;
                        // calculate a new path from landing point to node 36 (WAITINGPOINT)
                        _path = AStar::search(_navigationMesh, _currentPosition, getNodeCoordinates(36));
                        // change status of airplane to landed
                        _status = Status::LANDED;
                    };
                    if (_path[i+1].getType() == Node::Type::LANDINGPOINT_OTHERS && _type != Type::A320 && _type != Type::A380 && _status == Status::LANDING && _currentPosition.x == _path[i+1].getNodeCoordinates().x && _currentPosition.y == _path[i+1].getNodeCoordinates().y) {
                        // reduce cylce duration to simulate that airplane has been landed and therefore moves with lower velocity.
                        cycleDuration = 25;
                        // call AStar to calculate path to node 36
                        _path = AStar::search(_navigationMesh, _currentPosition, getNodeCoordinates(36));
                        // change status of airplane to landed
                        _status = Status::LANDED;
                    };
                    // check if airplane has left the runway on its way to the terminal
                    if (_currentPosition.y == _airport->getRunwayBoarderCoordinate() && _status == Status::LANDED) {
                        // send signal to runway, that airplane has left
                        _airport->vehicleHasLeftRunway();
                        _status = Status::LEFTRUNWAY;
                    };
                    // check if airplane has reached the WAITING POINT 36 and status is not DEPARTURED
                    if(_path[i+1].getType() ==  Node::Type::WAITINGPOINT && _currentPosition.x == _path[i+1].getNodeCoordinates().x && _currentPosition.y == _path[i+1].getNodeCoordinates().y && _status != Status::DEPARTURED) {
                        //std::cout << "DEBUG: Waiting Point has been reached" << endl;
                        //calculate new path from currentPosition (waitingpoint) to destination, which is any free terminal.
                        _terminal = getFreeTerminal();
                        _path = AStar::search(_navigationMesh, _currentPosition, _terminal->getTerminalCoordinates());
                    };
                    // check if Terminal has been reached
                    if(_path[i+1].getType() ==  Node::Type::TERMINAL && _currentPosition.x == _path[i+1].getNodeCoordinates().x && _currentPosition.y == _path[i+1].getNodeCoordinates().y) {
                        _status = Status::OFFBOARDING;
                        //std::cout << "DEBUG: Terminal has been reached" << endl;
                        // time needed for offboarding depends on number of passengers of the airplane: 0.1 sec per passenger!
                        int duration = _passenger * 0.1;
                        this_thread::sleep_for(chrono::seconds(duration));
                        // calculate new path to Endpoint of the runway
                        _destination = getPositionOfEndPoint();
                        _path = AStar::search(_navigationMesh, _currentPosition, _destination);
                        _status = Status::DEPARTURED;
                        // unblock terminal
                        _terminal->setTerminalStatus(false);
                    };
                    //check if runway will be entered soon and if yes request entering the runway if it is free else wait until free, Status == DEPARTURED
                    if(_currentPosition.y == 300 && _currentPosition.x == 668 && _status == Status::DEPARTURED) {
                        // request entry to runway
                        auto ftrEntryGranted = async(&Runway::addAirplaneToQueue, _airport->getRunway(), shared_from_this());
                        // wait until entry has been granted
                        ftrEntryGranted.get();
                    };
                    // check if final position for taking off is reached
                    if(_currentPosition.y == 323 && _status == Status::DEPARTURED) {
                        // wait for 2sec to simulate last check before taking off
                        this_thread::sleep_for(chrono::milliseconds(1000));
                        // reduce cycle duration to simulation accelaration when taking off
                        cycleDuration = 5;
                    };
                    // check if end of runway is reached
                    if(_currentPosition.x == getPositionOfEndPoint().x && _currentPosition.y == getPositionOfEndPoint().y) {
                        // terminate the move thread to be able to delete this
                        move = false;
                        // set status to HASLEFT
                        _status = Status::HASLEFT;
                        // unblock runway
                        _airport->getRunway()->setRunwayIsBlocked(false);
                    };
                    // reset stop watch for next cycle
                    lastUpdate = std::chrono::system_clock::now();
                };
            };
        };        
    };
    
};

Airplane::~Airplane(){
    // set up thread barrier before this object is destroyed
    for_each(_threads.begin(), _threads.end(), [](std::thread &t) {
        t.join();
    });
    //cout << "DEBUG: Airplane destructor is called" << endl;
}



