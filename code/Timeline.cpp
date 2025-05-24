#include "Timeline.h"

Timeline::Timeline(){};

Timeline::~Timeline(){
    for (int i = 0; i < listOfActions.size(); i++){
        //Deallocates the memory storing each action in he timeline
        delete listOfActions[i];
    }
}

void Timeline::performAction(Action* actionToPerform){
    //Adds the action to the list
    listOfActions.push_back(actionToPerform);
    //Performs the action immediately
    actionToPerform->performAction();
}

void Timeline::reverseAction(){
    if(listOfActions.size()>0){
        //Reverses the last action that was taken
        listOfActions.back()->reverseAction();
        //Deallocates the memory
        delete listOfActions.back();
        //Deletes the pointer from the vector
        listOfActions.pop_back();
    }else{
        cout << "You can't travel back before the beginning of time" << endl;
    }
}

void Timeline::printData(){
    cout << "Timeline of all actions" << endl;
    cout << "List of all actions: " << endl; 
    for (int i = 0; i < listOfActions.size(); i++){
        cout << listOfActions[i] << ", "; 
    } 
    cout << endl;
}
