//
//  videoAnnotater
//
//  Created by Matthew Martin on 25/07/14.
//
//

#ifndef videoAnnotater_textObject_h
#define videoAnnotater_textObject_h

#include <iostream>
#include "ofMain.h"
#include "textInput.h"

class textObject {
    
public:
    textObject(int _startTime, int _totalFrame);
    
    void        update(int mX, int mY, int currFrame);
    void        draw(ofTrueTypeFont font);
    void        keyPressed(int key);
    void        mouseReleased(int x, int y);
    void        addText(string& s);
    void        drawEntries();
    
    textInput textBox;

    // Text declarations
    bool        newText; // This is to check if the text is new
    bool        isSelected; // When the mouse has clicked on it
    
    // Typing
    vector<string> textEntries;
    
    // Timer
    int         endTime;
    int         startTime;
    int         tPosX;
    int         tPosY;
    int         lastFrame;
    int         totalFrame;
    
    // Position
    vector<int> posX;
    vector<int> posY;
};

#endif
