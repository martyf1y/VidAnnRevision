#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1440,835,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
// User gets two choices when saving and loading. They can load their file or if we have an autosaved one they can load that. Autosave happens automatically but they will need to manually save to save over their file.
// When loading the text file HAS TO exist

// Reset annotations function which deletes the old objects COMPLETED

// Make a warning about losing parts of the annotations if the video is smaller    COMPLETED

// STOP TEXTS FROM BEING SELECTED AT THE SAME TIME. COMPLETED

// Error with setting the position of previous frames that do not exist for the posX frame position FIXED?


// AUTOSAVING COMPLETED

// If the user begins without annotations then they will be saved to the annotations folder COMPLETED

// Turned the saving process into folders so there can be various types of files to load and save in annotations

// Ask for filename if there is no annotation

// The program creates a saving folder for the annotations if they are not there


// TODO

// SELECTING FILES
// Finds folder, looks for a hexadecimal txt, if it can't find one it creates one from scratch. If it finds similar files with hexadecimal names then it tries to use them too. Otherwise say it is currupt and don't use it

// Folder stuff
// MAYBE JUST A FOLDER THAT IS saved as READ ONLY or LOCKED

// RESIZING

// Make it so the texts only go as large as the file size

// Reratio the screen for the different dimensions. Also make it so the texts are altered to that position

// CREATE OTHER TYPES OF ANNOTATIONS

// ERRORS
// MAJOR ERROR!!!!!!!!!! reloading objects causes big errors with variables and whatnot. Specially selection stuff

// ERROR WITH THE LARGE FILES





