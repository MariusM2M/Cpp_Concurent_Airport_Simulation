# ifndef _AIRPORT_
# define _AIRPORT_

#include "graph.hpp"
#include <memory>
#include <mutex>
#include <future>
#include <memory>


class Airplane;

class WaitingAirplanes {

    private:
        vector<shared_ptr<Airplane>> _airplanes;    // list of all airplanes waiting to enter the runway
        vector<promise<void>> _promises;            // list of all associated promises
        std::mutex _mutex;                          

    public:
        int getSize();
        void pushBack(std::shared_ptr<Airplane> airplane, promise<void> &&promise);
        void permitEntryToFirstInQueue();
};


class Runway {
    
    private:
        vector<Node::Point> _runwayRectCoordinates;
        bool _isBlocked = false; 
        WaitingAirplanes _waitingAirplanes;
        std::mutex _mutex;     

    public:
        Runway();
        ~Runway();
        int getRunwayBoarderCoordinate();
        WaitingAirplanes getWaitingAirplanes();
        void setRunwayRectCoordinates();
        void setRunwayIsBlocked(bool status);
        bool isBlocked();
        void addAirplaneToQueue(std::shared_ptr<Airplane> airplane);
        void handleAirplaneQueue();
        void simulation();
    
    protected:
        vector<thread> _threads; // holds all threads that have been launched within the runway object
    

};

class Terminal {

    private:
        bool _isBlocked = false;
        Node::Point _coordinates;
        std::mutex _mutex; 
    public:
        Terminal(Node::Point point);
        Node::Point getTerminalCoordinates();
        void setTerminalCoordinates(Node::Point point);
        bool getTerminalStatus();
        void setTerminalStatus(bool status);

};

class Airport: public std::enable_shared_from_this<Airport> {

    private:
        vector<Node> _navigationMesh;
        shared_ptr<Runway> _runway;
        vector<shared_ptr<Terminal>> _terminals;
        vector<shared_ptr<Airplane>> _airplanes;
        static int _cntAirplanes;            // global variable to count number of airplanes 
        std::mutex _mutex;                   // needed to protect _airplanes

    public:
        // default constructor
        Airport();
        ~Airport();
        vector<shared_ptr<Airplane>> getAirplanes();
        void spawnAirplanes();
        int getRunwayBoarderCoordinate();
        void vehicleHasLeftRunway();
        shared_ptr<Airport> get_shared_this();
        vector<shared_ptr<Terminal>> getTerminals();
        shared_ptr<Runway> getRunway();
        void deleteAirplanesWhichHaveReachedDestination();
    
        protected:
        vector<thread> _threads;    // holds all threads that have been launched within the airport object
};

#endif // _AIRPORT_