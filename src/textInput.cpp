//
//  textInput.cpp
//  textInput
//
//  Created by Elliot Woods on 12/09/2011.
//  Copyright 2011 Kimchi and Chips.
//	MIT license
//	http://www.opensource.org/licenses/mit-license.php
//

#include "textInput.h"

textInput::textInput(){
    text="";
    position=0;
    cursorx=0;
    cursory=0;
    localSelected = true;
    debugCursor = 0;
}

void textInput::init(){
    ofAddListener(ofEvents().keyPressed, this, &textInput::keyPressedEvent);
}

void textInput::draw(int tPosX, int tPosY, bool isSelected)
const {
    ofPushStyle();
    int textLength = text.length();
    (localSelected?ofSetColor(0, 0, 0, 180):ofSetColor(255, 255, 255, 180));
    
    ofRect(tPosX - (textLength*8+10)/2, tPosY - 15, textLength*8+10, 15);

    (localSelected?ofSetColor(255, 255, 255, 255):ofSetColor(0, 0, 0, 255));
    ofDrawBitmapString(text, tPosX - (textLength*8)/2, tPosY-3); // Draw text
  //  font.drawString(text, tPoX - (textLength*8)/2, tPosY-3);
    if(localSelected){
        float timeFrac = 0.5 * sin(6.0f * ofGetElapsedTimef()) + 0.5;
        ofSetColor(255, 255, 255, 255 * timeFrac);
        ofSetLineWidth(2.0f); // Draw cursor line
        ofLine((cursorx*8)/2 + tPosX, 13.7*cursory + tPosY, (cursorx*8)/2 + tPosX, 13.7*cursory + tPosY - 15);
    }
    ofPopStyle();
}

void textInput::keyPressed(int key) {
    // Add character (non unicode sorry!)
    if (key >=32 && key <=126){ 
       // cout << text << endl;
        cout << "HERE " << localSelected << endl;
        text.insert(text.begin()+position, key);
        position++;
    }
    
    if (key==OF_KEY_DEL || key==OF_KEY_BACKSPACE){
        if (position>0){
            text.erase(text.begin()+position-1);
            --position;
        }
    }
    
    if (key==OF_KEY_LEFT){
        if (position > 0){
            --position;
            debugCursor --;
        }
    }
    
    if (key==OF_KEY_RIGHT){
        if (position<text.size()){
            ++position;
            debugCursor ++;
        }
    }
    
    //for multiline:
    // cursorx = cursory = 0;
   // for (int i=0; i<position; ++i){ // Places the cursor in the right place
   //     if (*(text.begin()+i) == '\n'){
   //         ++cursory;
  //          cursorx = 0;
   //     }
   //     else{
   //         cursorx++;
    //    }
   // }
    
    cursorx = position; // Places the cursor in the right place
    cursorx += debugCursor; // Alters it depending on left and right presses
}

void textInput::clear(){
    text.clear();
    position=0;
}

void textInput::keyPressedEvent(ofKeyEventArgs &a) {
    if(localSelected){
        keyPressed(a.key);
    }
}