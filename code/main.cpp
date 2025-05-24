#include "main.h"

// Window interface
int main(void) {
    //Sets size of window
    int SCREEN_WIDTH = 800;
    int SCREEN_HEIGHT = 500;

    //Creates an instance of the game with 
    Game game = Game(SCREEN_WIDTH, SCREEN_HEIGHT);

    //Game loop runs until escape key is pressed
    while(pollKey() != 27){
        game.handleInputs();
        game.drawScreen();
    };

    return 0;
}
