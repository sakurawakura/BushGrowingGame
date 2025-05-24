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

using namespace cv;

enum GameState{
    MAIN_MENU,
    INSTRUCTION_MENU,
    IN_GAME,
    PRUNING_ACTION
};

class Game : Printable{
    public:
        Game(int windowWidth, int windowHeight);

        ~Game();

        void handleInputs();

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

};




#endif