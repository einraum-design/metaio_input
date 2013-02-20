/*
 *  testApp.cpp
 *  ofMetaioSample
 *  openFrameworks Metaio Integration
 *  Created by Stefan Wagner on 07.01.13.
 *  Design and Systems
 *
 */

#include "testApp.h"
#include <metaioSDK/IGeometry.h>


//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(60);

	num_trackers = 22;
	initMetaio();

	// in case we need GL_TEXTURE_2D for our models coords.
    //ofDisableArbTex();

	light.enable();
    glShadeModel(GL_SMOOTH);

	// floor texture
	floortex.loadImage("textures/Floor.png");


	//old OF default is 96 - but this results in fonts looking larger than in other programs. 
	ofTrueTypeFont::setGlobalDpi(72);

	font.loadFont("graphics/Gotham-Book.otf", 80, true, true);
	fontsmall.loadFont("graphics/Gotham-Book.otf", 24, true, true);
	marker_names[0] = "Gussgehäuse";
	marker_names[1] = "Antriebswelle";
	marker_names[2] = "Flugförderstation FFZ";
	marker_names[3] = "Drehteil";

}



//--------------------------------------------------------------
void testApp::update(){
	updateMetaio();

	if(metaio_foundObject == true) {
		_visibilityTweener = 1.0;
	}
	else { 
		_visibilityTweener = 0.0;
		visibilityTweener = 0.0;
	}
	visibilityTweener += (_visibilityTweener - visibilityTweener)*0.1;
}



//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	drawMetaio();

	if(metaio_foundObject) {
		ofEnableLighting();
		light.enable();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_FRONT);
		for(int j = 0; j < num_trackers; j++) {
			if(metaio_found[j] == true) {
				glLoadIdentity();
				ofPushMatrix();
				getMetaioMatrix(j);
				ofSetColor(255,180*visibilityTweener);
				floortex.draw(-250,-250,500,500);
				ofSetColor(100,100+j*50,255-j*50);
				ofPushMatrix();
				ofTranslate(0,0,75 * visibilityTweener);
				ofBox(150.0 * visibilityTweener);
				ofRotateX(-90);
				ofSetColor(255);
				font.drawString(marker_names[j%4], -80, -130);
				ofPopMatrix();
				ofPopMatrix();
			}
		}
	}
	resetMetaioMatrix();
	ofDisableLighting();
	light.disable();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);


	ofFill();
	ofSetColor(128, 199, 227);
	ofRect(0, -70, ofGetWidth()/4, 80);
	for(int j = 0; j < num_trackers; j++) {
		if(metaio_found[j] && j%4 == 0) {
			ofRect(0, -70+70*visibilityTweener, ofGetWidth()/4, 80);
			ofSetColor(255);
			fontsmall.drawString("Kunststoff", 40, 50);
		}
		ofSetColor(189, 204, 0);
		ofRect(ofGetWidth()/4, -70, ofGetWidth()/4, 80);
		if(metaio_found[j] && j%4 == 1) {
			ofRect(ofGetWidth()/4, -70+70*visibilityTweener, ofGetWidth()/4, 80);
			ofSetColor(255);
			fontsmall.drawString("Food", ofGetWidth()/4+40, 50);
		}
		ofSetColor(250, 222, 99);
		ofRect(ofGetWidth()/2, -70, ofGetWidth()/4, 80);
		if(metaio_found[j] && j%4 == 2) {
			ofRect(ofGetWidth()/2, -70+70*visibilityTweener, ofGetWidth()/4, 80);
			ofSetColor(255);
			fontsmall.drawString("Chemie", ofGetWidth()/2+40, 50);
		}
		ofSetColor(158, 115, 99);
		ofRect(3*ofGetWidth()/4, -70, ofGetWidth()/4, 80);
		if(metaio_found[j] && j%4 == 3) {
			ofRect(3*ofGetWidth()/4, -70+70*visibilityTweener, ofGetWidth()/4, 80);
			ofSetColor(255);
			fontsmall.drawString("Mineralstoffe", 3*ofGetWidth()/4+40, 50);
		}
	}
	ofSetColor(255,255,255,255);
	//ofDrawBitmapString("framerate: "+ofToString(ofGetFrameRate(), 2), 10, 15);
}



void testApp::onNewCameraFrame( metaio::ImageStruct* cameraFrame )
{ 
	camTex.loadData(cameraFrame->buffer, cameraFrame->width, cameraFrame->height, GL_BGR_EXT );
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}


//--------------------------------------------------------------
void testApp::initMetaio(){
		/* --- BEGIN INTITALIZE METAIO SDK ---------------------------------------- */
	// GETTING CUSTOM IMAGE FOR PROCESSING - NEEDS PRO LICENSE
	
	camWidth 		= 640;	// try to grab at this size. 
	camHeight 		= 480;
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(camWidth,camHeight);
	videoMirror 	= new unsigned char[camWidth*camHeight*3];
	videoTexture.allocate(camWidth,camHeight, GL_RGB);	

	videoPx = new metaio::ImageStruct(videoInverted, camWidth, camHeight, metaio::common::ECF_GRAY, true);

	int u0=0, v0=0;
	//int wndWidth=1280, wndHeight=1024;
	int wndWidth=640, wndHeight=480;
	// create the SDK
	m_metaioSDK = metaio::CreateMetaioSDKWin32();
	
	//m_metaioSDK->initializeRenderer(wndWidth, wndHeight, u0, v0,  handler, metaio::ERENDER_SYSTEM_NULL );
	m_metaioSDK->initializeRenderer(wndWidth, wndHeight, metaio::ESCREEN_ROTATION_0, (HWND)0, metaio::ERENDER_SYSTEM_NULL);
	m_sensors = metaio::CreateSensorsComponent();
	m_metaioSDK->registerSensorsComponent( m_sensors );
	// set the callback to this class
	m_metaioSDK->registerCallback( this );
	// activate 1st camera
	//m_metaioSDK->startCamera( 0 );
	//m_metaioSDK->setCameraParameters("camParams.xml");

	// load your favorite configuration
	bool result = m_metaioSDK->setTrackingConfiguration( "TrackingData_MarkerlessFast.xml" );
	cout << "tracking data loaded:" << result;
	/* --- END INTITALIZE METAIO SDK ---------------------------------------- */
	arPos.set(0,0,0);
	camTex.allocate(wndWidth, wndHeight, GL_RGB);
	metaio_foundObject = false;
	selTr = 0;

	metaio_found = new bool[num_trackers];
	metaio_pos = new ofVec3f[num_trackers];
	metaio_rot = new ofQuaternion[num_trackers];
	for(int j = 0; j < num_trackers; j++) {
		metaio_found[j] = false;
		metaio_pos[j] = ofVec3f(0,0,0);
		metaio_rot[j] = ofQuaternion(0, 0, 0, 0);
	}
}


//--------------------------------------------------------------
void testApp::updateMetaio(){
	/* --- BEGIN UPDATE METAIO SDK ---------------------------------------- */
	// GETTING CUSTOM IMAGE FOR PROCESSING - NEEDS PRO LICENSE
	

	vidGrabber.update();
	if (vidGrabber.isFrameNew()){
		int totalPixels = camWidth*camHeight*3;
		unsigned char * pixels = vidGrabber.getPixels();

		/*for (int i = 0; i < totalPixels; i++){
			videoInverted[i] = 255- pixels[i];
		}*/

		/*
		 for (int i = 0; i < camHeight; i++) {
        for (int j = 0; j < camWidth*3; j+=3) {
           //  pixel number
            int pix1 = (i*camWidth*3) + j;
            int pix2 = (i*camWidth*3) + (j+1);
            int pix3 = (i*camWidth*3) + (j+2);
            // mirror pixel number
            int mir1 = ((camHeight-1-i)*camWidth*3) + j;
            int mir2 = ((camHeight-1-i)*camWidth*3) + (j+1);
            int mir3 = ((camHeight-1-i)*camWidth*3) + (j+2);
            // swap pixels
            videoMirror[pix1] = pixels[mir1];
            videoMirror[pix2] = pixels[mir2];
            videoMirror[pix3] = pixels[mir3];	
        }
		 }
		 */
		/*
		 		 for (int i = 0; i < camHeight; i++) {
        for (int j = 0; j < camWidth*3; j+=3) {
             pixel number
            int pix1 = (i*camWidth*3) + j;
            int pix2 = (i*camWidth*3) + (j+1);
            int pix3 = (i*camWidth*3) + (j+2);
             mirror pixel number
            int mir1 = (i*camWidth*3)+1 * (camWidth*3 - j-3);
            int mir2 = (i*camWidth*3)+1 * (camWidth*3 - j-2);
            int mir3 = (i*camWidth*3)+1 * (camWidth*3 - j-1);
             swap pixels
            videoMirror[pix1] = pixels[mir1];
            videoMirror[pix2] = pixels[mir2];
            videoMirror[pix3] = pixels[mir3];	
        }
    }
	*/

		videoTexture.loadData(pixels, camWidth,camHeight, GL_BGR_EXT);
		videoPx->buffer = pixels;
		//cout << videoPx->buffer << endl;
		m_metaioSDK->setImage(*videoPx);



	}
	// m_metaioSDK->setImage(videoTexture);
	
	/*
	metaio::Vector2di test = m_metaioSDK->setImage("C:/test.jpg");
	cout << test.x << " " << test.y << endl;
	*/
	for(int j = 0; j < num_trackers; j++) {
		metaio_found[j] = false;
	}
	metaio_foundObject = false;
	//if(ofGetFrameNum()%30 > 15) m_metaioSDK->setImage("C:/test.jpg");
	//else m_metaioSDK->setImage("C:/2.jpg");
	selTr = 0;
	// do capture, tracking and rendering
	m_metaioSDK->requestCameraImage();
	m_metaioSDK->render();
	//  set the geometry to the detected COS
	std::vector< metaio::TrackingValues > trackingValues = m_metaioSDK->getTrackingValues();
	if( trackingValues.size()>0 )
	{
		metaio_foundObject = true;
		for(int i = 0; i < trackingValues.size(); i++) {
			for(int j = 0; j < num_trackers; j++) {
				if(trackingValues[i].cosName == "MarkerlessCOS" + ofToString(j+1)) {
					metaio_found[j] = true;
					metaio_pos[j].set(trackingValues[i].translation.x, trackingValues[i].translation.y, trackingValues[i].translation.z);
					metaio_rot[j] = ofQuaternion(trackingValues[i].rotation.getQuaternion().x, trackingValues[i].rotation.getQuaternion().y, trackingValues[i].rotation.getQuaternion().z, trackingValues[i].rotation.getQuaternion().w);
					cout << "found marker: " << j << "pos: " << metaio_pos[j] << endl;
				}
			}
		}
	}
	/* --- END UPDATE METAIO SDK ---------------------------------------- */
}


//--------------------------------------------------------------
void testApp::drawMetaio(){
		// GETTING CUSTOM IMAGE FOR PROCESSING - NEEDS PRO LICENSE
	
	ofSetHexColor(0xffffff);
	//vidGrabber.draw(20,20);
	//videoTexture.draw(20+camWidth,20,camWidth,camHeight);
	
	

	/* --- DRAW CAMERA IMAGE ---------------------------------------- */
	// TO DO: SWAP RED AND BLUE CHANNELS
	/*if(camTex.isAllocated()) {
		cout << "load the pixels";
		camTex.readToPixels(px);
		cout << "swap rgb";
		px.swapRgb();
		camTex.loadData(px);
	}*/
	
	glDisable(GL_DEPTH_TEST);
	ofSetColor(255);
	camTex.draw(0,0, ofGetWidth(), ofGetHeight());
	glEnable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------
void testApp::getMetaioMatrix(int j){
	ofTranslate(metaio_pos[j].x*1.5, metaio_pos[j].y*1.5, metaio_pos[j].z);
	ofVec3f qaxis; float qangle;  
	metaio_rot[j].getRotate(qangle, qaxis);  
	ofRotate(qangle, qaxis.x, qaxis.y, qaxis.z);  
}

//--------------------------------------------------------------
void testApp::resetMetaioMatrix(){
	int w, h;

	w = ofGetWidth();
	h = ofGetHeight();

	float halfFov, theTan, screenFov, aspect;
	screenFov 		= 60.0f;

	float eyeX 		= (float)w / 2.0;
	float eyeY 		= (float)h / 2.0;
	halfFov 		= PI * screenFov / 360.0;
	theTan 			= tanf(halfFov);
	float dist 		= eyeY / theTan;
	float nearDist 	= dist / 10.0;	// near / far clip plane
	float farDist 	= dist * 10.0;
	aspect 			= (float)w/(float)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(screenFov, aspect, nearDist, farDist);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, dist, eyeX, eyeY, 0.0, 0.0, 1.0, 0.0);

	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
  	glTranslatef(0, -h, 0);       // shift origin up to upper-left corner.
}