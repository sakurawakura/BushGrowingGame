#ifndef GAME_H
#define GAME_H

#include "Printable.h"
#include "Timeline.h"
#include "Tree.h"
#include "Clickable.h"
#include "Player.h"
#include "GrowingAction.h"
#include "WateringAction.h"
#include "FertilisingAction.h"
#include "PruningAction.h"

// Removed: Forward declaration for nlohmann::json
// namespace nlohmann {
//     class json;
// }

using namespace cv;

enum GameState{
    MAIN_MENU,
    INSTRUCTION_MENU,
    IN_GAME,
    PRUNING_ACTION,
    AWAITING_WATER_INPUT,
    AWAITING_FERTILISER_INPUT,
    BERRIES_MENU
};

/**
 * @brief Manages the overall game state, game loop, user input, and screen rendering.
 * It orchestrates interactions between the Player, Tree, Timeline, and UI elements.
 */
class Game : Printable{
    public:
        Game(int windowWidth, int windowHeight);

        ~Game();

        void handleInputs(int keyPressed); // Modified to accept key presses

        void drawScreen();

        void printData();

        static void handleMouseClick(int event, int mouseX, int mouseY, int , void*);

        static int mouseXPos;
        static int mouseYPos;
        static bool mouseClicked;

    private:
        Mat* screenImg;  

        Tree* gameTree;
        Player* gamePlayer;
        Timeline* gameTimeline;

        vector<Clickable*> buttonList;

        int WINDOW_WIDTH;
        int WINDOW_HEIGHT;

        GameState currentState;

        Clickable* saveGameButton; // Save Game button
        Clickable* loadGameButton; // Load Game button
        Clickable* berriesMenuButton; // Berries button

        std::string currentInputText; // For text input pop-up
        int pendingActionType; //

        void drawInGameBackgroundUI(); // Helper method for common UI drawing
        void drawTextLines(cv::Mat* targetImg, const std::vector<std::string>& lines, int x, int startY, int lineSpacing, int fontFace, double fontScale, const cv::Scalar& color, int thickness); // Helper for drawing multiple text lines
        void saveGame(); // save
        void loadGame(); // load game

        std::string transientMessage;
        double transientMessageDurationSeconds;
        double transientMessageStartTime; // To store time in seconds

        // Fruit collection counters
        int redFruitsCollectedCount;
        int blueFruitsCollectedCount;
        int goldFruitsCollectedCount;
};




#endif