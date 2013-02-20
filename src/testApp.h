/*
 *  testApp.h
 *  ofMetaioSample
 *  openFrameworks Metaio Integration
 *  Created by Stefan Wagner on 07.01.13.
 *  Design and Systems
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofVboMesh.h"

#include <metaioSDK/IMetaioSDKWin32.h>

class testApp : public ofBaseApp, metaio::IMetaioSDKCallback{
	
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
		// Metaio Stuff
		void initMetaio();
		void updateMetaio();
		void drawMetaio();
		void getMetaioMatrix(int j);
		void resetMetaioMatrix();
		// derived from IUnifeyeMobileCallback
		virtual void onNewCameraFrame( metaio::ImageStruct*  cameraFrame);

		ofVideoGrabber 		vidGrabber;
		unsigned char * 	videoInverted;
		unsigned char * videoMirror;
		ofTexture			videoTexture;
		int 				camWidth;
		int 				camHeight;

		metaio::IMetaioSDKWin32*	m_metaioSDK;
		metaio::IGeometry* m_geometry;
		metaio::ISensorsComponent*		m_sensors;
		bool 	m_requestingClose;

		int num_trackers;
		bool metaio_foundObject;
		bool * metaio_found;
		ofVec3f * metaio_pos;
		ofQuaternion * metaio_rot;
		int selTr;
		ofTexture camTex;
		ofPixels px;
		ofVec3f arPos;
		ofQuaternion arRot;

		metaio::ImageStruct* videoPx;


private:
        ofLight	light;
		ofImage floortex;
		ofTrueTypeFont	font, fontsmall;
		string marker_names[4];

		float visibilityTweener, _visibilityTweener;
};
