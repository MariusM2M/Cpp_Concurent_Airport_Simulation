#include "graphics.hpp"
#include "airport.hpp"

int main()
{
    // relative path to backgroundimage
    string backGroundImage = "../resources/stuttgart-airport.jpg";
    string airplaneImage = "../resources/airplane_vector_scaled50x50_blue.png";

    // Create airport object as a shared pointer of type airport.
    shared_ptr<Airport> airport = make_shared<Airport>();

    // Launch visualization: only the airplanes are visualized

    // draw all objects in vector
    Graphics *graphics = new Graphics();
    graphics->setBgFilename(backGroundImage);
    graphics->setArplnFilename(airplaneImage);
    graphics->setAirport(airport);
    graphics->simulate();
}