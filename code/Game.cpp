#include "Game.h"
#include <fstream> // For std::ofstream
// Removed: #include "include/nlohmann/json.hpp" // For JSON serialization

//Sets static variables
int Game::mouseXPos = 0;
int Game::mouseYPos = 0;
bool Game::mouseClicked = false;

Game::Game(int windowWidth, int windowHeight) : currentState(MAIN_MENU){
    WINDOW_WIDTH = windowWidth;
    WINDOW_HEIGHT = windowHeight;
    currentInputText = "";
    pendingActionType = 0; // 0 for None

    transientMessage = "";
    transientMessageDurationSeconds = 0.0;
    transientMessageStartTime = 0.0;

    redFruitsCollectedCount = 0;
    blueFruitsCollectedCount = 0;
    goldFruitsCollectedCount = 0;

    gamePlayer = new Player(10, 5);

    //Creates tree with a default supply of 10L of water and 10kg of nutrients
    gameTree = new Tree(10, 10, new Branch(0, 0, 1, 50, 10,  WINDOW_WIDTH/ 2, WINDOW_HEIGHT));


    //Creates the image to display to the screen
    screenImg = new Mat(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3, CV_RGB(150, 150, 255));

    //Creates the timeline
    gameTimeline = new Timeline();

    int buttonWidth = 250; // Used for other buttons, main menu buttons are 200 wide

    // Main Menu Buttons - Adjusted Layout
    int mainMenuButtonWidth = 200;
    int mainMenuButtonHeight = 80;
    int mainMenuButtonX = WINDOW_WIDTH / 2 - mainMenuButtonWidth / 2; // Centered
    int titleBottomMargin = 120; // Assuming title + padding takes up to Y=120
    int buttonGap = 15;

    int playButtonY = titleBottomMargin + buttonGap; // First button starts after title area
    Rect mainMenuButtonRect(mainMenuButtonX, playButtonY, mainMenuButtonWidth, mainMenuButtonHeight);
    buttonList.push_back(new Clickable(mainMenuButtonRect, 1, "Play"));

    int instructionsButtonY = playButtonY + mainMenuButtonHeight + buttonGap;
    Rect instructionMenuButtonRect(mainMenuButtonX, instructionsButtonY, mainMenuButtonWidth, mainMenuButtonHeight);
    buttonList.push_back(new Clickable(instructionMenuButtonRect, 2, "Instructions"));
    
    // Instantiate Load Game button for Main Menu - Adjusted
    int loadGameButtonY = instructionsButtonY + mainMenuButtonHeight + buttonGap;
    Rect loadGameButtonRect(mainMenuButtonX, loadGameButtonY, mainMenuButtonWidth, mainMenuButtonHeight); 
    loadGameButton = new Clickable(loadGameButtonRect, 11, "Load Game");


    // Other Buttons (Back, Cancel, Actions, Save)
    Rect backButtonRect(10, 10, 120, 50); // Reduced size: Width 120, Height 50
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
    Rect saveGameButtonRect(140, 10, 120, 50); // Repositioned next to Back button
    saveGameButton = new Clickable(saveGameButtonRect, 10, "Save");

    // Instantiate Load Game button for Main Menu - MOVED UP AND ADJUSTED

    namedWindow("Time Travel Tree", 0); // Restored GUI call
    setMouseCallback("Time Travel Tree", Game::handleMouseClick); // Restored GUI call

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
        { // Block to scope variables for title
            std::string titleText = "Bush trimming simulator";
            int fontFace = cv::FONT_HERSHEY_TRIPLEX;
            double fontScale = 1.2;
            int thickness = 2;
            int baseline = 0;
            cv::Size textSize = cv::getTextSize(titleText, fontFace, fontScale, thickness, &baseline);
            
            // Calculate x-coordinate to center the text
            cv::Point textOrg((WINDOW_WIDTH - textSize.width) / 2, 80); // Y set to 80 pixels from top

            cv::putText(*screenImg, titleText, textOrg, fontFace, fontScale, cv::Scalar(0,0,0), thickness); // Black color
        }
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

        { // Start new scope block
            //Explains the instructions
            int instructionFont = cv::FONT_HERSHEY_DUPLEX; // Changed font
            double instructionFontScale = 0.8; // Slightly reduced scale for DUPLEX to fit well
            int instructionThickness = 1;      // Adjusted thickness for DUPLEX

            putText(*screenImg, "Water and fertilise your tree so it grows", Point(WINDOW_WIDTH/2-300, 150), instructionFont, instructionFontScale, Scalar(0, 0, 0), instructionThickness);
            putText(*screenImg, "big and tall. Prune branches that you", Point(WINDOW_WIDTH/2-300, 200), instructionFont, instructionFontScale, Scalar(0, 0, 0), instructionThickness);
            putText(*screenImg, "want to remove and reverse your previous", Point(WINDOW_WIDTH/2-300, 250), instructionFont, instructionFontScale, Scalar(0, 0, 0), instructionThickness);
            putText(*screenImg, "actions if you make a mistake or don't", Point(WINDOW_WIDTH/2-300, 300), instructionFont, instructionFontScale, Scalar(0, 0, 0), instructionThickness);
            putText(*screenImg, "like how the tree has grown. You get 2L", Point(WINDOW_WIDTH/2-300, 350), instructionFont, instructionFontScale, Scalar(0, 0, 0), instructionThickness);
            putText(*screenImg, "water and 1kg fertiliser free every time", Point(WINDOW_WIDTH/2-300, 400), instructionFont, instructionFontScale, Scalar(0, 0, 0), instructionThickness);
            putText(*screenImg, "you let your tree grow. Press ESC to quit", Point(WINDOW_WIDTH/2-300, 450), instructionFont, instructionFontScale, Scalar(0, 0, 0), instructionThickness);
        } // End new scope block
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

        // Draw fruits
        if (gameTree) { // Ensure gameTree is not null
            const std::vector<Fruit>& fruits = gameTree->getFruitsList();
            for (const Fruit& fruit : fruits) {
                if (!fruit.collected) { // Only draw if not collected
                    cv::circle(*screenImg, fruit.position, static_cast<int>(fruit.radius), fruit.color, -1); // -1 for filled circle
                    // Optional: Add a border to the fruit
                    // cv::circle(*screenImg, fruit.position, static_cast<int>(fruit.radius), CV_RGB(0,0,0), 1); // Black border, 1px thick
                }
            }
        }

        // Display Player Stats
        if (gamePlayer) {
            std::string waterText = "Water: " + std::to_string(static_cast<int>(gamePlayer->getWaterSupply())) + "L";
            std::string fertText = "Fertiliser: " + std::to_string(static_cast<int>(gamePlayer->getFertiliserSupply())) + "kg";
            
            cv::putText(*screenImg, waterText, cv::Point(10, WINDOW_HEIGHT - 35), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);
            cv::putText(*screenImg, fertText, cv::Point(10, WINDOW_HEIGHT - 15), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);

            // Display Fruit Counters
            std::string redText = "Red Fruits: " + std::to_string(redFruitsCollectedCount);
            std::string blueText = "Blue Fruits: " + std::to_string(blueFruitsCollectedCount);
            std::string goldText = "Gold Fruits: " + std::to_string(goldFruitsCollectedCount);
            int statYStart = WINDOW_HEIGHT - 15; // Y where last player stat ended
            cv::putText(*screenImg, redText,  cv::Point(10, statYStart + 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);
            cv::putText(*screenImg, blueText, cv::Point(10, statYStart + 40), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);
            cv::putText(*screenImg, goldText, cv::Point(10, statYStart + 60), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);
        }
        break;

    case AWAITING_WATER_INPUT:
    case AWAITING_FERTILISER_INPUT:
        { // Block to scope variables
            // Draw the underlying IN_GAME screen elements first
            // (This assumes IN_GAME case has drawn them, or we explicitly call a common draw function)
            // For simplicity, we'll re-draw necessary IN_GAME elements or rely on previous frame's partial draw
            // A better way would be to have a common function `drawInGameElements()`
            // For now, the IN_GAME case already draws the tree and buttons if we fall through.
            // However, the switch structure needs a break for IN_GAME, so we might need to duplicate drawing logic slightly or refactor.
            // Let's assume IN_GAME elements are drawn if we don't clear screenImg again.
            // The current structure clears screenImg at the top, then draws based on state.
            // So, for the modal, we draw IN_GAME first, then the modal.
            // To achieve this, we can call drawScreen recursively with IN_GAME state, or duplicate drawing logic.
            // For this task, let's just draw the modal on top of what IN_GAME would draw.
            // The main game elements (tree, buttons) will be drawn by the IN_GAME case if we fall through.
            // However, the switch statement structure means only one case is executed.

            // Re-draw the IN_GAME state as background for the pop-up
            // This is a simplified approach. A more robust solution might use a dedicated function.
            buttonList[2]->draw(screenImg); // Back button
            for(int i = 4; i < buttonList.size(); i++){ buttonList[i]->draw(screenImg); } // Action buttons
            if (saveGameButton) { saveGameButton->draw(screenImg); }
            if (gameTree) { 
                gameTree->draw(screenImg); 
                // Also draw fruits in the pop-up background
                const std::vector<Fruit>& fruits = gameTree->getFruitsList();
                for (const Fruit& fruit : fruits) {
                    if (!fruit.collected) {
                        cv::circle(*screenImg, fruit.position, static_cast<int>(fruit.radius), fruit.color, -1);
                    }
                }
            }
             if (gamePlayer) { // Also draw stats
                std::string waterText = "Water: " + std::to_string(static_cast<int>(gamePlayer->getWaterSupply())) + "L";
                std::string fertText = "Fertiliser: " + std::to_string(static_cast<int>(gamePlayer->getFertiliserSupply())) + "kg";
                cv::putText(*screenImg, waterText, cv::Point(10, WINDOW_HEIGHT - 35), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);
                cv::putText(*screenImg, fertText, cv::Point(10, WINDOW_HEIGHT - 15), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);

                // Display Fruit Counters
                std::string redText = "Red Fruits: " + std::to_string(redFruitsCollectedCount);
                std::string blueText = "Blue Fruits: " + std::to_string(blueFruitsCollectedCount);
                std::string goldText = "Gold Fruits: " + std::to_string(goldFruitsCollectedCount);
                int statYStart = WINDOW_HEIGHT - 15; // Y where last player stat ended
                cv::putText(*screenImg, redText,  cv::Point(10, statYStart + 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);
                cv::putText(*screenImg, blueText, cv::Point(10, statYStart + 40), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);
                cv::putText(*screenImg, goldText, cv::Point(10, statYStart + 60), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,0), 2);
            }

            // Draw a semi-transparent overlay
            cv::Mat overlay = screenImg->clone();
            cv::rectangle(overlay, cv::Rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), cv::Scalar(0, 0, 0), -1);
            cv::addWeighted(overlay, 0.5, *screenImg, 0.5, 0, *screenImg);
            
            cv::Rect inputBoxRect(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 50, 300, 100);
            cv::rectangle(*screenImg, inputBoxRect, CV_RGB(220, 220, 220), -1); // Light grey Box background
            cv::rectangle(*screenImg, inputBoxRect, CV_RGB(0, 0, 0), 2);      // Box border

            std::string prompt = (currentState == AWAITING_WATER_INPUT) ? "Amount to Water:" : "Amount to Fertilise:";
            cv::putText(*screenImg, prompt, cv::Point(inputBoxRect.x + 10, inputBoxRect.y + 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, CV_RGB(0,0,0), 2);
            
            // Display currentInputText with a cursor placeholder (e.g., '|')
            std::string displayText = currentInputText + "|";
            cv::putText(*screenImg, displayText, cv::Point(inputBoxRect.x + 10, inputBoxRect.y + 70), cv::FONT_HERSHEY_SIMPLEX, 0.7, CV_RGB(50,50,50), 2);
        }
        break;
    }

    if (!transientMessage.empty()) {
        double currentTime = static_cast<double>(cv::getTickCount()) / cv::getTickFrequency();
        if (currentTime - transientMessageStartTime < transientMessageDurationSeconds) {
            // Display the message
            cv::Size textSize = cv::getTextSize(transientMessage, cv::FONT_HERSHEY_SIMPLEX, 1.0, 2, nullptr);
            cv::Point textOrg((WINDOW_WIDTH - textSize.width) / 2, WINDOW_HEIGHT / 2 + textSize.height / 2); // Centered
            // Draw a semi-transparent background for the text for better visibility
            // Ensure the background rectangle is within the screen boundaries
            cv::Rect textBackgroundRect(
                std::max(0, textOrg.x - 10), 
                std::max(0, textOrg.y - textSize.height - 10), 
                std::min(WINDOW_WIDTH - (textOrg.x - 10), textSize.width + 20), 
                std::min(WINDOW_HEIGHT - (textOrg.y - textSize.height - 10), textSize.height + 20)
            );

            // Check if the background rectangle has valid dimensions
            if (textBackgroundRect.width > 0 && textBackgroundRect.height > 0) {
                 cv::Mat roi = (*screenImg)(textBackgroundRect);
                 cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 0)); 
                 double alpha = 0.3;
                 cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi); 
            }
            
            // Put the text
            cv::putText(*screenImg, transientMessage, textOrg, cv::FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(255, 255, 255), 2); // White text
        } else {
            // Time expired, clear the message
            transientMessage = "";
        }
    }

    cv::imshow("Time Travel Tree", *screenImg); // Restored GUI call
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

void Game::handleInputs(int keyPressed){ // Signature already changed in Game.h
    
    // Handle mouse clicks first, as they are independent of key presses for button interactions
    if(Game::mouseClicked){
        Point mousePos(Game::mouseXPos, Game::mouseYPos);
        Game::mouseClicked = false; // Consume the click

        // Mouse click handling should only be active if not in an input state
        if (currentState != AWAITING_WATER_INPUT && currentState != AWAITING_FERTILISER_INPUT) {
            int prunedIndex = gameTree->getClickedIndex(mousePos.x, mousePos.y);

            switch(currentState) {
                case MAIN_MENU:
                    if(buttonList[0]->contains(mousePos)){ buttonList[0]->isPressed = true; currentState = IN_GAME; }
                    else if(buttonList[1]->contains(mousePos)){ buttonList[1]->isPressed = true; currentState = INSTRUCTION_MENU; }
                    else if (loadGameButton && loadGameButton->contains(mousePos)) { loadGameButton->isPressed = true; loadGame(); }
                    break;
                case INSTRUCTION_MENU:
                    if(buttonList[2]->contains(mousePos)){ buttonList[2]->isPressed = true; currentState = MAIN_MENU; }
                    break;
                case PRUNING_ACTION:
                    if(buttonList[3]->contains(mousePos)){ buttonList[3]->isPressed = true; currentState = IN_GAME; } // Cancel pruning
                    else if (prunedIndex != -1) {
                        // No button press here, direct action on tree
                        gameTimeline->performAction(new PruningAction(gameTree, prunedIndex));
                        currentState = IN_GAME;
                    } else if (saveGameButton && saveGameButton->contains(mousePos)) {
                        saveGameButton->isPressed = true;
                        saveGame();
                    }
                    break;
                case IN_GAME:
                    { // Use a block to scope collectedType and collectedId if needed, or declare them outside switch
                        FruitType collectedType;
                        int collectedId;
                        if (gameTree && gameTree->collectFruitAtPoint(mousePos, collectedType, collectedId)) {
                            // std::cout << "INFO: Collected fruit of type " << static_cast<int>(collectedType) << " with ID " << collectedId << std::endl; // Remove/comment this
                            if (collectedType == FruitType::RED) {
                                redFruitsCollectedCount++;
                            } else if (collectedType == FruitType::BLUE) {
                                blueFruitsCollectedCount++;
                            } else if (collectedType == FruitType::GOLD) {
                                goldFruitsCollectedCount++;
                            }
                            // Game::mouseClicked = false; // This is handled by the outer structure
                        } else if (buttonList[2]->contains(mousePos)) { // Back button
                            buttonList[2]->isPressed = true;
                            currentState = MAIN_MENU;
                        } else if (saveGameButton && saveGameButton->contains(mousePos)) {
                            std::cout << "DEBUG: Save Game button clicked in IN_GAME state." << std::endl;
                            saveGameButton->isPressed = true;
                            saveGame();
                        } else if (buttonList[4]->contains(mousePos)) { // Water tree button (ID 5)
                            buttonList[4]->isPressed = true;
                            currentState = AWAITING_WATER_INPUT;
                            pendingActionType = 5;
                            currentInputText = "";
                            // Game::mouseClicked = false; // Not needed due to return
                            return; 
                        } else if (buttonList[5]->contains(mousePos)) { // Fertilise tree button (ID 6)
                            buttonList[5]->isPressed = true;
                            currentState = AWAITING_FERTILISER_INPUT;
                            pendingActionType = 6;
                            currentInputText = "";
                            // Game::mouseClicked = false; // Not needed due to return
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
                        // If a fruit was collected, the click is effectively consumed for other actions
                        // as it was handled in the first 'if' block.
                        // If AWAITING_... states are entered, 'return' exits.
                        // Otherwise, mouseClicked is naturally consumed at the start of handleInputs for the next event.
                    }
                    break; // End of IN_GAME case
                default: 
                    break;
            }
        }
    }

    // Handle keyboard input, especially for AWAITING_... states
    if (keyPressed != -1) { // A key was pressed
        if (currentState == AWAITING_WATER_INPUT || currentState == AWAITING_FERTILISER_INPUT) {
            if (keyPressed >= '0' && keyPressed <= '9') {
                currentInputText += static_cast<char>(keyPressed);
            } else if (keyPressed == '.' && currentInputText.find('.') == std::string::npos) { // Allow one decimal point
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
        // Other global key presses (not related to text input) could be handled here if needed
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
    std::ofstream saveFile("savegame.txt");
    if (!saveFile.is_open()) {
        std::cerr << "Error: Could not open savegame.txt for writing." << std::endl;
        return;
    }

    // Save Player data
    if (gamePlayer) {
        gamePlayer->saveToStream(saveFile);
    } else {
        std::cerr << "Error: gamePlayer object is null. Cannot save player data." << std::endl;
        // Optionally, write placeholder player data or skip
    }

    // Save Tree data
    if (gameTree) {
        gameTree->saveToStream(saveFile);
    } else {
        std::cerr << "Error: gameTree object is null. Cannot save tree data." << std::endl;
        // Optionally, write placeholder tree data or skip
    }

    saveFile.close();
    // Check stream state *before* closing for output streams.
    // However, to match the prompt's style for saveGame, we'll keep the good() check after close.
    // For loadGame, fail() checks are more common during read operations.
    if (saveFile.good()) { 
        std::cout << "Game saved successfully to savegame.txt" << std::endl;
        transientMessage = "Game Saved!";
        transientMessageStartTime = static_cast<double>(cv::getTickCount()) / cv::getTickFrequency(); // Get current time in seconds
        transientMessageDurationSeconds = 3.0; // Display for 3 seconds
    } else {
        std::cerr << "Error writing to savegame.txt." << std::endl;
    }
}

void Game::loadGame() {
    std::ifstream loadFile("savegame.txt");
    if (!loadFile.is_open()) {
        std::cerr << "Error: Could not open savegame.txt for reading. Starting new game." << std::endl;
        // Optionally, initialize a default game state here or let the main menu handle it.
        // For now, just returning will keep the user on the main menu.
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
        gameTimeline = nullptr; // Will be recreated
    }
    
    // Recreate timeline
    gameTimeline = new Timeline();


    // --- Load Player data ---
    Player loadedPlayer = Player::loadFromStream(loadFile);
    if (loadFile.fail()) { // Check for stream errors after trying to load player
        std::cerr << "Error loading player data from savegame.txt. Starting new game." << std::endl;
        loadFile.close();
        // Initialize with default player and tree if player loading fails
        gamePlayer = new Player(10, 5); // Default player
        gameTree = new Tree(10, 10, new Branch(0, 0, 1, 50, 10, WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT)); // Default tree
        currentState = MAIN_MENU; // Stay on main menu or go to new game
        return;
    }
    gamePlayer = new Player(loadedPlayer); // Create a heap-allocated copy

    // --- Load Tree data ---
    // Pass WINDOW_WIDTH and WINDOW_HEIGHT to Tree::loadFromStream for default trunk creation if needed
    Tree* loadedTreePtr = Tree::loadFromStream(loadFile, WINDOW_WIDTH, WINDOW_HEIGHT); 
    if (!loadedTreePtr || loadFile.fail()) { // Check for stream errors or if loadFromStream returned nullptr
        std::cerr << "Error loading tree data from savegame.txt. Starting new game with loaded player." << std::endl;
        loadFile.close();
        // Player data was loaded, but tree failed. Create default tree.
        // gamePlayer is already set from above.
        gameTree = new Tree(10, 10, new Branch(0, 0, 1, 50, 10, WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT)); // Default tree
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