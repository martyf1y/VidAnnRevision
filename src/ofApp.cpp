#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Basic settings
    ofBackground(95,120,120);
    ofSetVerticalSync(true); //smooth video
    dimWidth = 1440;
    dimHeight = 810;
    leftClick = false;
    
    // Fonts
    fVerdanaSize = 12;
    fVerdana.loadFont(OF_TTF_SANS, fVerdanaSize);
    fVerdana.setLineHeight(18.0f);
    fVerdana.setLetterSpacing(1.037);
    fHelveticaSize = 14;
    fHelvetica.loadFont("Helvetica.dfont", fHelveticaSize);
    fHelvetica.setLetterSpacing(1.037);
    
    // Selection
    vidDir.listDir("media/");
    vidDir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order
    vidSelected =   false;
    displayError =  false;
    
    // Annotation selection
    annDir.listDir("annotation/");
    annDir.sort();
    annSelected = false;
    
    // Text Annotations
    theTexts = new textObject*[1000];
    textCount = 0;
    createNew = true;
    
    // Video
    vidMain.setPixelFormat(OF_PIXELS_RGB);
    decodeMode = OF_QTKIT_DECODE_PIXELS_AND_TEXTURE; // Texture only is fastest, but no pixel access allowed.
    volume = 0.5;
    
    // Scrubber
    barWidth = dimWidth;
    scrubReady = false;
    
    // Interaction
    useCommKeys = true;
    
    // Quiting settings
    doIWantToQuit = false;
    ofSetEscapeQuitsApp(false);
    
    // Options
    textOption = true;
    audioOption = false;
    highlightOption = false;
    hyperlinkOption = false;
    
    // The menus
    coreMode = menu;
    menuMode = selectVid;
}

void ofApp::vidSetup(){
    switch(menuMode){
        case selectVid:{
            // Pairs allow you to return more than one value. Wooh!!!
            std::pair<vector<string>, bool> namePathSelectValuesFromFunc = menuDisplays(vidDir, "media", vidName, vidPath, 70);
            vidName = namePathSelectValuesFromFunc.first[0];
            vidPath = namePathSelectValuesFromFunc.first[1];
            vidSelected = namePathSelectValuesFromFunc.second;
            
            if(vidSelected){
                if(vidMain.isLoaded()){// Closes previous one
                    vidMain.stop();
                    vidMain.close();
                }
                vidMain.loadMovie(vidPath, decodeMode);
                vidMain.play();
                vidMain.setVolume(0);
            }
            
            if(vidMain.getPositionInSeconds() >= 10){
                vidMain.setPosition(0);
            }
            
            if(vidMain.isLoaded()){ // PREVIEW!!!!
                ofSetColor(255, 255, 255);
                vidMain.draw(dimWidth/2 - vidMain.getWidth()/6,
                             dimHeight/1.8, vidMain.getWidth()/3, vidMain.getHeight()/3);
                //Let's the person move to next stage
                bool wantToLoad = fontHighlightClick(fVerdana, "Click to load media for annotating", fVerdanaSize, dimWidth/2, dimHeight/2 + fVerdanaSize*2.5, ofColor::white, ofColor::black, ofColor::red,  true);
                if(leftClick && wantToLoad){
                    vidMain.stop();
                    menuMode = selectAnn; // WE MOVE ONTO THE NEXT CASE!!!!!!
                }
            }
            ////////////// Text Displays //////////////////
            if(vidName != ""){fontHighlight(fVerdana, "Media Selected: " + vidName, fVerdanaSize, dimWidth/2, dimHeight/2, ofColor::green, ofColor::black, true);} //Shows video selected
            string credit = "Annotation software created by Matthew Martin. Icon designed by Emile Drescher";
            ofSetColor(ofColor::black);
            fVerdana.drawString(credit, 10, ofGetHeight() - fVerdanaSize/2); //Credits
        }
            break;
            
        case selectAnn:{
            // Pairs allow you to return more than one value. Wooh!!!
            std::pair<vector<string>, bool> namePathSelectValuesFromFunc2 = menuDisplays(annDir, "annotation", annName, annPath, 170);
            annName = namePathSelectValuesFromFunc2.first[0];
            annPath = namePathSelectValuesFromFunc2.first[1];
            annSelected = namePathSelectValuesFromFunc2.second;
            
            if(annSelected){
                annPath = annPath + "/";
                bool textFileThere = false;
                hexAnnName = ofToHex(annName); // This looks for the hex file
                ofDirectory hexDir;
                try{ // If there is a folder we can proceed
                    hexDir.listDir(annPath);
                }
                catch (exception &e){
                    cout << "Did not work: " << &e << endl;
                    displayError = true;
                    hexAnnName = annName = annPath = "";
                }
                
                for(int i = 0; i < hexDir.size(); i++){
                    if(hexDir.getName(i) == hexAnnName + ".txt"){ // This compares all the files
                        textFileThere = true;
                    }
                }
                if(textFileThere){ // If there is no text file then we don't want to load anything, move on.
                    try{
                        loadAnnotations(annPath);
                    }
                    catch (exception &e){ // States the file could not be loaded correctly
                        displayError = true;
                        hexAnnName = annName = annPath = "";
                    }
                }
                else{
                    displayError = true;
                    hexAnnName = annName = annPath = "";
                }
                
                
                
                //  THIS WILL BE WHERE IMAGES AND AUDIO SEARCHES GO
                
                
                
                annSelected = false;
                
            }
            
            bool back = fontHighlightClick(fVerdana, ("Back"), fVerdanaSize, 0, ofGetHeight() - fVerdanaSize/2, ofColor::white, ofColor::black, ofColor::red,  false);
            if(leftClick && back){
                menuMode = selectVid;
                vidMain.close();
                hexAnnName = annName = annPath = "";
                vidName = vidPath = "";
                for(int i = 0; i < textCount; i++){ // NEED TO ERASE THE OBJECT VALUABLES
                    delete theTexts[i];
                }
                textCount = 0;
            }
            
            // Let's the person move to next stage
            bool beginAnn = fontHighlightClick(fVerdana, (annName!=""?"Annotation " + annName.substr(0, 15) + " selected. Click to begin":"Begin without loading annotations"), fVerdanaSize, dimWidth/2, dimHeight - fVerdanaSize, (annName!=""?ofColor::green:ofColor::white), ofColor::black, ofColor::green,  true);
            
            if(leftClick && beginAnn){
                if(annName==""){ // Name a new annotation file
                    annName = ofSystemTextBoxDialog("Name the annotation file that will save");
                    hexAnnName = ofToHex(annName); // This looks for the hex file
                    annPath = "annotation/" + annName + "/";
                }
                if(annName !="" && annPath !=""){
                    bool result = true;
                    if(deletionWarning){ // Warn the user that they will delete part of the loaded annotations
                        result = ofSystemYesNoDialoge("The annotation file chosen is larger than the video timeframe.","Continue anyway and lose the rest of the annotations?");
                    }
                    if(result){
                        // Checks to see if the autosave folder already exists
                        autoPath = "autoSaves/" + annName + "/"; // We create the autosaves folder
                        ofDirectory dirAuto(autoPath);
                        if(dirAuto.exists()){ // Creates the folder names
                            bool loadAutoSaves = ofSystemYesNoDialoge("There are autosaves associated with this annotation name. Want to use them instead of your main annotations?", "It will not save over your main annotations until you say so.");
                            if(loadAutoSaves){ // LOAD AUTO SAVES HERE, otherwise continue
                                try{
                                    loadAnnotations(autoPath);
                                }
                                catch (exception &e){ // States the file could not be loaded correctly
                                    ofSystemAlertDialog("Autosave could not be loaded");
                                }
                            }
                            if(displayError){ // This is us reverting back to the original annotaitons because auto didn't work
                                ofSystemAlertDialog("Could not load autosave, loading the original folder instead.");
                                try{
                                    loadAnnotations(annPath);
                                }
                                catch (exception &e){ // States the file could not be loaded correctly
                                    cout << "Error: " << &e << endl;;
                                }
                            }
                        }
                        else{ // Creat the auto path if not there
                            dirAuto.create(true);
                        }
                        for(int i = 0; i < textCount - 1; i++){
                            theTexts[i]->newText = false;
                            theTexts[i]->isSelected = false;
                            theTexts[i]->textBox.localSelected = false; // This is how I stop things from drawing on others
                        }
                        coreMode = mediaPlayback;
                        menuMode = selectVid; // WE MOVE ONTO THE NEXT CASE!!!!!!
                        vidMain.setFrame(0);
                        vidMain.play();
                        vidMain.setVolume(volume);
                    }
                    else{
                        hexAnnName = annName = annPath = autoPath = "";
                        for(int i = 0; i < textCount; i++){// DELETE TEXTS
                            delete theTexts[i];
                        }
                        textCount = 0;
                    }
                }
            }
            //////////////Text Displays//////////////////
            string instruct2 = "Media has loaded.";
            string instruct3 = "You can also skip the annotations by clicking the 'Begin without loading annotations' button";
            fontHighlight(fVerdana, instruct2, fVerdanaSize, dimWidth/2, 50, ofColor::red, ofColor::black, true); //Instructions
            fontHighlight(fVerdana, instruct3, fVerdanaSize, dimWidth/2, 80, ofColor::white, ofColor::black, true); //Instructions
        }
            break;
    }
}
void ofApp::loadAnnotations(string loadPath){
    for(int i = 0; i < textCount; i++){ // NEED TO ERASE THE OBJECT VALUABLES
        delete theTexts[i];
    }
    textCount = 0;
    deletionWarning = false;
    ofBuffer loadFile = ofBufferFromFile(loadPath + hexAnnName + ".txt");
    vector<string> textInfo;// This will contain all texts in an array
    textInfo = ofSplitString(loadFile, "|<|>1<|>|");
    if(textInfo.size() == 1){ // If this file doesn't work write then stop trying
        loadPath = "";
        displayError = true;
    }
    
    for(int i = 0; i < textInfo.size() - 1; i++){//Creating the objects
        vector<string> splitTextNames = ofSplitString(textInfo[i], "~~<0>~~");
        vector<string> splitTextStart = ofSplitString(splitTextNames[1], "START");
        vector<string> splitTextEnd = ofSplitString(splitTextStart[1], "END");
        vector<string> splitTextPos = ofSplitString(splitTextEnd[1], "&Y");
        if(ofToInt(splitTextStart[0]) < vidMain.getTotalNumFrames() - 10){ // Make sure it is not past the end total of the video
            theTexts[textCount] = new textObject(0, vidMain.getTotalNumFrames());
            theTexts[textCount]->textBox.text = splitTextNames[0];//Here is the content
            //cout << "TEXT: " << theTexts[textCount]->textBox.text << endl;
            theTexts[textCount]->startTime = ofToInt(splitTextStart[0]);//Here is the start time
            if(ofToInt(splitTextEnd[0]) < vidMain.getTotalNumFrames()){ //Stops going over end
                theTexts[textCount]->endTime = ofToInt(splitTextEnd[0]);//Here is the end time
            }
            else {
                theTexts[textCount]->endTime = vidMain.getTotalNumFrames() - 1;
                deletionWarning = true;
            }
            bool endOfLine = false; // This will contain all positions in an array
            int m = 0; // This helps get all the right coordintes in
            int s = 0; // This puts the coordinates position correctly to get a full array
            while(!endOfLine){// This will make the maximum amount of positions of the text for the frame size of the video.
                vector<string> textPosXY = ofSplitString(splitTextPos[m+s], "&X");
                if(textPosXY[0] != "" && textPosXY[textPosXY.size()-1] != ""){// Have to watch out for blank positions
                    theTexts[textCount]->posX[s] = ofToInt(textPosXY[0]);//Here is the x value
                    theTexts[textCount]->posY[s] = ofToInt(textPosXY[1]);//Here is the y value
                    s++;
                }
                else{
                    m++;
                }
                if(s > theTexts[textCount]->endTime - theTexts[textCount]->startTime - 1 ||
                   s+m >= splitTextPos.size()){
                    endOfLine = true;
                }
                else if(s > (vidMain.getTotalNumFrames() - theTexts[textCount]->startTime)){
                    endOfLine = true;
                    deletionWarning = true;
                    cout << "DOES THIS EVER HAPPEN??????" << endl;
                }
            }
            theTexts[textCount]->newText = false;
            theTexts[textCount]->isSelected = false;
            theTexts[textCount]->textBox.localSelected = false; // This is how I stop things from drawing on others
            textCount ++;
        }
        else{
            deletionWarning = true;
        }
    }
}
std::pair<vector<string>, bool> ofApp::menuDisplays(ofDirectory selDir, string fileType, string name, string path, int menuPosX){
    std::pair<vector<string>, bool> namePathSelectValues;
    
    for(int i = 0; i < (int)selDir.size(); i++){ // Displays the file choices to select
        string fileInfo = ofToUpper(fileType) + " " + ofToString(i + 1) + ": " + selDir.getName(i);
        if(fileInfo.length() >= 30){fileInfo = fileInfo.substr(0, 30);}
        bool overTop = fontHighlightClick(fVerdana, fileInfo, fVerdanaSize, dimWidth/8, i * 28 + menuPosX, ofColor::white, ofColor::black, ofColor::red, false);//Sees if mouse is over button
        if(leftClick && overTop){
            name = selDir.getName(i);
            path = selDir.getPath(i);
            namePathSelectValues.second = true;
        }
    }
    
    //Here we find a specific file
    bool overSel = fontHighlightClick(fVerdana, "Locate a local " + fileType + (fileType==" media"?"file":" folder"), fVerdanaSize, dimWidth/4 * 3, menuPosX, ofColor::white, ofColor::black, ofColor::red,  true);
    
    if(leftClick && overSel){
        ofFileDialogResult res;
        res = ofSystemLoadDialog("Please select the " + fileType + (fileType==" media"?"file":" folder"), (fileType=="media"?false:true), "../../../data/" + fileType + "/"); // /Users/myMarty/...
        if(res.bSuccess){
            name = res.fileName;
            path = res.filePath;
            namePathSelectValues.second = true;
        }
    }
    
    ////////////// Text Displays //////////////////
    // Description for the menu selection screen
    fontHighlight(fVerdana, "OR", fVerdanaSize, dimWidth/2, menuPosX , ofColor::white, ofColor::black, false);
    string instruct = "Select the " + fileType + " file from the library or click the 'Locate a local " + fileType + " file' button to find the " + fileType + " file on your machine";
    fontHighlight(fVerdana, instruct, fVerdanaSize, dimWidth/2, 20, ofColor::white, ofColor::black, true); //Instructions
    
    if(displayError){
        ofSystemAlertDialog("Sorry could not load " + fileType + ", please try another one");
        displayError = false;
    }
    namePathSelectValues.first.push_back(name);
    namePathSelectValues.first.push_back(path);
    return namePathSelectValues;
}

//--------------------------------------------------------------
void ofApp::update(){
    if(vidMain.isLoaded() && menuMode != selectAnn){  //This should always make sure the video is working
        vidMain.update();
        vidSelected = false;
    }
    else if(vidSelected) {displayError = true; vidSelected = false; vidName = "";}
    switch(coreMode){
        case menu:{}
            break;
        case mediaPlayback:{
            // Updates the text annotations
            useCommKeys=true;
            for(int i = 0; i < textCount; i++){ // This has to be by itself otherwise its not accurate
                if(useCommKeys){
                    if(theTexts[i]->isSelected){useCommKeys=false;} // Checks to see if the text is selected for controls
                }
            }
            // bool alreadyGotOne = false; // This means I already have a text silly
            for(int i = 0; i < textCount; i++){// Checks to see if the annotation can be released
                if(vidMain.getCurrentFrame() <= theTexts[i]->endTime
                   && vidMain.getCurrentFrame() >= theTexts[i]->startTime
                   && leftClick){
                    if(theTexts[i]->isSelected && !useCommKeys){
                        theTexts[i]->mouseReleased(mouseX, mouseY);
                    }
                    else if(useCommKeys){
                        theTexts[i]->mouseReleased(mouseX, mouseY);
                        if(theTexts[i]->isSelected){useCommKeys=false;}
                    }
                }
                if(vidMain.getCurrentFrame() - theTexts[i]->startTime >= 0){//This makes sure it doesnt update outside the limit
                    theTexts[i]->update(mouseX, mouseY, vidMain.getCurrentFrame());
                }
            }
            
          //  (vidMain.isPaused()?startTime=ofGetElapsedTimeMillis()-timer:timer=ofGetElapsedTimeMillis()-startTime); // Timer does not save when paused
            timer=ofGetElapsedTimeMillis()-startTime;
            
            if(timer >= 10000){ // Every minute
                bool alreadyPause;
                if(vidMain.isPaused()){alreadyPause=true;} // If it is paused we want to keep it paused
                else{alreadyPause=false; vidMain.setPaused(true);}
                fontHighlight(fVerdana, "SAVING", fVerdanaSize, dimWidth/2, dimHeight/2, ofColor::red, ofColor::black, true); //Instructions
                savingAnnotations(autoPath);
                (alreadyPause?vidMain.setPaused(true):vidMain.setPaused(false));
                startTime = ofGetElapsedTimeMillis();
            }
        }
            break;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    switch(coreMode){
        case menu:{vidSetup();}
            break;
        case mediaPlayback:{
            if (vidMain.isLoaded()){
                ofSetColor(255, 255, 255);
                vidMain.draw(0, 0, dimWidth, dimHeight);
            }
            
            ////////////////////////////// SCRUBBER //////////////////////////////
            float pct = ofMap(vidMain.getCurrentFrame(), 0.0, vidMain.getTotalNumFrames(), 0.0, 1.0, true); // Percantage of the timer
            ofSetHexColor(0xf02589);
            ofNoFill();
            ofRect(0, dimHeight - 25, barWidth*pct - 3, 15);
            ofSetColor(255); // Draw a timeline at the bottom of the screen.
            ofRect(0, dimHeight -35, dimWidth, 35);
            float playheadX = vidMain.getPosition() * dimWidth;
            ofLine(playheadX, dimHeight - 50, playheadX, dimHeight);
            ofDrawBitmapStringHighlight(ofToString(vidMain.getCurrentFrame()) + " / " + ofToString(vidMain.getTotalNumFrames()), playheadX + 10, dimHeight - 40);
            
            ofFill();
            ofDrawBitmapStringHighlight("Rate: " + ofToString(vidMain.getSpeed(), 2) + "\n" +
                                        "Frames: " + ofToString(vidMain.getCurrentFrame()) + " / " + ofToString(vidMain.getTotalNumFrames()) + "\n" +
                                        "Position: " + ofToString(vidMain.getPosition() * 100, 1) + "% \n" +
                                        "Volume: " + ofToString(volume, 2) + "\n" +
                                        "\n" +
                                        "TAB: Create text / End text \n" +
                                        "[space]: Pause / Play \n" +
                                        "[up]/[down]: Adjust Speed \n" +
                                        "[-]/[=]: Volume \n" +
                                        "Saving in: " + ofToString(10 - timer/1000) +  "\n" +
                                        "useCommKeys: " + (useCommKeys?"YES":"NO") + "\n",
                                        10, 20, ofColor(0,0,0, 130), ofColor::white);
            
            ////////////////////////////// TEXTS //////////////////////////////
            for(int i = 0; i < textCount; i++){
                if((vidMain.getCurrentFrame() <= theTexts[i]->endTime
                    && vidMain.getCurrentFrame() >= theTexts[i]->startTime)
                   || theTexts[i]->isSelected == true){
                    theTexts[i]->draw(fVerdana);
                }
                else if(theTexts[i]->isSelected == false){
                    theTexts[i]->textBox.localSelected = false; // STOP BEING SELECTED
                }
            }
            
            
            bool text  = fontHighlightClick(fVerdana, ("Text"), fVerdanaSize, dimWidth/4, ofGetHeight() - fVerdanaSize/2, ofColor::white, (textOption?ofColor::darkGreen:ofColor::black), ofColor::red,  true);
            if(leftClick && text){textOption = true; audioOption = false; highlightOption = false; hyperlinkOption = false;}
            bool audio = fontHighlightClick(fVerdana, ("Audio"), fVerdanaSize, dimWidth/8*3, ofGetHeight() - fVerdanaSize/2, ofColor::white, (audioOption?ofColor::darkGreen:ofColor::black), ofColor::red,  true);
            if(leftClick && audio){textOption = false; audioOption = true; highlightOption = false; hyperlinkOption = false;}
            bool highlight  = fontHighlightClick(fVerdana, ("Highlight"), fVerdanaSize, dimWidth/2, ofGetHeight() - fVerdanaSize/2, ofColor::white, (highlightOption?ofColor::darkGreen:ofColor::black), ofColor::red,  true);
            if(leftClick && highlight){textOption = false; audioOption = false; highlightOption = true; hyperlinkOption = false;}
            bool hyperlink  = fontHighlightClick(fVerdana, ("Hyperlink"), fVerdanaSize, dimWidth/8*5, ofGetHeight() - fVerdanaSize/2, ofColor::white, (hyperlinkOption?ofColor::darkGreen:ofColor::black), ofColor::red,  true);
            if(leftClick && hyperlink){textOption = false; audioOption = false; highlightOption = false; hyperlinkOption = true;}
            
            bool save = fontHighlightClick(fVerdana, ("Save"), fVerdanaSize, 60, ofGetHeight() - fVerdanaSize/2, ofColor::white, ofColor::black, ofColor::red,  false);
            if(leftClick && save && useCommKeys){ // The saving area
                vidMain.setPaused(true);
                bool shouldISave = ofSystemYesNoDialoge("Would you like to save?");
                if(shouldISave){
                    bool alreadyPause;
                    if(vidMain.isPaused()){alreadyPause=true;} // If it is paused we want to keep it paused
                    else{alreadyPause=false; vidMain.setPaused(true);}
                    fontHighlight(fVerdana, "SAVING", fVerdanaSize, dimWidth/2, dimHeight/2, ofColor::red, ofColor::black, true); //Instructions
                    savingAnnotations(annPath);
                    (alreadyPause?vidMain.setPaused(true):vidMain.setPaused(false));
                }
                vidMain.setPaused(false);
            }
            
            ////////////////////////////// QUITTING //////////////////////////////
            bool quit = fontHighlightClick(fVerdana, ("Quit"), fVerdanaSize, 0, ofGetHeight() - fVerdanaSize/2, ofColor::white, ofColor::black, ofColor::red,  false);
            if((leftClick && quit && useCommKeys) || doIWantToQuit){ // The quitting area
                vidMain.setPaused(true);
                bool areYouSure = ofSystemYesNoDialoge("Are you sure you want to quit?","Click Yes to quit this annotation session");
                if(areYouSure){
                    bool askSave = ofSystemYesNoDialoge("Would you like to save before you quit?","Click Yes to save annotation session");
                    if(askSave){
                        savingAnnotations(annPath);
                    }
                    menuMode = selectVid;
                    coreMode = menu;
                    vidMain.close();
                    hexAnnName = annName = annPath = "";
                    vidName = vidPath = "";
                    for(int i = 0; i < textCount; i++){ // NEED TO ERASE THE OBJECT VALUABLES
                        delete theTexts[i];
                    }
                    textCount = 0;
                    setup(); // Just to reset hings again
                }
                else{
                    vidMain.setPaused(false);
                }
            }
        }
            break;
    }
    fontHighlight(fVerdana, "Annotation Texts Number: " + ofToString(textCount), fVerdanaSize, dimWidth - 125, ofGetHeight() - fVerdanaSize/2, ofColor::green, ofColor::black, true);
    leftClick=false; // Stops mouse from always being on
    doIWantToQuit=false; // Stops escape from always being on
    
    cout << ofToHex("Come") << endl; // This looks for the hex file

}

void ofApp::savingAnnotations(string savingPath)
{
    //Gather all the objects and save them
    ofBuffer theTextFile; // Saves the file
    
    for(int i = 0; i < textCount; i++){
        theTextFile.append(theTexts[i]->textBox.text + "~~<0>~~"); // Saves as name, put in unlikely character to separate types
        theTextFile.append(ofToString(theTexts[i]->startTime) + "START");// Saves start time
        theTextFile.append(ofToString(theTexts[i]->endTime) + "END"); // Saves end time
        int lastPosX = 0; // These will make sure the text doesn't go to 0
        int lastPosY = 0;
        for(int s = 0; s < theTexts[i]->posX.size(); s++){ // This is to get all pos, posx and posy should be the same
            if (theTexts[i]->posX[s] == 0 || theTexts[i]->posY[s] == 0){ // If not selected stay where it is
                // This will make sure the annotations that have no end stay where they are
                theTextFile.append(ofToString(lastPosX) + "&X");
                theTextFile.append(ofToString(lastPosY) + "&Y");
            }
            else{
                theTextFile.append(ofToString(theTexts[i]->posX[s]) + "&X");
                theTextFile.append(ofToString(theTexts[i]->posY[s]) + "&Y");
                lastPosX = theTexts[i]->posX[s];
                lastPosY = theTexts[i]->posY[s];
            }
            // cout << "ALL X POS " << theTexts[i]->posX[s] << endl;
            // cout << "ALL Y POS " << theTexts[i]->posY[s] << endl;
        }
        theTextFile.append("|<|>1<|>|"); // This is the end of the object
    }
    cout << savingPath + hexAnnName + ".txt" << endl;
    ofFile newFile(ofToDataPath(savingPath + hexAnnName + ".txt"), ofFile::WriteOnly); // File may not exist yet
    newFile.create();
    bool fileWritten = ofBufferToFile(savingPath + hexAnnName + ".txt", theTextFile);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_ESC){doIWantToQuit=true;}
    
    switch(coreMode){
        case menu:{}break;
        case mediaPlayback:{
            if(useCommKeys){ // Allow commands when not typing
                switch(key){
                    case OF_KEY_UP:vidMain.setSpeed(vidMain.getSpeed() + 0.05);
                        break;
                    case OF_KEY_DOWN:vidMain.setSpeed(vidMain.getSpeed() - 0.05);
                        break;
                    case '0':vidMain.firstFrame();
                        break;
                    case '-':
                        (volume>0.00?volume-=0.02:volume=0.00);
                        vidMain.setVolume(volume);
                        break;
                    case '=':
                        (volume<1.00?volume+=0.02:volume=1.00);
                        vidMain.setVolume(volume);
                        break;
                    case ' ':(vidMain.isPaused()?vidMain.setPaused(false):vidMain.setPaused(true));
                        break;
                }
            }
        }
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch(coreMode){
        case menu:{}break;
        case mediaPlayback:{
            switch(key){
                case OF_KEY_RETURN:{ // Alternate to pausing
                    (vidMain.isPaused()?vidMain.setPaused(false):vidMain.setPaused(true));
                }
                    break;
                case OF_KEY_TAB:{
                    for(int i = 0; i < textCount; i++){
                        int countSelect = 0;
                        if(theTexts[i]->isSelected){// This makes sure you cant have selected and create new text
                            createNew = false;
                            
                            theTexts[i]->endTime = vidMain.getCurrentFrame() + 1; // Make sure that when the the user places text it is stil seen for a bit
                            theTexts[i]->posX.erase(theTexts[i]->posX.begin() +
                                                    vidMain.getCurrentFrame() - theTexts[i]->startTime,
                                                    theTexts[i]->posX.begin() + theTexts[i]->posX.size());
                            theTexts[i]->posY.erase(theTexts[i]->posY.begin() +
                                                    vidMain.getCurrentFrame() - theTexts[i]->startTime,
                                                    theTexts[i]->posY.begin() + theTexts[i]->posY.size());
                            theTexts[i]->newText = false;
                            theTexts[i]->isSelected = false;
                            theTexts[i]->textBox.localSelected = false; // This is how I stop texts from drawing on others
                        }
                    }
                    if(createNew && vidMain.getSpeed() >= 0){ // Creates an annotation when the video is playing
                        theTexts[textCount] = new textObject(vidMain.getCurrentFrame(), vidMain.getTotalNumFrames());
                        textCount ++; // Remember once text is created it becomes selected
                    }
                    else{
                        createNew = true;// Put it back in to allow new text
                    }
                }
                    break;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(scrubReady){
        vidMain.setFrame(ofMap(mouseX, 0, barWidth, 0, vidMain.getTotalNumFrames()));
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    switch(coreMode){
        case menu:{}break;
        case mediaPlayback:{
            if(mouseX > 0 && mouseX < barWidth &&
               mouseY > dimHeight -50 && mouseY < dimHeight && button==0 && useCommKeys){
                vidMain.setFrame(ofMap(mouseX, 0, barWidth, 0, vidMain.getTotalNumFrames()));
                scrubReady = true;
            }
        }
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if(button==0){leftClick = true; scrubReady = false;}
}

void ofApp::fontHighlight(ofTrueTypeFont fFont, string sString, int size, int x, int y, ofColor fontCol, ofColor bg, bool center){
    ofRectangle sBound = fFont.getStringBoundingBox(sString, 0, 0);
    ofSetColor(0,0,0,0);
    ofRect((center?x - sBound.width/2 - size:x), y - (size+size/2), sBound.width + size*2, size*2);
    ofSetColor(fontCol);
    fFont.drawString(sString, (center?x - sBound.width/2:x+size), y);
}

bool ofApp::fontHighlightClick(ofTrueTypeFont fFont, string sString, int size, int x, int y, ofColor fontCol, ofColor bg, ofColor selCol, bool center){
    ofRectangle sBound = fFont.getStringBoundingBox(sString, 0, 0);
    ofRectangle bgBound((center?x - sBound.width/2 - size:x), y - (size+size/2), sBound.width + size*2, size*2);
    bool thisOne = false;
    
    if(mouseX >= bgBound.x && mouseY >= bgBound.y &&
       mouseX <= bgBound.x + bgBound.width && mouseY <= bgBound.y + bgBound.height)
    {thisOne = true;}
    
    ofSetColor(thisOne?ofColor::white:bg);
    ofRect(bgBound);
    ofSetColor(thisOne?selCol:fontCol);
    fFont.drawString(sString, (center?x - sBound.width/2:x+size), y);
    
    // Add different cursor
    return thisOne;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
