#ifndef ACTION_H
#define ACTION_H


class Action {
    public:
        virtual bool performAction() = 0;
        virtual void reverseAction() = 0;
};

#endif