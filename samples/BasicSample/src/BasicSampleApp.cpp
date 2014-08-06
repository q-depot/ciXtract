/*
 *  BasicSampleApp.cpp
 *
 */

#include "cinder/app/AppNative.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/MonitorNode.h"

#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define WIDGET_SIZE Vec2i( 150, 40 )


class BasicSampleApp : public AppBasic {

public:
	
	void prepareSettings( Settings *settings );
	void setup();
	void update();
    void draw();
	void keyDown( KeyEvent event );
	
	void drawPcmData();
	void drawData( ciXtractFeatureRef feature, Rectf rect );
    
    ciXtractRef                     mXtract;
	
    vector<ciXtractFeatureRef>      mFeatures;
    
    
	audio::InputDeviceNodeRef		mInputDeviceNode;
	audio::MonitorNodeRef           mMonitorNode;
    audio::Buffer                   mPcmBuffer;
    
};


void BasicSampleApp::prepareSettings(Settings *settings)
{
	settings->setTitle("ciXtract Sample");
	settings->setWindowSize( 900, 700 );
}


void BasicSampleApp::setup()
{
    auto ctx = audio::Context::master();
    
    vector<audio::DeviceRef> devices = audio::Device::getInputDevices();
    console() << "List audio devices:" << endl;
    for( auto k=0; k < devices.size(); k++ )
        console() << devices[k]->getName() << endl;

    // init Soundflower
    // audio::DeviceRef dev    = audio::Device::findDeviceByName( "Soundflower (2ch)" );
    // mInputDeviceNode        = ctx->createInputDeviceNode( dev );
    
    // init default input device
    mInputDeviceNode = ctx->createInputDeviceNode();
    
	// By providing an FFT size double that of the window size, we 'zero-pad' the analysis data, which gives
	// an increase in resolution of the resulting spectrum data.
	auto monitorFormat = audio::MonitorNode::Format().windowSize( CIXTRACT_PCM_SIZE );
	mMonitorNode = ctx->makeNode( new audio::MonitorNode( monitorFormat ) );
    
	mInputDeviceNode >> mMonitorNode;
    
	// InputDeviceNode (and all InputNode subclasses) need to be enabled()'s to process audio. So does the Context:
	mInputDeviceNode->enable();
	ctx->enable();
 
	// Initialise xtract and get feature refs.
    mXtract     = ciXtract::create();
    mFeatures   = mXtract->getFeatures();
    
	// Features are disabled by default, enableFeature() also enable each feature dependencies
    for( auto k=0; k < XTRACT_FEATURES; k++ )
        mXtract->enableFeature( (xtract_features_)k );
}


void BasicSampleApp::update()
{
    mPcmBuffer = mMonitorNode->getBuffer();
    
	mXtract->update( mPcmBuffer.getData() );
}


void BasicSampleApp::draw()
{
	gl::clear( Color::gray( 0.85f ) ); 	
    gl::enableAlphaBlending();
    
	gl::color( Color::gray( 0.1f ) );
	drawPcmData();
    
    if ( mXtract->isCalibrating() )
        gl::drawString( "CALIBRATION IN PROGRESS", Vec2f( 15, getWindowHeight() - 20 ), Color::black() );
    else
        gl::drawString( "Press 'c' to run the calibration", Vec2f( 15, getWindowHeight() - 20 ), Color::black() );

    Vec2f initPos( 15, 100 );
    Vec2f pos = initPos;
    
    for( auto k=0; k < mFeatures.size(); k++ )
    {
        drawData( mFeatures[k], Rectf( pos, pos + WIDGET_SIZE ) );
        
        pos.y += WIDGET_SIZE.y + 25;
        
        if ( pos.y >= getWindowHeight() - WIDGET_SIZE.y )
        {
            pos.x += WIDGET_SIZE.x + initPos.x;
            pos.y = initPos.y;
        }
    }
}


void BasicSampleApp::drawPcmData()
{
	uint32_t    bufferLength    = mPcmBuffer.getSize() / mPcmBuffer.getNumChannels();
    float       *leftBuffer     = mPcmBuffer.getData();

	int     displaySize = getWindowWidth();
	float   scale       = displaySize / (float)bufferLength;

	PolyLine<Vec2f>	leftBufferLine;

    gl::color( Color::gray( 0.4f ) );
	
	for( int i = 0; i < bufferLength; i++ )
    {
		float x = i * scale;
        float y = 50 + leftBuffer[i] * 60;
		leftBufferLine.push_back( Vec2f( x , y) );
	}

	gl::draw( leftBufferLine );
}


void BasicSampleApp::drawData( ciXtractFeatureRef feature, Rectf rect )
{
    ColorA bgCol    = ColorA( 0.0f, 0.0f, 0.0f, 0.1f );
    
    ColorA dataCol  = ColorA( 1.0f, rect.y1 / getWindowHeight(), rect.x1 / getWindowWidth(), 1.0f );
    
    glPushMatrix();
    
    gl::drawString( feature->getName(), rect.getUpperLeft(), Color::black() );
    
    rect.y1 += 10;
    
    gl::color( bgCol );
    gl::drawSolidRect( rect );
    
    gl::translate( rect.getUpperLeft() );
    
    glBegin( GL_QUADS );
    
    std::shared_ptr<double> data  = feature->getResult();
    int                     dataN = feature->getResultN();
    float                   min   = feature->getResultMin();
    float                   max   = feature->getResultMax();
    float                   step  = rect.getWidth() / dataN;
    float                   h     = rect.getHeight();
    float                   val, barY;
    
    gl::color( dataCol );
    
    for( int i = 0; i < dataN; i++ )
    {
        val     = ( data.get()[i] - min ) / ( max - min );
        val     = math<float>::clamp( val, 0.0f, 1.0f );
        barY    = h * val;
        
        glVertex2f( i * step,           h );
        glVertex2f( ( i + 1 ) * step,   h );
        glVertex2f( ( i + 1 ) * step,   h-barY );
        glVertex2f( i * step,           h-barY );
    }

    glEnd();

    gl::popMatrices();
}


void BasicSampleApp::keyDown( KeyEvent event )
{
    char c = event.getChar();
    
    if ( c == 'c' )
        mXtract->calibrateFeatures();
}


CINDER_APP_BASIC( BasicSampleApp, RendererGl )


