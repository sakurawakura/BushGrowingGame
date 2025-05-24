#ifndef CLICKABLE_H
#define CLICKABLE_H // making it useful for other scripts

#include <opencv2/opencv.hpp>
#include <functional> // For std::function

using namespace cv;

// the clickable class
class Clickable {

public:

    //pass though the features needed for clickable class
    Clickable(Rect rect, int id, string text) : rect_(rect), id_(id), clicked_(false), buttonText_(text) {}

    //is the cursor over the clickable area
    bool contains(Point2f pt) const { return rect_.contains(pt); }

    // Did the user click it
    void setClicked(bool clicked) { clicked_ = clicked; }

    // if its clicked return as clicked_
    bool isClicked() const { return clicked_; }

    // an id feature to keeptrack of different buttons
    int getId() const { return id_; }

    // draws a scalar box around the clickable area as a rectangle 
    void draw(Mat* img, const Scalar& color = Scalar(0, 0, 255), int thickness = 2) const {
        rectangle(*img, rect_, color, thickness);
        putText(*img, buttonText_, Point(rect_.x + 5, rect_.y + 32), FONT_HERSHEY_SIMPLEX, 1, color, thickness);
    }

    // callback for getting a click
    void setCallback(function<void(int)> callback) { callback_ = callback; }
    void executeCallback() { if (callback_) callback_(id_); }

// provate attributes 
private:
    Rect rect_;

    int id_;

    bool clicked_;

    string buttonText_;

    function<void(int)> callback_;
};

#endif