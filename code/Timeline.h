#ifndef TIMELINE_H
#define TIMELINE_H


#include "Printable.h"
#include <vector>
#include "Action.h"


using namespace std;

class Timeline : public Printable{
    public:
        Timeline();
        ~Timeline();

        void performAction(Action* actionToPerform);
        void reverseAction();
        void printData();

    private:
        vector<Action*> listOfActions;
};

#endif