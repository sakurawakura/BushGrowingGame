#ifndef CLICKABLE_H
#define CLICKABLE_H // making it useful for other scripts

#include <opencv2/opencv.hpp>
#include <functional> // For std::function

using namespace cv;

// the clickable class
class Clickable {

public:
    // Member to track pressed state??
    mutable bool isPressed; 

    //pass though the features needed for clickable class
    Clickable(Rect rect, int id, string text) : rect_(rect), id_(id), clicked_(false), buttonText_(text), isPressed(false) {}

    //iiis the cursor over the clickable area?
    bool contains(Point2f pt) const { return rect_.contains(pt); }

    // did user click itt
    void setClicked(bool clicked) { clicked_ = clicked; }

    // if its clicked return as clicked_
    bool isClicked() const { return clicked_; }

    // ids to keep track of buttons
    int getId() const { return id_; }

// ------------------- visual implementation

    // draw button (CHATGPT FOR STYLING)
    void draw(Mat* img) const { 
        Scalar originalBgColor = CV_RGB(220, 220, 220);    // Light grey
        Scalar originalBorderColor = CV_RGB(150, 150, 150); // Medium grey for border
        int originalBorderThickness = 2;
        
        Scalar pressedBgColor = CV_RGB(200, 200, 200);      // Slightly darker grey
        Scalar pressedBorderColor = CV_RGB(130, 130, 130);   // Slightly darker border
        int pressedBorderThickness = 3; // Thicker border

        Scalar textColor = CV_RGB(0, 0, 0);             // Black text
        double fontScale = 0.8; 
        int textThickness = 2;

        // Determine current styles based on isPressed state
        Scalar currentBgColor = this->isPressed ? pressedBgColor : originalBgColor;
        Scalar currentBorderColor = this->isPressed ? pressedBorderColor : originalBorderColor;
        int currentBorderThickness = this->isPressed ? pressedBorderThickness : originalBorderThickness;

        // Draw the filled background rectangle
        rectangle(*img, rect_, currentBgColor, FILLED);

        // Draw the border rectangle
        rectangle(*img, rect_, currentBorderColor, currentBorderThickness);

        // Calculate text size to center it
        int baseline = 0;
        Size textSize = getTextSize(buttonText_, FONT_HERSHEY_SIMPLEX, fontScale, textThickness, &baseline);

        // Calculate text position for centering
        Point textOrg(rect_.x + (rect_.width - textSize.width) / 2,
                      rect_.y + (rect_.height + textSize.height) / 2 - baseline / 2); 

        // Optionally offset text when pressed
        if (this->isPressed) {
            textOrg.x += 1;
            textOrg.y += 1;
        }

        // Draw the text
        putText(*img, buttonText_, textOrg, FONT_HERSHEY_SIMPLEX, fontScale, textColor, textThickness);

        // Reset isPressed state after drawing
        if (this->isPressed) {
            this->isPressed = false; // Requires isPressed to be mutable
        }
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