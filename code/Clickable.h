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

    // draws the button with enhanced styling
    void draw(Mat* img) const { // Removed color and thickness parameters to use internal styling
        // Styling choices
        Scalar backgroundColor = CV_RGB(220, 220, 220); // Light grey
        Scalar borderColor = CV_RGB(150, 150, 150);     // Medium grey for border
        Scalar textColor = CV_RGB(0, 0, 0);             // Black text
        int borderThickness = 2;
        double fontScale = 0.8; // Adjusted for better fit
        int textThickness = 2;

        // Draw the filled background rectangle
        rectangle(*img, rect_, backgroundColor, FILLED);

        // Draw the border rectangle
        rectangle(*img, rect_, borderColor, borderThickness);

        // Calculate text size to center it
        int baseline = 0;
        Size textSize = getTextSize(buttonText_, FONT_HERSHEY_SIMPLEX, fontScale, textThickness, &baseline);

        // Calculate text position for centering
        Point textOrg(rect_.x + (rect_.width - textSize.width) / 2,
                      rect_.y + (rect_.height + textSize.height) / 2 - baseline / 2); // Adjusted for better vertical centering

        // Draw the text
        putText(*img, buttonText_, textOrg, FONT_HERSHEY_SIMPLEX, fontScale, textColor, textThickness);
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