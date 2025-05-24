#include "Game.h"

//Sets static variables
int Game::mouseXPos = 0;
int Game::mouseYPos = 0;
bool Game::mouseClicked = false;

Game::Game(int windowWidth, int windowHeight) : currentState(MAIN_MENU){
    WINDOW_WIDTH = windowWidth;
    WINDOW_HEIGHT = windowHeight;


    gamePlayer = new Player(10, 5);

    //Creates tree with a default supply of 10L of water and 10kg of nutrients
    gameTree = new Tree(10, 10, new Branch(0, 0, 1, 50, 10,  WINDOW_WIDTH/ 2, WINDOW_HEIGHT));


    //Creates the image to display to the screen
    screenImg = new Mat(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3, CV_RGB(150, 150, 255));

    //Creates the timeline
    gameTimeline = new Timeline();

    int buttonWidth = 250;

    //Creates on-screen buttons
    Rect mainMenuButtonRect(WINDOW_WIDTH/2-100, WINDOW_HEIGHT/2-50, 200, 100);
    buttonList.push_back(new Clickable(mainMenuButtonRect, 1, "Play"));

    Rect instructionMenuButtonRect(WINDOW_WIDTH/2-100, WINDOW_HEIGHT/2+60, 200, 100);
    buttonList.push_back(new Clickable(instructionMenuButtonRect, 2, "Instructions"));

    Rect backButtonRect(10, 10, 200, 100);
    buttonList.push_back(new Clickable(backButtonRect, 3, "Back" ));

    Rect cancelPruningRect(10, WINDOW_HEIGHT-110, 200, 100);
    buttonList.push_back(new Clickable(cancelPruningRect, 4, "Cancel"));

    Rect waterTreeButtonRect(WINDOW_WIDTH-buttonWidth, 0, buttonWidth, 100);
    buttonList.push_back(new Clickable(waterTreeButtonRect, 5, "Water tree"));

    Rect fertiliseTreeRect(WINDOW_WIDTH-buttonWidth, 100, buttonWidth, 100);
    buttonList.push_back(new Clickable(fertiliseTreeRect, 6, "Fertilise tree"));

    Rect pruneBranchRect(WINDOW_WIDTH-buttonWidth, 200, buttonWidth, 100);
    buttonList.push_back(new Clickable(pruneBranchRect, 7, "Prune branch"));

    Rect growTreeRect(WINDOW_WIDTH-buttonWidth, 300, buttonWidth, 100);
    buttonList.push_back(new Clickable(growTreeRect, 8, "Grow tree"));

    Rect reverseActionRect(WINDOW_WIDTH-buttonWidth, 400, buttonWidth, 100);
    buttonList.push_back(new Clickable(reverseActionRect, 9, "Reverse action"));

    namedWindow("Time Travel Tree", 0);

    //Sets the mouse callback function
    setMouseCallback("Time Travel Tree", Game::handleMouseClick);

    //Tells the user how many supplies they have
    cout << "You have " << gamePlayer->getWaterSupply() << "L of water" << endl;
    cout << "You have " << gamePlayer->getFertiliserSupply() << "kg of fertiliser" << endl;

}

Game::~Game(){
    //Loops through button list and frees memory
    for(int i = 0; i < buttonList.size(); i++){
        delete buttonList[i];
    }

    //Frees memory
    delete screenImg;
    delete gameTree;
    delete gamePlayer;
    delete gameTimeline;

}

void Game::drawScreen(){
    //Clears what was previously on the screen
    *screenImg = CV_RGB(150, 150, 255);


    switch(currentState) {
    case MAIN_MENU:
        //Draws the play button
        buttonList[0]->draw(screenImg);
        //Draws the instruction menu button
        buttonList[1]->draw(screenImg);
        break;
    case INSTRUCTION_MENU:
        //Draws the back button
        buttonList[2]->draw(screenImg);

        //Explains the instructions
        putText(*screenImg, "Water and fertilise your tree so it grows", Point(WINDOW_WIDTH/2-300, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
        putText(*screenImg, "big and tall. Prune branches that you", Point(WINDOW_WIDTH/2-300, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
        putText(*screenImg, "want to remove and reverse your previous", Point(WINDOW_WIDTH/2-300, 250), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
        putText(*screenImg, "actions if you make a mistake or don't", Point(WINDOW_WIDTH/2-300, 300), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
        putText(*screenImg, "like how the tree has grown. You get 2L", Point(WINDOW_WIDTH/2-300, 350), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
        putText(*screenImg, "water and 1kg fertiliser free every time", Point(WINDOW_WIDTH/2-300, 400), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
        putText(*screenImg, "you let your tree grow. Press ESC to quit", Point(WINDOW_WIDTH/2-300, 450), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
         
        break;
    case PRUNING_ACTION:
        //Draws the cancel pruning button
        buttonList[3]->draw(screenImg);

    case IN_GAME:
        //Draws the back button
        buttonList[2]->draw(screenImg);

        //Draws the action buttons
        for(int i = 4; i < buttonList.size(); i++){
            buttonList[i]->draw(screenImg);
        }

        //Draws the tree to the screen
        gameTree->draw(screenImg);

        break;
    }

    cv::imshow("Time Travel Tree", *screenImg);
}

void Game::handleMouseClick(int event, int mouseX, int mouseY, int , void*){
    //Checks that the mouse was clicked
    if(event == EVENT_LBUTTONDOWN){
        //Sets variables appropriately
        Game::mouseXPos = mouseX;
        Game::mouseYPos = mouseY;
        Game::mouseClicked = true;


    }
}

void Game::handleInputs(){
    //Checks that the mouse has been clicked
    if(Game::mouseClicked){
        Game::mouseClicked = false;
    }else{
        return;
    }

    Point mousePos(Game::mouseXPos, Game::mouseYPos);
    int prunedIndex = gameTree->getClickedIndex(mousePos.x, mousePos.y);

    //Checks if any buttons are being pressed
    switch(currentState) {
    case MAIN_MENU:
        //Checks if the play button has been pressed
        if(buttonList[0]->contains(mousePos)){
            currentState = IN_GAME;
        }else if(buttonList[1]->contains(mousePos)){
            currentState = INSTRUCTION_MENU;
        }
        

    break;
    case INSTRUCTION_MENU:
        if(buttonList[2]->contains(mousePos)){
            //Goes back to main menu
            currentState = MAIN_MENU;
        }

    break;
    case PRUNING_ACTION:
        
        if(buttonList[3]->contains(mousePos)){
            //Exits the pruning state
            currentState = IN_GAME;
        }
        else if(prunedIndex != -1){
            //Prunes a branch and returns to the regular game state
            gameTimeline->performAction(new PruningAction(gameTree, prunedIndex));

            currentState = IN_GAME;
        }

    break;
    case IN_GAME:
        if(buttonList[2]->contains(mousePos)){
            currentState = MAIN_MENU;
        }

        //Checks if any of the actions are taken
        //Add water button pressed
        if(buttonList[4]->contains(mousePos)){
            float waterAmount;

            cout << "Enter the amount of water you want to add to the tree" << endl;
            cin >> waterAmount;

            gameTimeline->performAction(new WateringAction(gamePlayer, gameTree, waterAmount));
        //Add fertiliser button pressed
        }else if (buttonList[5]->contains(mousePos)){
            float fertiliserAmount;

            cout << "Enter the amount of fertiliser you want to add to the tree" << endl;
            cin >> fertiliserAmount;

            gameTimeline->performAction(new FertilisingAction(gamePlayer, gameTree, fertiliserAmount));
        //Prune branch button pressed
        }else if (buttonList[6]->contains(mousePos)){
            currentState = PRUNING_ACTION;
        //Grow button pressed
        }else if (buttonList[7]->contains(mousePos)){
            gameTimeline->performAction(new GrowingAction(gamePlayer, gameTree));
        }
        //Reverse action button pressed
        else if(buttonList[8]->contains(mousePos)){
            gameTimeline->reverseAction();
        }


    break;

    }


}


void Game::printData(){
    cout << "Game object" << endl;
    gameTree->printData();
    gamePlayer->printData();
    gameTimeline->printData();

    cout << "Window width: " << WINDOW_WIDTH << endl;
    cout << "Window height: " << WINDOW_HEIGHT << endl;

    cout << "Game state: " << currentState << endl;
}