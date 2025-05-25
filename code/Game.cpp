#include "Game.h"
#include <fstream> // For std::ofstream
#include "include/nlohmann/json.hpp" // For JSON serialization

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

    // Instantiate Save Game button
    Rect saveGameButtonRect(WINDOW_WIDTH-buttonWidth, 500, buttonWidth, 100); // Placed below "Reverse action"
    saveGameButton = new Clickable(saveGameButtonRect, 10, "Save Game");

    // Instantiate Load Game button for Main Menu
    Rect loadGameButtonRect(WINDOW_WIDTH/2-100, WINDOW_HEIGHT/2+170, 200, 100); // Below "Instructions"
    loadGameButton = new Clickable(loadGameButtonRect, 11, "Load Game");

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
    delete saveGameButton; // Free Save Game button
    delete loadGameButton; // Free Load Game button

}

void Game::drawScreen(){
    //Clears what was previously on the screen by drawing a vertical gradient
    // Define start and end colors for the gradient (Sky Blue to a deeper Steel Blue)
    Scalar startColor = CV_RGB(135, 206, 250); // Light Sky Blue (Note: CV_RGB uses BGR order internally, but definition is RGB)
    Scalar endColor = CV_RGB(70, 130, 180);   // Steel Blue

    int width = screenImg->cols;
    int height = screenImg->rows;

    for (int y = 0; y < height; ++y) {
        // Calculate interpolation factor (0.0 at top, 1.0 at bottom)
        float t = static_cast<float>(y) / height;

        // Interpolate RGB components
        // OpenCV Scalar stores channels in BGR order. CV_RGB(R,G,B) creates a Scalar(B,G,R).
        // So, startColor.val[2] is R, .val[1] is G, .val[0] is B.
        int r = static_cast<int>(startColor.val[2] * (1.0f - t) + endColor.val[2] * t);
        int g = static_cast<int>(startColor.val[1] * (1.0f - t) + endColor.val[1] * t);
        int b = static_cast<int>(startColor.val[0] * (1.0f - t) + endColor.val[0] * t);
        
        // Draw a horizontal line with the interpolated color
        // Note: CV_RGB macro expects R, G, B order for its arguments.
        cv::line(*screenImg, Point(0, y), Point(width - 1, y), CV_RGB(r, g, b), 1);
    }

    switch(currentState) {
    case MAIN_MENU:
        //Draws the play button
        buttonList[0]->draw(screenImg);
        //Draws the instruction menu button
        buttonList[1]->draw(screenImg);
        //Draws the load game button
        if (loadGameButton) { // Ensure it's initialized
            loadGameButton->draw(screenImg);
        }
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
        // Draw Save Game button in Pruning Action state
        if (saveGameButton) { // Ensure it's initialized
            saveGameButton->draw(screenImg);
        }
        // Note: IN_GAME case is below and will also draw relevant buttons.
        // The structure of switch-case (fall-through from PRUNING_ACTION to IN_GAME for drawing)
        // means IN_GAME drawing logic will also apply if not explicitly broken.

    case IN_GAME:
        //Draws the back button
        buttonList[2]->draw(screenImg);

        //Draws the action buttons from buttonList
        for(int i = 4; i < buttonList.size(); i++){
            buttonList[i]->draw(screenImg);
        }

        // Draw Save Game button in In Game state
        if (saveGameButton) { // Ensure it's initialized
             saveGameButton->draw(screenImg);
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
        if(buttonList[0]->contains(mousePos)){ // Play button
            currentState = IN_GAME;
        } else if(buttonList[1]->contains(mousePos)){ // Instructions button
            currentState = INSTRUCTION_MENU;
        } else if (loadGameButton && loadGameButton->contains(mousePos)) { // Load Game button
            loadGame();
            // mouseClicked = false; // Optional: prevent other actions on same click
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
        } else if (saveGameButton && saveGameButton->contains(mousePos)) {
            saveGame();
            // mouseClicked = false; // Optional: prevent other actions on same click
        }

    break;
    case IN_GAME:
        if(buttonList[2]->contains(mousePos)){ // Back button
            currentState = MAIN_MENU;
        } else if (saveGameButton && saveGameButton->contains(mousePos)) { // Save Game button
            saveGame();
            // mouseClicked = false; // Optional: prevent other actions on same click
        }
        //Checks if any of the actions are taken
        //Add water button pressed
        else if(buttonList[4]->contains(mousePos)){
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

void Game::saveGame() {
    nlohmann::json saveData;

    // Serialize gameTree
    if (gameTree) {
        saveData["tree"] = gameTree->toJson();
    }

    // Serialize gamePlayer
    if (gamePlayer) {
        saveData["player"] = gamePlayer->toJson();
    }
    
    // Potentially save other relevant game state if any
    // saveData["gameState"] = static_cast<int>(currentState); // Example if you want to save current game state enum

    std::ofstream file("savegame.json");
    if (file.is_open()) {
        // Using dump(4) for pretty-printing JSON with an indent of 4 spaces
        file << saveData.dump(4); 
        file.close();
        std::cout << "Game saved to savegame.json" << std::endl;
        // Optionally provide visual feedback to the user on screen here
    } else {
        std::cerr << "Error: Could not open savegame.json for writing." << std::endl;
        // Optionally provide visual error feedback to the user on screen here
    }
}

void Game::loadGame() {
    std::ifstream file("savegame.json");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open savegame.json for reading. Starting new game." << std::endl;
        // Optional: Display this message on screen instead of console
        // Creating a new default game session or letting user click "Play"
        // For now, we just return and let the user decide next action (e.g. click Play for new game)
        return;
    }

    nlohmann::json saveData;
    try {
        file >> saveData;
    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "Error: Could not parse savegame.json: " << e.what() << std::endl;
        file.close();
        // Optional: Display this message on screen
        return;
    }
    file.close();

    // Clear existing game objects before loading
    delete gameTree;
    gameTree = nullptr; // Set to nullptr after delete to avoid dangling pointer issues
    delete gamePlayer;
    gamePlayer = nullptr;
    delete gameTimeline; 
    gameTimeline = new Timeline(); // Reset timeline

    bool treeLoaded = false;
    // Deserialize and reconstruct gameTree
    if (saveData.contains("tree")) {
        gameTree = Tree::fromJson(saveData["tree"]);
        if (!gameTree) {
            std::cerr << "Error: Failed to load tree data from savefile. Corrupted or invalid data." << std::endl;
            // gameTree is already nullptr
        } else {
            treeLoaded = true;
        }
    } else {
        std::cerr << "Error: Save data does not contain tree information." << std::endl;
    }

    // If tree loading failed, reinitialize default tree
    if (!treeLoaded) {
        std::cout << "Starting new game with default tree." << std::endl;
        gameTree = new Tree(10, 10, new Branch(0, 0, 1, 50, 10,  WINDOW_WIDTH/ 2, WINDOW_HEIGHT));
    }

    bool playerLoaded = false;
    // Deserialize and reconstruct gamePlayer
    if (saveData.contains("player")) {
        // Player::fromJson returns a Player object by value, so we construct a new Player on the heap
        gamePlayer = new Player(Player::fromJson(saveData["player"]));
        playerLoaded = true;
    } else {
        std::cerr << "Error: Save data does not contain player information." << std::endl;
    }
    
    // If player loading failed, reinitialize default player
    if (!playerLoaded) {
        std::cout << "Using default player." << std::endl;
        // Ensure gamePlayer is not leaked if it was partially allocated or something went wrong
        delete gamePlayer; // delete if it was allocated by a failed fromJson or similar
        gamePlayer = new Player(10, 5); // Default player
    }
    
    if (treeLoaded && playerLoaded) {
        currentState = IN_GAME; // Transition to game after successful load
        std::cout << "Game loaded successfully from savegame.json" << std::endl;
    } else {
        std::cout << "Failed to load full game state. Some defaults may be used. Check errors above." << std::endl;
        // Stay on MAIN_MENU or decide on a different state
        // Forcing a new game state if critical parts failed
        if (!treeLoaded) { // If tree is critical and failed to load, don't go IN_GAME
             currentState = MAIN_MENU; // Stay on main menu
             // Re-init default tree and player if not already done by error handling above
             delete gameTree;
             gameTree = new Tree(10, 10, new Branch(0, 0, 1, 50, 10,  WINDOW_WIDTH/ 2, WINDOW_HEIGHT));
             delete gamePlayer;
             gamePlayer = new Player(10, 5);
             std::cout << "Critical load failure. Resetting to new game state." << std::endl;
        } else {
            // If only player failed, we might still proceed or handle differently.
            // For now, if either critical part (tree) fails, it's not a successful load to IN_GAME.
            currentState = MAIN_MENU; // Or specific error state
        }
    }
}