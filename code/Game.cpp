#include "Game.h"
#include <fstream> // Ffor ofstream


//Sets static variables
int Game::mouseXPos = 0;
int Game::mouseYPos = 0;
bool Game::mouseClicked = false;

// const for game, initalises all vars
Game::Game(int windowWidth, int windowHeight) : currentState(MAIN_MENU){ // making the game window
    WINDOW_WIDTH = windowWidth;
    WINDOW_HEIGHT = windowHeight;
    currentInputText = "";
    pendingActionType = 0;

    transientMessage = "";
    transientMessageDurationSeconds = 0.0;
    transientMessageStartTime = 0.0;

    redFruitsCollectedCount = 0;
    blueFruitsCollectedCount = 0;
    goldFruitsCollectedCount = 0;

    gamePlayer = new Player(10, 5);

    //Creates a bush with default values
    gameTree = new Tree(10, 10, new Branch(0, 0, 1, 50, 10,  WINDOW_WIDTH/ 2, WINDOW_HEIGHT));

    //Make background image
    screenImg = new Mat(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3, CV_RGB(150, 150, 255));

    //timeline for new game
    gameTimeline = new Timeline();

    //button values

    int buttonWidth = 200; 
    int actionButtonHeight = 60;

    //--------------- MAIN MENU BUTTONS
    int mainMenuButtonWidth = 200;
    int mainMenuButtonHeight = 80;
    int mainMenuButtonX = WINDOW_WIDTH / 2 - mainMenuButtonWidth / 2;
    int titleBottomMargin = 120; 
    int buttonGap = 15;

    int playButtonY = titleBottomMargin + buttonGap; //play
    Rect mainMenuButtonRect(mainMenuButtonX, playButtonY, mainMenuButtonWidth, mainMenuButtonHeight);
    buttonList.push_back(new Clickable(mainMenuButtonRect, 1, "Play"));

    int instructionsButtonY = playButtonY + mainMenuButtonHeight + buttonGap; //instructions
    Rect instructionMenuButtonRect(mainMenuButtonX, instructionsButtonY, mainMenuButtonWidth, mainMenuButtonHeight);
    buttonList.push_back(new Clickable(instructionMenuButtonRect, 2, "Instructions"));
    
  
    int loadGameButtonY = instructionsButtonY + mainMenuButtonHeight + buttonGap; //load
    Rect loadGameButtonRect(mainMenuButtonX, loadGameButtonY, mainMenuButtonWidth, mainMenuButtonHeight); 
    loadGameButton = new Clickable(loadGameButtonRect, 11, "Load Game");


    int berriesButtonY = loadGameButtonY + mainMenuButtonHeight + buttonGap; // berries
    Rect berriesMenuButtonRect(mainMenuButtonX, berriesButtonY, mainMenuButtonWidth, mainMenuButtonHeight);
    berriesMenuButton = new Clickable(berriesMenuButtonRect, 12, "Berries");

    // Other Buttons (Back, Cancel, Actions, Save) ---- INGAME BUTTONS

    Rect backButtonRect(10, 10, 120, 50); //back
    buttonList.push_back(new Clickable(backButtonRect, 3, "Back" ));

    Rect cancelPruningRect(10, WINDOW_HEIGHT-60, 150, 50); // cancel trim
    buttonList.push_back(new Clickable(cancelPruningRect, 4, "Cancel"));

    Rect waterBushRect(WINDOW_WIDTH-buttonWidth, 0 * actionButtonHeight, buttonWidth, actionButtonHeight);
    buttonList.push_back(new Clickable(waterBushRect, 5, "Water Bush"));

    Rect fertiliseBushRect(WINDOW_WIDTH-buttonWidth, 1 * actionButtonHeight, buttonWidth, actionButtonHeight);
    buttonList.push_back(new Clickable(fertiliseBushRect, 6, "Fertilise Bush"));

    Rect trimHedgeRect(WINDOW_WIDTH-buttonWidth, 2 * actionButtonHeight, buttonWidth, actionButtonHeight);
    buttonList.push_back(new Clickable(trimHedgeRect, 7, "Trim Hedge"));

    Rect growBushRect(WINDOW_WIDTH-buttonWidth, 3 * actionButtonHeight, buttonWidth, actionButtonHeight);
    buttonList.push_back(new Clickable(growBushRect, 8, "Grow Bush"));

    Rect undoActionRect(WINDOW_WIDTH-buttonWidth, 4 * actionButtonHeight, buttonWidth, actionButtonHeight);
    buttonList.push_back(new Clickable(undoActionRect, 9, "Undo Action"));

    Rect saveGameButtonRect(140, 10, 120, 50); // save game button
    saveGameButton = new Clickable(saveGameButtonRect, 10, "Save");

    // window text
    namedWindow("Bush Growing Simulator", 0); 
    setMouseCallback("Bush Growing Simulator", Game::handleMouseClick); 

    //supplies info 
    cout << "You have " << gamePlayer->getWaterSupply() << "L of water" << endl;
    cout << "You have " << gamePlayer->getFertiliserSupply() << "kg of fertiliser" << endl;

}

// used to show and hide buttons on screen
Game::~Game(){
    //based on what screen user in, removes and adds buttons from mem
    for(int i = 0; i < buttonList.size(); i++){
        delete buttonList[i];
    }

    //free up screen memory
    delete screenImg;
    delete gameTree;
    delete gamePlayer;
    delete gameTimeline;
    delete saveGameButton; 
    delete loadGameButton; 
    if (berriesMenuButton) {
        delete berriesMenuButton;
        berriesMenuButton = nullptr; 
    }
}

// actual draw const 
void Game::drawTextLines(cv::Mat* targetImg, const std::vector<std::string>& lines, int x, int startY, int lineSpacing, int fontFace, double fontScale, const cv::Scalar& color, int thickness) {
    for (size_t i = 0; i < lines.size(); ++i) {
        cv::Point textOrg(x, startY + i * lineSpacing);
        cv::putText(*targetImg, lines[i], textOrg, fontFace, fontScale, color, thickness);
    }
}

void Game::drawInGameBackgroundUI() {
    //drwa back button
    buttonList[2]->draw(screenImg);

    //draw action button
    for(int i = 4; i < buttonList.size(); i++){ 
        buttonList[i]->draw(screenImg);
    }

    // Draw Save Game button
    if (saveGameButton) { 
         saveGameButton->draw(screenImg);
    }

    //Draws the bush to the screen
    if (gameTree) {
        gameTree->draw(screenImg);

        // draw fruits
        const std::vector<Fruit>& fruits = gameTree->getFruitsList();
        for (const Fruit& fruit : fruits) {
            if (!fruit.collected) { 
                cv::circle(*screenImg, fruit.position, static_cast<int>(fruit.radius), fruit.color, -1); // -1 for filled circle
            }
        }
    }

    // ------------------- Display Player Stats
    if (gamePlayer) {
        // fruit counter
        std::string fruitCounterText = "Fruits: R:" + std::to_string(redFruitsCollectedCount) +
                                     " B:" + std::to_string(blueFruitsCollectedCount) +
                                     " G:" + std::to_string(goldFruitsCollectedCount);
        cv::putText(*screenImg, fruitCounterText, cv::Point(10, WINDOW_HEIGHT - 55), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);

        std::string waterText = "Water: " + std::to_string(static_cast<int>(gamePlayer->getWaterSupply())) + "L";
        std::string fertText = "Fertiliser: " + std::to_string(static_cast<int>(gamePlayer->getFertiliserSupply())) + "kg";
        
        cv::putText(*screenImg, waterText, cv::Point(10, WINDOW_HEIGHT - 35), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);
        cv::putText(*screenImg, fertText, cv::Point(10, WINDOW_HEIGHT - 15), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);
    }
}

void Game::drawScreen(){
    //Clears what was previously on the screen by drawing a vertical gradient
    Scalar startColor = CV_RGB(135, 206, 250); // sky background
    Scalar endColor = CV_RGB(70, 130, 180);   

    int width = screenImg->cols;
    int height = screenImg->rows;

    // draws game screen
    for (int y = 0; y < height; ++y) {
        float t = static_cast<float>(y) / height;

        int r = static_cast<int>(startColor.val[2] * (1.0f - t) + endColor.val[2] * t);
        int g = static_cast<int>(startColor.val[1] * (1.0f - t) + endColor.val[1] * t);
        int b = static_cast<int>(startColor.val[0] * (1.0f - t) + endColor.val[0] * t);

        cv::line(*screenImg, Point(0, y), Point(width - 1, y), CV_RGB(r, g, b), 1);
    }

// MAIN MENU SCREEN --------------
    switch(currentState) {
    case MAIN_MENU:

        { // title
            std::string titleText = "Bush Growing simulator";
            int fontFace = cv::FONT_HERSHEY_TRIPLEX;
            double fontScale = 1.2;
            int thickness = 2; 
            int baseline = 0;
            cv::Size textSize = cv::getTextSize(titleText, fontFace, fontScale, thickness, &baseline);
            
            // x-coordinate to center the text
            cv::Point textOrg((WINDOW_WIDTH - textSize.width) / 2, 80); 

            // Draw text
            cv::putText(*screenImg, titleText, textOrg, fontFace, fontScale, cv::Scalar(0,0,0), thickness + 2); // Outline
            cv::putText(*screenImg, titleText, textOrg, fontFace, fontScale, cv::Scalar(255,255,255), thickness); // Fill
        }

        { // my name at bottom right
            std::string projectText = "OOP Project - Bush Growing Simulator"; 
            int projectFontFace = cv::FONT_HERSHEY_SIMPLEX;
            double projectFontScale = 0.4; 
            int projectThickness = 1;
            int projectBaseline = 0;
            cv::Size projectTextSize = cv::getTextSize(projectText, projectFontFace, projectFontScale, projectThickness, &projectBaseline);
            cv::Point projectTextOrg(WINDOW_WIDTH - projectTextSize.width - 10, WINDOW_HEIGHT - 10); 
            cv::putText(*screenImg, projectText, projectTextOrg, projectFontFace, projectFontScale, cv::Scalar(0,0,0), projectThickness + 1); 
            cv::putText(*screenImg, projectText, projectTextOrg, projectFontFace, projectFontScale, cv::Scalar(255,255,255), projectThickness);
        }

        buttonList[0]->draw(screenImg);
        buttonList[1]->draw(screenImg);

        //load game button draw
        if (loadGameButton) { 
            loadGameButton->draw(screenImg);
        }
        //Draws the berries button
        if (berriesMenuButton) { 
            berriesMenuButton->draw(screenImg);
        }
        break;


        // INSTRUCTION SCREEN ---------------------------------

    case INSTRUCTION_MENU:
        //Draws the back button
        buttonList[2]->draw(screenImg);

        { // Scope for title
            std::string titleText = "Instructions";
            int titleFontFace = cv::FONT_HERSHEY_TRIPLEX;
            double titleFontScale = 1.0;
            int titleThickness = 2;
            int baseline = 0;
            cv::Size textSize = cv::getTextSize(titleText, titleFontFace, titleFontScale, titleThickness, &baseline);
            cv::Point titleOrg((WINDOW_WIDTH - textSize.width) / 2, 80); 
            cv::putText(*screenImg, titleText, titleOrg, titleFontFace, titleFontScale, cv::Scalar(0,0,0), titleThickness);
        }

        { // Start new scope block for instruction lines
            int instructionFont = cv::FONT_HERSHEY_DUPLEX; 
            double instructionFontScale = 0.8;
            int instructionThickness = 1;      
            
            int instructionXPos = WINDOW_WIDTH/2-300;
            int instructionStartY = 180;
            int lineSpacing = 50;

            std::vector<std::string> instructionTexts = {
                "Water and fertilise your bush so it grows.",
                "Collect berries, some are rare!!. Do you",
                "want to remove and reverse your previous",
                "actions if you make a mistake or don't",
                "like how your bush has grown. You get free",
                "water and fertiliser every turn, make use.",
                "you let your bush grow. Press ESC to quit"
            };
            drawTextLines(screenImg, instructionTexts, instructionXPos, instructionStartY, lineSpacing, instructionFont, instructionFontScale, Scalar(0, 0, 0), instructionThickness);
        } 
        break;
    case PRUNING_ACTION:
        this->drawInGameBackgroundUI(); // draw the common buttons
        buttonList[3]->draw(screenImg); // cancle button
        break; 

    case IN_GAME:
        this->drawInGameBackgroundUI();
        break;

    case BERRIES_MENU:
        { 
            // --- Draw Title ---
            std::string titleText = "Berry Rarity Information";
            int titleFontFace = cv::FONT_HERSHEY_TRIPLEX; 
            double titleFontScale = 1.0;
            int titleThickness = 2;
            cv::Size titleTextSize = cv::getTextSize(titleText, titleFontFace, titleFontScale, titleThickness, nullptr);
            cv::Point titleOrg((WINDOW_WIDTH - titleTextSize.width) / 2, 80); // Y=80 for title
            cv::putText(*screenImg, titleText, titleOrg, titleFontFace, titleFontScale, cv::Scalar(0,0,0), titleThickness);

            // --- Draw Rarity Info ---
            int infoFont = cv::FONT_HERSHEY_DUPLEX; 
            double infoFontScale = 0.7;
            int infoThickness = 1;
            cv::Scalar infoColor = cv::Scalar(0,0,0); 

            int startY = titleOrg.y + titleTextSize.height + 50; 
            int lineHeight = 35; 

            // Centering text lines,
            std::vector<std::string> berryInfoTexts = {
                "Red Fruit: Common (Approx. 70% spawn rate)",
                "Blue Fruit: Uncommon (Approx. 25% spawn rate)",
                "Gold Fruit: Rare (Approx. 5% spawn rate)"
            };

            cv::Size approxMaxTextSize = cv::getTextSize(berryInfoTexts[1], infoFont, infoFontScale, infoThickness, nullptr);
            int infoXPos = (WINDOW_WIDTH - approxMaxTextSize.width) / 2;
            if (infoXPos < 0) infoXPos = 10; 

            drawTextLines(screenImg, berryInfoTexts, infoXPos, startY, lineHeight, infoFont, infoFontScale, infoColor, infoThickness);
            
            // --- Draw Back Button ---
            if (buttonList.size() > 2 && buttonList[2] != nullptr) {
                 buttonList[2]->draw(screenImg);
            }
        }
        break;

    case AWAITING_WATER_INPUT:
    case AWAITING_FERTILISER_INPUT:
        { 
            this->drawInGameBackgroundUI();

            cv::Mat overlay = screenImg->clone(); // draw semi transparent display
            cv::rectangle(overlay, cv::Rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), cv::Scalar(0, 0, 0), -1);
            cv::addWeighted(overlay, 0.5, *screenImg, 0.5, 0, *screenImg);
            cv::Rect inputBoxRect(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 50, 300, 100);
            cv::rectangle(*screenImg, inputBoxRect, CV_RGB(220, 220, 220), -1); 
            cv::rectangle(*screenImg, inputBoxRect, CV_RGB(0, 0, 0), 2);    
            std::string prompt = (currentState == AWAITING_WATER_INPUT) ? "Amount to Water:" : "Amount to Fertilise:";
            cv::putText(*screenImg, prompt, cv::Point(inputBoxRect.x + 10, inputBoxRect.y + 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, CV_RGB(0,0,0), 2);
            std::string displayText = currentInputText + "|";
            cv::putText(*screenImg, displayText, cv::Point(inputBoxRect.x + 10, inputBoxRect.y + 70), cv::FONT_HERSHEY_SIMPLEX, 0.7, CV_RGB(50,50,50), 2);
        }
        break;
    }

    if (!transientMessage.empty()) { //messages for undo and load and save
        double currentTime = static_cast<double>(cv::getTickCount()) / cv::getTickFrequency();
        if (currentTime - transientMessageStartTime < transientMessageDurationSeconds) {
            // Display the message
            cv::Size textSize = cv::getTextSize(transientMessage, cv::FONT_HERSHEY_SIMPLEX, 1.0, 2, nullptr);
            cv::Point textOrg((WINDOW_WIDTH - textSize.width) / 2, WINDOW_HEIGHT / 2 + textSize.height / 2);
            // Draw a semi-transparent background
            cv::Rect textBackgroundRect(
                std::max(0, textOrg.x - 10), 
                std::max(0, textOrg.y - textSize.height - 10), 
                std::min(WINDOW_WIDTH - (textOrg.x - 10), textSize.width + 20), 
                std::min(WINDOW_HEIGHT - (textOrg.y - textSize.height - 10), textSize.height + 20)
            );

            if (textBackgroundRect.width > 0 && textBackgroundRect.height > 0) {
                 cv::Mat roi = (*screenImg)(textBackgroundRect);
                 cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 0)); 
                 double alpha = 0.3;
                 cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi); 
            }

            cv::putText(*screenImg, transientMessage, textOrg, cv::FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(255, 255, 255), 2); 
        } else {
            // clear the message after a while
            transientMessage = "";
        }
    }

    cv::imshow("Bush Growing Simulator", *screenImg); 
}

void Game::handleMouseClick(int event, int mouseX, int mouseY, int , void*){
    //check if mouse was clickjed
    if(event == EVENT_LBUTTONDOWN){
        //Sets variables appropriately
        Game::mouseXPos = mouseX;
        Game::mouseYPos = mouseY;
        Game::mouseClicked = true;


    }
}

void Game::handleInputs(int keyPressed){ 
    
    // Handle mouse clicks first
    if(Game::mouseClicked){
        Point mousePos(Game::mouseXPos, Game::mouseYPos);
        Game::mouseClicked = false; 

        // Mouse click handling 
        if (currentState != AWAITING_WATER_INPUT && currentState != AWAITING_FERTILISER_INPUT) {
            int prunedIndex = gameTree->getClickedIndex(mousePos.x, mousePos.y);

            switch(currentState) { // this handles all of the visual mecanics of state swtiching, shows/hides per screen
                case MAIN_MENU:
                    if(buttonList[0]->contains(mousePos)){ buttonList[0]->isPressed = true; currentState = IN_GAME; }
                    else if(buttonList[1]->contains(mousePos)){ buttonList[1]->isPressed = true; currentState = INSTRUCTION_MENU; }
                    else if (loadGameButton && loadGameButton->contains(mousePos)) { loadGameButton->isPressed = true; loadGame(); }
                    else if (berriesMenuButton && berriesMenuButton->contains(mousePos)) {
                        berriesMenuButton->isPressed = true;
                        currentState = BERRIES_MENU;
                    }
                    break;
                case INSTRUCTION_MENU:
                    if(buttonList[2]->contains(mousePos)){ buttonList[2]->isPressed = true; currentState = MAIN_MENU; }
                    break;
                case BERRIES_MENU:
                    if (buttonList[2] && buttonList[2]->contains(mousePos)) { 
                        buttonList[2]->isPressed = true;
                        currentState = MAIN_MENU;
                    }
                    break; 
                case PRUNING_ACTION:
                    if(buttonList[3]->contains(mousePos)){ buttonList[3]->isPressed = true; currentState = IN_GAME; } // Cancel pruning
                    else if (prunedIndex != -1) {
            
                        gameTimeline->performAction(new PruningAction(gameTree, prunedIndex));
                        currentState = IN_GAME;
                    } else if (saveGameButton && saveGameButton->contains(mousePos)) {
                        saveGameButton->isPressed = true;
                        saveGame();
                    }
                    break;
                case IN_GAME:
                    { 
                        FruitType collectedType;
                        int collectedId;
                        if (gameTree && gameTree->collectFruitAtPoint(mousePos, collectedType, collectedId)) {
                            
                            if (collectedType == FruitType::RED) {
                                redFruitsCollectedCount++;
                            } else if (collectedType == FruitType::BLUE) {
                                blueFruitsCollectedCount++;
                            } else if (collectedType == FruitType::GOLD) {
                                goldFruitsCollectedCount++;
                            }
                          
                        } else if (buttonList[2]->contains(mousePos)) { 
                            buttonList[2]->isPressed = true;
                            currentState = MAIN_MENU;
                        } else if (saveGameButton && saveGameButton->contains(mousePos)) {
                            std::cout << "DEBUG: Save Game button clicked in IN_GAME state." << std::endl;
                            saveGameButton->isPressed = true;
                            saveGame();
                        } else if (buttonList[4]->contains(mousePos)) { 
                            buttonList[4]->isPressed = true;
                            currentState = AWAITING_WATER_INPUT;
                            pendingActionType = 5;
                            currentInputText = "";
                           
                            return; 
                        } else if (buttonList[5]->contains(mousePos)) { 
                            buttonList[5]->isPressed = true;
                            currentState = AWAITING_FERTILISER_INPUT;
                            pendingActionType = 6;
                            currentInputText = "";
                           
                            return; 
                        } else if (buttonList[6]->contains(mousePos)) { // Prune branch
                            buttonList[6]->isPressed = true;
                            currentState = PRUNING_ACTION;
                        } else if (buttonList[7]->contains(mousePos)) { // Grow tree
                            buttonList[7]->isPressed = true;
                            gameTimeline->performAction(new GrowingAction(gamePlayer, gameTree));
                        } else if (buttonList[8]->contains(mousePos)) { // Reverse action
                            buttonList[8]->isPressed = true;
                            gameTimeline->reverseAction();
                        }
                    
                    }
                    break; 
                default: 
                    break;
            }
        }
    }

    // Handle keyboard input
    if (keyPressed != -1) { // see if key pressed
        if (currentState == AWAITING_WATER_INPUT || currentState == AWAITING_FERTILISER_INPUT) {
            if (keyPressed >= '0' && keyPressed <= '9') {
                currentInputText += static_cast<char>(keyPressed);
            } else if (keyPressed == '.' && currentInputText.find('.') == std::string::npos) { //allows decimal point
                currentInputText += '.';
            } else if (keyPressed == 8) { // Backspace
                if (!currentInputText.empty()) {
                    currentInputText.pop_back();
                }
            } else if (keyPressed == 13) { // Enter
                if (!currentInputText.empty()) {
                    try {
                        float amount = std::stof(currentInputText);
                        if (amount > 0) { // Ensure positive amount
                            if (pendingActionType == 5) { // Water
                                std::cout << "Watering tree with " << amount << "L from input." << std::endl;
                                gameTimeline->performAction(new WateringAction(gamePlayer, gameTree, amount));
                            } else if (pendingActionType == 6) { // Fertilise
                                std::cout << "Fertilising tree with " << amount << "kg from input." << std::endl;
                                gameTimeline->performAction(new FertilisingAction(gamePlayer, gameTree, amount));
                            }
                        } else {
                             std::cout << "Amount must be positive." << std::endl;
                        }
                    } catch (const std::invalid_argument& ia) {
                        std::cerr << "Invalid input: Not a number." << std::endl;
                    } catch (const std::out_of_range& oor) {
                        std::cerr << "Invalid input: Number too large." << std::endl;
                    }
                }
                currentState = IN_GAME;
                currentInputText = "";
                pendingActionType = 0;
            } else if (keyPressed == 27) { // Escape
                currentState = IN_GAME;
                currentInputText = "";
                pendingActionType = 0;
            }
        }
        
    }
}
// printing the data (testing)
void Game::printData(){
    cout << "Game object" << endl;
    gameTree->printData();
    gamePlayer->printData();
    gameTimeline->printData();

    cout << "Window width: " << WINDOW_WIDTH << endl;
    cout << "Window height: " << WINDOW_HEIGHT << endl;

    cout << "Game state: " << currentState << endl;
}

//saving the game  + ERRORS
void Game::saveGame() {
    std::ofstream saveFile("savegame.txt");
    if (!saveFile.is_open()) {
        std::cerr << "Error cant open savegame" << std::endl;
        return;
    }

    // Save Player data
    if (gamePlayer) {
        gamePlayer->saveToStream(saveFile);
    } else {
        std::cerr << "error player data is 0 cant svae" << std::endl;
    }

    // Save Tree data
    if (gameTree) {
        gameTree->saveToStream(saveFile);
    } else {
        std::cerr << "ErrorCannot save tree data." << std::endl;
    }

    saveFile.close();

    if (saveFile.good()) { 
        std::cout << "Game saved successfully to savegame.txt" << std::endl;
        transientMessage = "Game Saved!!!!!";
        transientMessageStartTime = static_cast<double>(cv::getTickCount()) / cv::getTickFrequency();
        transientMessageDurationSeconds = 3.0; // Display for 3 seconds
    } else {
        std::cerr << "error writing to savegame.txt." << std::endl;
    }
}

void Game::loadGame() {
    std::ifstream loadFile("savegame.txt");
    if (!loadFile.is_open()) {
        std::cerr << "Error: Could not open savegame.txt for reading. Starting new game." << std::endl;
        return;
    }

    // --- Clean up existing game state before loading ---
    // Delete old tree, player, and timeline
    if (gameTree) {
        delete gameTree;
        gameTree = nullptr;
    }
    if (gamePlayer) {
        delete gamePlayer;
        gamePlayer = nullptr;
    }
    if (gameTimeline) {
        delete gameTimeline;
        gameTimeline = nullptr; 
    }
    
    // Recreate timeline
    gameTimeline = new Timeline();

    // --- Load Player data ---
    Player loadedPlayer = Player::loadFromStream(loadFile);
    if (loadFile.fail()) { // check erros
        std::cerr << "Error loading player data from savegame.txt. Starting new game." << std::endl;
        loadFile.close();
        // use default if not working
        gamePlayer = new Player(10, 5); // Default player
        gameTree = new Tree(10, 10, new Branch(0, 0, 1, 50, 10, WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT)); // Default tree
        currentState = MAIN_MENU; // Stay on main menu or go to new game
        return;
    }
    gamePlayer = new Player(loadedPlayer); // heap copy

    // --- Load Tree data ---

    Tree* loadedTreePtr = Tree::loadFromStream(loadFile, WINDOW_WIDTH, WINDOW_HEIGHT); 
    if (!loadedTreePtr || loadFile.fail()) { 
        std::cerr << "Error loading tree data from savegame.txt. Starting new game with loaded player." << std::endl;
        loadFile.close();
        gameTree = new Tree(10, 10, new Branch(0, 0, 1, 50, 10, WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT)); 
        currentState = MAIN_MENU; // Stay on main menu or go to new game
        return;
    }
    gameTree = loadedTreePtr; // Assign the loaded tree

    loadFile.close();
    std::cout << "Game loaded successfully from savegame.txt" << std::endl;

    transientMessage = "Game Loaded!";
    transientMessageStartTime = static_cast<double>(cv::getTickCount()) / cv::getTickFrequency(); // Get current time in seconds
    transientMessageDurationSeconds = 3.0; // Display for 3 seconds

    currentState = IN_GAME; // Transition to game
}