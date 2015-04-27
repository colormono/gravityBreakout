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
    
    // this is the function for contacts
    void contactStart(ofxBox2dContactArgs &e);
    void contactEnd(ofxBox2dContactArgs &e);
    
    ofxBox2d box2d; // mundo
    
    vector <ofPtr<ofxBox2dRect> > ventanas; // Ventanas (solidos)
    vector <ofPtr<Brick> > bricks; // Globos
    Player player; // Jugador
    Ball ball; // Pelota
    
    bool debug = true; // Debug Info
    string estado;  // Estado Inicial
    int puntos; // Puntos
    
    // Eventos
    static bool shouldRemoveBrick(ofPtr<Brick> &b);
    static bool playerCollision(Player &p);
    void gameEvent(GameEvent &e);
    
    // OSC Tracking
    bool oscTracking;
    ofxOscReceiver receiver;
    
};

