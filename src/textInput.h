//
//  textInput.h
//  textInput
//
//  Created by Elliot Woods on 12/09/2011.
//  Copyright 2011 Kimchi and Chips.
//	MIT license
//	http://www.opensource.org/licenses/mit-license.php
//

#include "ofMain.h"
#include "ofEvents.h"

class textInput {
public:
    textInput();
   
    void        init();
    void        draw(int tPosX, int tPosY, bool _isSelected) const;
    void        keyPressed(int key);
    void        update(bool isSelected);
    void        clear();
    
    string      text;
    int         position;
    int         cursorx, cursory;
    int         debugCursor; //Weird debug of left/right keys
    bool        localSelected;
    ofEvent<string> evtEnter;
    
protected:
    void        keyPressedEvent(ofKeyEventArgs &a);
};
