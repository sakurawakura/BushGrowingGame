#ifndef ACTION_H
#define ACTION_H

// Ccontrols the undo or perform feature
class Action {
    public:
        virtual bool performAction() = 0;
        virtual void reverseAction() = 0;
};

#endif