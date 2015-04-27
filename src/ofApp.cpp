#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackgroundHex(0x222222);
    ofSetFrameRate(60);
    
    // Inicializar mundo
    box2d.init();
    box2d.enableEvents();   // <-- turn on the event listener
    box2d.setGravity(0, -5);
    box2d.createBounds();
    //box2d.setFPS(30.0);
    box2d.registerGrabbing();
    
    // Estado inicial
    estado = "precarga";
    puntos = 0;
    
    // Player
    player.setup();
    
    // Ball
    ball.setup();
    
    // listen to any of the events for the game
    ofAddListener(GameEvent::events, this, &ofApp::gameEvent);
    
    // Type of tracking
    oscTracking = false;
    
    // start to listen OSC messages on port 12345
    receiver.setup(12345);
}

//--------------------------------------------------------------
void ofApp::gameEvent(GameEvent &e) {
    
    // Reventar globo
    if ( e.message == "reventar-globo" ) {
        cout << "Game Event: "+e.message << endl;
        // Sumar puntos
        puntos ++;
    }
    
    // Reventar globo especial
    if ( e.message == "reventar-globo-a" ) {
        cout << "Game Event: "+e.message << endl;
        // Cambiar color de la pelota por x tiempo
        // Los puntos suman doble
        ball.color.setHex(0xff0000);
        puntos += 2;
    }
    
}

//--------------------------------------------------------------
bool ofApp::shouldRemoveBrick(ofPtr<Brick> &b) {
    return b.get()->bRemove;
}

//--------------------------------------------------------------
bool ofApp::playerCollision(Player &p) {
    //return p.bRemove;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // listen OSC messages
    while( receiver.hasWaitingMessages() ){
        // Get the next message
        ofxOscMessage m;
        receiver.getNextMessage( &m );
        // Parse message, for example:
        if( m.getAddress() == "/biggestBlobXPos" ){
            // Get first argument, biggest blog position normalized [0,1]
            float biggestBlobXPos = m.getArgAsFloat(0);
            // Use blob position for our player
            player.location.x = ofMap( biggestBlobXPos, 0,1, 0,ofGetWidth() );
        }
    }
    
    // Avanzar mundo
    box2d.update();
    
    // Seleccionar estado
    if ( estado=="precarga" ) {
        
        // Construir Ventanas
        if( ventanas.size() < 3 ){
            ofPtr <ofxBox2dRect> ventana = ofPtr <ofxBox2dRect>(new ofxBox2dRect); // crear objeto
            ventana.get()->setPhysics(0, 0.5, 0.9); // densidad, rebote, friccion
            ventana.get()->setup( box2d.getWorld(), ofRandom(0, ofGetWidth()), ofGetHeight()/2+ofRandom(-100,100), 40, 40 ); // world, x, y, w, h
            ventanas.push_back(ventana); // sumar al final del arreglo
        }
        
        // Tirar Globos
        if( bricks.size() < 20 ){
            
            if( (int)ofRandom(0, 50) == 0 ) {
                ofPtr<Brick> b = ofPtr<Brick>(new Brick); // crear objeto
                b.get()->setPhysics(0.9, 0.5, 0.1); // densidad, rebote, friccion
                b.get()->setup(box2d.getWorld(), ofRandom(0, ofGetWidth()), ofGetHeight()-20, ofRandom(20, 60)); // world, x, y, radio
                //b.get()->setVelocity(ofRandom(-30, 30), ofRandom(-30, 30));
                b.get()->setupTheCustomData(false);
                bricks.push_back(b);
            }
            
        } else {
            
            // Comenzar a jugar nivel
            estado = "nivel";
        }
        
    }
    else if ( estado=="nivel" ) {
        
        // Player
        if ( !oscTracking ) {
            player.update();
        }
        
        // Ball
        ball.update();
        
        // Detect Player Collision
        //ball.collisionPlayer(player);
        if(
           ball.location.x > player.location.x && ball.location.x < player.location.x + player.w &&
           ball.location.y >= player.location.y - ball.diametro/2
           ){
            // Cambiar sentido de la direcci—n
            ball.direction.y *= -1;
            
            // Desviar la pelota
            float d = ofDist( ball.location.x, ball.location.y, player.location.x, player.location.y );
            float error = ofRandom(-0.3,0.3);
            if ( d<player.w*0.25 ) ball.direction.x = -2 + error;
            if ( d>=player.w*0.25 && d<=player.w/2-5 ) ball.direction.x = -1 + error;
            if ( d>player.w*0.5-5 && d<player.w*0.5+5 ) ball.direction.x = 0 + error;
            if ( d>=player.w*0.5+5 && d<player.w*0.75 ) ball.direction.x = 1 + error;
            if ( d>=player.w*0.75 ) ball.direction.x = 2 + error;
        }
        
        // Detectar colisi—n con ventanas
        for( int i=0; i<ventanas.size(); i++){
            float d = ofDist( ball.location.x, ball.location.y, ventanas[i].get()->getPosition().x, ventanas[i].get()->getPosition().y );
            
            if( d < ventanas[i].get()->getWidth() ){
                // Cambiar sentido de la direcci—n
                ball.direction.y *= -1;
                printf("rebotar con ventana\n");
            }
        }
        
        // Detectar colisi—n con globos
        for( int i=0; i<bricks.size(); i++){
            float d = ofDist( ball.location.x, ball.location.y, bricks[i].get()->getPosition().x, bricks[i].get()->getPosition().y );
            
            if( d < bricks[i].get()->getRadius() ){
                
                // Cambiar sentido de la direcci—n
                ball.direction.y *= -1;
                
                // Incrementar velocidad
                if ( ball.direction.y <= 10 ){
                    ball.direction.y += 0.5;
                }
                
                // Enviar mensaje segœn tipo de globo
                if( bricks[i].get()->bName == "a" ){
                    static GameEvent newEvent;
                    newEvent.message = "reventar-globo-a";
                    ofNotifyEvent(GameEvent::events, newEvent);
                }
                
                // Eliminar ladrillo
                bricks[i].get()->bRemove = true;
            }
        }
        // Revisar si tiene que eliminar algœn globo
        ofRemove(bricks, shouldRemoveBrick);
        
        // Pierde una vida
        if ( ball.location.y > ofGetHeight() ){
            ball.direction.y = -10;
            ball.direction.x = 0;
            player.vidas--;
        }
        
        // Se queda sin vidas
        if ( player.vidas <= 0 ){
            estado = "gameOver";
        }
        
    }
    else {
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // Siempre Ventanas
    for(int i=0; i<ventanas.size(); i++) {
        ofFill();
        ofSetHexColor(0xffffff);
        ventanas[i].get()->draw();
    }
    
    // Siempre Globos
    for( int i=0; i<bricks.size(); i++){
        bricks[i].get()->draw();
    }
    
    // Segœn estado
    if ( estado=="precarga" ) {
    }
    else if ( estado=="nivel" ) {
        
        // Player
        player.draw();
        
        // Ball
        ball.draw();
    }
    else {
    }
    
    
    // Info
    ofSetColor(225);
    string info = "";
    info += "VIDAS: "+ofToString(player.vidas, 1)+"\n";
    info += "PUNTOS: "+ofToString(puntos, 1)+"\n";
    ofDrawBitmapString(info, 10, 10);
    
    
    // Debug info
    if (debug) {
        string debugTxt = "";
        debugTxt += "FPS: "+ofToString(ofGetFrameRate(), 1)+"\n";
        debugTxt += "Estado: "+ofToString(estado, 1)+"\n";
        debugTxt += "Location.x: "+ofToString(ball.location.x, 1)+"\n";
        ofDrawBitmapString(debugTxt, 30, ofGetHeight()-90);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case ' ':
            break;
        case'p':
            break;
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if ( key == 'd' || key == 'D' ){ debug = !debug; }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
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
