#pragma once

// Libraries
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxBox2d.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

// Classes
#include "Player.h"
#include "Brick.h"
#include "Ball.h"
#include "GameEvent.h"

// App
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

    void contactStart(ofxBox2dContactArgs &e);
    void contactEnd(ofxBox2dContactArgs &e);
    static bool shouldRemoveBrick(ofPtr<Brick> &b);
    static bool playerCollision(Player &p);
    void gameEvent(GameEvent &e);

    ofxXmlSettings XML; // XML settings
    string xmlStructure;
    string message;

    ofxBox2d box2d; // 2d world
    vector <ofPtr<ofxBox2dRect> > ventanas; // Obstacles
    vector <ofPtr<Brick> > bricks; // Bricks
    Player player; // Player
    Ball ball; // Ball
    float timer; // Clock for timing events
    bool countdown; // Countdown trigger
    
    bool debug; // Show debug info
    string gameState;  // Game state
    int score; // Score
    bool useOSC; // OSC Tracking
    int userAvailable; // Check if there is a user
    ofxOscReceiver receiver; // OSC Receiver
    
    
};

