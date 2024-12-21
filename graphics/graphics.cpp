#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include "graphics.hpp"
#include "graph.hpp"

void Graphics::simulate()
{
    this->loadBackgroundImg();
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // update airplanes
        this->setAirplanes();

        // update graphics
        this->drawAirplanes();
    }
}

void Graphics::loadBackgroundImg()
{
    // create window
    _windowName = "Concurrency Airport Simulation";
    cv::namedWindow(_windowName, cv::WINDOW_NORMAL);

    // load image and create copy to be used for semi-transparent overlay
    cv::Mat background = cv::imread(_bgFilename);
    
    // load airplane image
    cv::Mat airplane = cv::imread(_arplnFilename);

    // resize airplane image
    cv::Mat airplane_small;
    cv::resize(airplane, airplane_small, cv::Size(35,35),0,0,1);

    // remove black background from airplane image
    cv::Mat airplane_small_gray;
    cv::cvtColor(airplane_small, airplane_small_gray, cv::COLOR_RGB2GRAY);

    // turn all pixels with threshold 20 into black
    cv::Mat mask;
    cv::threshold(airplane_small_gray,mask,20,255,cv::THRESH_BINARY);

    // Create inverse mask
    cv::Mat mask_inv;
    cv::bitwise_not(mask, mask_inv);

    _images.push_back(background);              // first element is the original background
    _images.push_back(background.clone());      // second element will be the displayed image including all airplanes
    _images.push_back(airplane);                // third element is the original airplane image
    _images.push_back(mask_inv);                // inverse mask of the original airplane image

}

cv::Mat Graphics::rotatedImage(double angle){

    // calculate center of image for rotation
    cv::Point2f center((_images.at(3).cols)/2,(_images.at(3).cols)/2);

    // get rotation matrix
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);

    cv::Mat rotatedMask_inv;
    // rotate inverse mask
    cv::warpAffine(_images.at(3), rotatedMask_inv, rotationMatrix, _images.at(3).size());

    return rotatedMask_inv;

}

void Graphics::drawAirplanes()
{
    // reset background image
    _images.at(1) = _images.at(0).clone();

    for (auto it : _airplanes) {
        Node::Point position;
        position = it->getCurrentPosition();
        cv::Mat mask;
        if (it->getPathAngle() != 0){
            // rotate Image
            double angle = it->getPathAngle();
            mask = rotatedImage(angle);
        } else {
           mask = _images.at(3);
        }
        // This is a inconsistency in the program: There are situations where the position of a airplane is 0. This causes openCV to crash when the roi is generated.
        if(position.x != 0 && position.y != 0) {
            cv::Mat Roi = _images.at(1)(cv::Rect(position.x - mask.cols/2, position.y - mask.cols/2, mask.cols, mask.rows));
            cv::Mat background;
            cv::bitwise_or(Roi, Roi, background, mask = mask);
            background.copyTo(_images.at(1)(cv::Rect(position.x - mask.cols/2, position.y - mask.cols/2, mask.cols, mask.rows)));
        }
    }

    cv::namedWindow(_windowName, cv::WINDOW_NORMAL);
    cv::imshow(_windowName, _images.at(1));
    cv::waitKey(33);
}
