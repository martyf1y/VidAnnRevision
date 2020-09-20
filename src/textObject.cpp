//
//  textObject.cpp
//  videoAnnotater
//
//  Created by Marty on 25/07/14.
//
//

#include "textObject.h"

//--------------------------------------------------------------
textObject::textObject(int _startTime, int _totalFrame){
    startTime = _startTime;
    endTime = _totalFrame - 1; // This makes sure it only plays to the end of vid
    totalFrame = _totalFrame;
    textBox.init(); // This sets up the key pressed event
    
    ofAddListener(textBox.evtEnter, this, &textObject::addText);
    newText = true;
    isSelected = true;
    lastFrame = 0;
    posX.resize(totalFrame - startTime); // Initial size
    posY.resize(totalFrame - startTime); // Initial size
    std::fill(posX.begin(), posX.end(), 0);
    std::fill(posY.begin(), posY.end(), 0);
}

//--------------------------------------------------------------
void textObject::update(int mX, int mY, int currFrame){
    if(isSelected){
        tPosX = mX; //When selected follow mouse, even if its not a new frame
        tPosY = mY;
    }

    if(lastFrame != currFrame){ // Makes sure saving things only happen when a frame passes
        if(isSelected){
            if(currFrame > endTime){ // Makes sure the selected doesn't go away after end frame
                (endTime<totalFrame-105?endTime=currFrame+100:endTime=totalFrame-1); // Makes sure it doesn't go over the limit
            }
            if(currFrame >= totalFrame - 5){ // Bit of leeway to make sure the item is deselected
                isSelected = false;
                newText = false;
            }
        }
        else if (posX[currFrame - startTime] == 0 || posY[currFrame - startTime] == 0){ // If not selected stay where it is
            // Do nothing to the position
            // This is just to make sure the text stays where it is otherwise
        }
        else{ // If it has already gone through once move it to the old spot
            tPosX = posX[currFrame - startTime];
            tPosY = posY[currFrame - startTime];
        }
        
        if((currFrame < endTime
            && currFrame > startTime)
           || isSelected == true){ // This sets the previous positions
            posX[currFrame - startTime] = tPosX;
            posY[currFrame - startTime] = tPosY;
        }
        lastFrame = currFrame;
    }
}

//--------------------------------------------------------------
void textObject::draw(ofTrueTypeFont font){
    ofPushMatrix();
    textBox.draw(tPosX, tPosY, isSelected);
 //   cout << "FIND IT3 " << startTime << "  WOW " << isSelected << endl;

    ofPopMatrix();
    
 //    drawEntries();
}

void textObject::mouseReleased(int mX, int mY){
    if(isSelected == true){
        isSelected = false;
    }
    else if(mX > tPosX - (textBox.cursorx*8+10)/2
            && mX < (tPosX - (textBox.cursorx*8+10)/2) + textBox.cursorx*8+10
            && mY > tPosY - 15 && mY < tPosY
            && !isSelected){
        isSelected = true;
    }
    textBox.localSelected = isSelected; // This is how I stop things from drawing on others
}

//--------------------------------------------------------------
void textObject::addText(string &s){
    textEntries.push_back(s);
    textBox.clear();
}
//--------------------------------------------------------------
//void textObject::drawEntries(){
  //  for (int i=0; i<textEntries.size(); ++i) {
    //    ofDrawBitmapString(textEntries[i], 50, 100+10*i);
   // }
//}
