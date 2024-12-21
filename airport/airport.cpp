#include "airport.hpp"
#include "airplane.hpp"
#include <chrono>
#include <thread>
#include <algorithm>

int WaitingAirplanes::getSize() {

    std::lock_guard<std::mutex> lock(_mutex);

    return _airplanes.size();

};

void WaitingAirplanes::pushBack(std::shared_ptr<Airplane> airplane, promise<void> &&promise){
    
    std::lock_guard<std::mutex> lock(_mutex);
    _airplanes.push_back(airplane);
    _promises.push_back(std::move(promise));
};

void WaitingAirplanes::permitEntryToFirstInQueue() {
    
    // lock the class
    lock_guard<mutex> lock(_mutex);

    // get entries from the front of both queues
    auto firstPromise = _promises.begin();
    auto firstAirplane = _airplanes.begin();

    // fulfill promise and send signal back, thus permission to enter has been granted
    firstPromise->set_value();
    // cout << "DEBUG: Promise in  WaitingAirplanes::permitEntryToFirstInQueue() has been granted " << endl;

    // remove front elements from both queues
    _airplanes.erase(firstAirplane);
    _promises.erase(firstPromise);

};

Runway::Runway(){
    Runway::setRunwayRectCoordinates();
};

void Runway::setRunwayRectCoordinates(){
        
        Node::Point edge1;
            edge1.x = 28;
            edge1.y = 309;
        Node::Point edge2;
            edge2.x = 1399;
            edge2.y = 309;
        Node::Point edge3;
            edge3.x = 28;
            edge3.y = 337;
        Node::Point edge4;
            edge4.x = 1399;
            edge4.y = 337;

    _runwayRectCoordinates = {edge1, edge2, edge3, edge4};

};

int Runway::getRunwayBoarderCoordinate(){
    return _runwayRectCoordinates[0].y;
};

void Runway::setRunwayIsBlocked(bool status){
    lock_guard<mutex> lock(_mutex);
    _isBlocked = status;
}

bool Runway::isBlocked(){
    lock_guard<mutex> lock(_mutex);
    return _isBlocked;
}

void Runway::simulation(){

    // start thread to handle the airplane queue
    _threads.emplace_back(thread(&Runway::handleAirplaneQueue, this));
};

void Runway::addAirplaneToQueue(std::shared_ptr<Airplane> airplane){

    // add new vehicle to the end of the waiting line
    std::promise<void> prmsAirplaneAllowedToEnter;
    std::future<void> ftrAirplaneAllowedToEnter = prmsAirplaneAllowedToEnter.get_future();
    _waitingAirplanes.pushBack(airplane, std::move(prmsAirplaneAllowedToEnter));

    // wait until the vehicle is allowed to enter
    ftrAirplaneAllowedToEnter.wait();

};

void Runway::handleAirplaneQueue(){

    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        this_thread::sleep_for(chrono::milliseconds(1000));

        if (_waitingAirplanes.getSize() > 0 && !_isBlocked) {

            // set runway to blocked ...
            this -> setRunwayIsBlocked(true);
            
            // ... and permit entry to the first airplane in the queue
            _waitingAirplanes.permitEntryToFirstInQueue();

        }
    }
};

Runway::~Runway() {
    // set up thread barrier before this object is destroyed
    for_each(_threads.begin(), _threads.end(), [](std::thread &t) {
        t.join();
    });
}

Terminal::Terminal(Node::Point point){
  
    _coordinates = point;

};

void Terminal::setTerminalCoordinates(Node::Point point){
    
    _coordinates = point;

};

Node::Point Terminal::getTerminalCoordinates(){
    return _coordinates;
};

bool Terminal::getTerminalStatus(){
    lock_guard<mutex> lock(_mutex);
    return _isBlocked;
};

void Terminal::setTerminalStatus(bool status){
    lock_guard<mutex> lock(_mutex);
    _isBlocked = status;
};


// init static variable
int Airport::_cntAirplanes = 0;

Airport::Airport(){

    // initialize the navigation mesh of the airport
    NavigationMesh mesh;
    _navigationMesh = mesh.getNavMesh();

    // initialize the runway. Note: use shared pointer to ensure the object exists until the thread is finished
    _runway =  make_shared<Runway>();
    // start simulation of the runway
    _runway->simulation();

    // initialize the Terminals
    Node::Point coordinatesT1;
        coordinatesT1.x = 20;
        coordinatesT1.y = 86;
    shared_ptr<Terminal> t1 = make_shared<Terminal>(coordinatesT1);
    _terminals.push_back(t1);

    Node::Point coordinatesT2;
        coordinatesT2.x = 46;
        coordinatesT2.y = 86;
    shared_ptr<Terminal> t2 = make_shared<Terminal>(coordinatesT2);
    _terminals.push_back(t2);

    Node::Point coordinatesT3;
        coordinatesT3.x = 73;
        coordinatesT3.y = 86;
    shared_ptr<Terminal> t3 = make_shared<Terminal>(coordinatesT3);
    _terminals.push_back(t3);

    Node::Point coordinatesT4;
        coordinatesT4.x = 100;
        coordinatesT4.y = 86;
    shared_ptr<Terminal> t4 = make_shared<Terminal>(coordinatesT4);
    _terminals.push_back(t4);

    Node::Point coordinatesT5;
        coordinatesT5.x = 184;
        coordinatesT5.y = 86;
    shared_ptr<Terminal> t5 = make_shared<Terminal>(coordinatesT5);
    _terminals.push_back(t5);

    // spawn airplanes
    _threads.emplace_back(thread(&Airport::spawnAirplanes, this));
    // delete airplanes which have reached their destination
    _threads.emplace_back(thread(&Airport::deleteAirplanesWhichHaveReachedDestination, this));

};

int Airport::getRunwayBoarderCoordinate() {
    return _runway->getRunwayBoarderCoordinate();
};

vector<shared_ptr<Airplane>> Airport::getAirplanes() {
    return _airplanes;
}

void Airport::vehicleHasLeftRunway(){
    _runway->setRunwayIsBlocked(false);
};

shared_ptr<Airport> Airport::get_shared_this() { 
    return shared_from_this(); 
};

vector<shared_ptr<Terminal>> Airport::getTerminals(){
    return _terminals;
};

shared_ptr<Runway> Airport::getRunway(){
    return _runway;
}

void Airport::spawnAirplanes(){

    while(true) {
        // sleep for 1 milliseconds to reduce CPU load: TODO: Change from 1 sec to 1 millisecond
        this_thread::sleep_for(chrono::milliseconds(1));

        // spawn airplane if maximum number of airplanes is not reached and runway is free
        if (_cntAirplanes < 5 && !_runway->isBlocked()) {
            // initialize a new airplanes
            shared_ptr<Airplane> airplane = make_shared<Airplane>(_navigationMesh, get_shared_this());
            // ---> request entry to the runway (using async) --> spawn airplanes is not running because it is waiting to get entry to the runway
            // however there are already two airplanes in the waitingque, which have to be served first.
            auto ftrEntryGranted = async(&Runway::addAirplaneToQueue, _runway, airplane);
            // wait until entry has been granted
            ftrEntryGranted.get();
            // add airplane to container with all airplanes currently on the airport
            std::unique_lock<std::mutex> lck(_mutex);
            _airplanes.push_back(airplane);
            _cntAirplanes++;
            lck.unlock();
            // start simulation of the airplane
            airplane->simulation();
            // check if any of the airplanes in _airplanes has reached its destination
        };
        
    };
};

void Airport::deleteAirplanesWhichHaveReachedDestination(){
    while(true){
        this_thread::sleep_for(chrono::milliseconds(1000));
        if(_airplanes.size() > 0){
            for(int i=0; i < _airplanes.size(); i++){
                int currentPosX = _airplanes[i]->getCurrentPosition().x;
                int currentPosY = _airplanes[i]->getCurrentPosition().y;
                int endpointX = _airplanes[i]->getPositionOfEndPoint().x;
                int endpointY = _airplanes[i]->getPositionOfEndPoint().y;
                Airplane::Status status =  _airplanes[i] -> getStatus();
                if(currentPosX == endpointX && currentPosY == endpointY && status == Airplane::Status::HASLEFT){
                    std::unique_lock<std::mutex> lck(_mutex);
                    _airplanes[i].reset();
                    _airplanes.erase(_airplanes.begin()+i);
                    _cntAirplanes--;
                    lck.unlock();
                    //break;
                };
            };
        };
    };
};

Airport::~Airport(){
    // set up thread barrier before this object is destroyed
    for_each(_threads.begin(), _threads.end(), [](std::thread &t) {
        t.join();
    });

}