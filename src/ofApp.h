#pragma once

#include "ofMain.h"
#include "textObject.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        // Menu Functions
        void vidSetup();
        void loadAnnotations(string loadPath);
        void savingAnnotations(string savingPath);
        std::pair<vector<string>, bool> menuDisplays(ofDirectory selDir, string fileType, string name, string path, int menuPosX);
        void fontHighlight(ofTrueTypeFont fFont, string sString, int size, int x, int y, ofColor fontCol, ofColor bg, bool center); //Cool hightlight function, yeah!!
        bool fontHighlightClick(ofTrueTypeFont fFont, string sString, int size, int x, int y, ofColor fontCol, ofColor bg, ofColor selCol, bool center); //Cool hightlight function, yeah!!

        // Different menus
        enum CoreMode
        {
        menu,
        mediaPlayback
        };
    
        enum MenuMode
        {
        selectVid,
        selectAnn
        };

        CoreMode        coreMode;
        MenuMode        menuMode;

        // Basic settings
        bool            leftClick;
        int             dimWidth;
        int             dimHeight;
        int             fVerdanaSize;
        int             fHelveticaSize;
        ofTrueTypeFont	fVerdana;
        ofTrueTypeFont	fHelvetica;

    
        // Selecting video
        ofDirectory     vidDir;
        bool            displayError;
        bool            vidSelected;
        string          vidName;
        string          vidPath;
    
        // Annotation selecting
        ofDirectory     annDir;
        bool            annSelected;
        bool            deletionWarning;
        int             annNumSel;
        string          annName;
        string          annPath;
        string          autoPath;
        string          hexAnnName;
    
        //Video player variables
        ofQTKitPlayer   vidMain;
        ofQTKitDecodeMode decodeMode;
        float             volume;

        //Scrubber variables
        float           barWidth;
        bool            scrubReady;
    
        // Saving
        // Saving will be asked right before going into the video, it ask what to name the file and if it wants to overwrite one already there
        ofDirectory     txtDir;//This is to see if there is a txt already there;
        string          content;
        vector<string>  timeStartEnd;
        vector<string>  sPosX; //saves the positions
        vector<string>  sPosY;
        bool            doIWantToQuit;
    
        // Timer
        float           timer;
        float           startTime; // store when we start time timer
    
        // Text object
        textObject**    theTexts;
        int             textCount;
        bool            createNew;
        bool            useCommKeys;
    
        bool textOption, audioOption, highlightOption, hyperlinkOption;
};
