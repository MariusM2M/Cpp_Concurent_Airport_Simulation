#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include "airplane.hpp"
#include "airport.hpp"

class Airplane;

class Graphics
{
public:

    // getters / setters
    void setBgFilename(std::string filename) { _bgFilename = filename; }
    void setArplnFilename(std::string filename) { _arplnFilename = filename; }
    void setAirplanes() { _airplanes = _airport -> getAirplanes(); }
    void setAirport(std::shared_ptr<Airport> airport) { _airport = airport; }

    // typical behaviour methods
    void simulate();

private:

    // typical behaviour methods
    void loadBackgroundImg();
    cv::Mat rotatedImage(double angle);
    void drawAirplanes();

    // member variables
    std::shared_ptr<Airport> _airport;
    std::vector<std::shared_ptr<Airplane>> _airplanes;
    std::string _bgFilename;
    std::string _arplnFilename;
    std::string _windowName;
    std::vector<cv::Mat> _images;
};

#endif