#pragma once

#include "ofMain.h"
//Inlcude the main openframeworks GRT header
#include "ofGRT/ofGRT.h"
//Inlcude the main openframeworks GRT header
#include "GRT/GRT.h"
#include "openniProxie.h"
//#include "NiteSampleUtilities.h"
#include <Windows.h>
#include "Nite_HandTracker.h"

//State that we are using the GRT namespace
using namespace GRT;
using namespace nite;

#define TIME_SERIES_GRAPH_WIDTH 500
#define DEFAULT_PREP_TIME 10000
#define DEFAULT_RECORD_TIME 20000


class static_training : public ofBaseApp{

private:
	openniProxie oniprox;

public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();
    
    void setupGraphs();
    vector<double> point3fToVectorDouble(Point3f points[], int nPoints);
    vector<double> point3fToVectorDouble(Point3f points);

    vector< double > leftHand;
    vector< double > rightHand;
    
    TimeSeriesGraph leftHandGraph;
    TimeSeriesGraph rightHandGraph;
    
    string infoText;
    ofTrueTypeFont	font;
    
    GestureRecognitionPipeline pipeline;
    LabelledClassificationData trainingData;
    TrainingDataRecordingTimer trainingTimer;
    bool trainingModeActive;
    bool predictionModeActive;
    UINT trainingClassLabel;
    
};
