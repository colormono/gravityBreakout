#pragma once

// Libraries
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxBox2d.h"
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
    
    // Contact events
    void contactStart(ofxBox2dContactArgs &e);
    void contactEnd(ofxBox2dContactArgs &e);

    ofxBox2d box2d; // 2d world
    
    vector <ofPtr<ofxBox2dRect> > ventanas; // Ventanas (obstacles)
    vector <ofPtr<Brick> > bricks; // Globos (breakball bricks like)
    Player player; // Jugador
    Ball ball; // Pelota
    
    bool debug = true; // Show debug info
    string gameState;  // Game state
    int score; // Score
    
    // Eventos
    static bool shouldRemoveBrick(ofPtr<Brick> &b);
    static bool playerCollision(Player &p);
    void gameEvent(GameEvent &e);
    
    // OSC Tracking
    bool useOSC;
    ofxOscReceiver receiver;
    
};

