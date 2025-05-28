#include "main.h" // Includes Game.h and OpenCV headers like highgui.hpp

// Window interface
int main(void) {
    //Sets size of window
    int SCREEN_WIDTH = 800;
    int SCREEN_HEIGHT = 500; // Original was 500, test harness used 600. Reverting to 500.

    //Creates an instance of the game
    // Note: Game constructor takes windowWidth, windowHeight
    Game game = Game(SCREEN_WIDTH, SCREEN_HEIGHT);

    //Game loop runs until escape key is pressed
    int keyPressed = -1;
    while((keyPressed = cv::pollKey()) != 27){ // Capture key, then check for ESC (27 aCSI)
        game.handleInputs(keyPressed); // Pass the pressed key
        game.drawScreen();
    };

    return 0;
}
