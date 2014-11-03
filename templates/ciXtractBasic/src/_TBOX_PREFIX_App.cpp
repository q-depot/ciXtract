
#include "cinder/app/AppNative.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/MonitorNode.h"

#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class _TBOX_PREFIX_App : public AppBasic {

public:

	void prepareSettings( Settings *settings );
	void setup();
	void update();
    void draw();

	void drawPcmData();

    ciXtractRef                     mXtract;

    vector<ciXtractFeatureRef>      mFeatures;

	audio::InputDeviceNodeRef		mInputDeviceNode;
	audio::MonitorNodeRef           mMonitorNode;
    audio::Buffer                   mPcmBuffer;
    
};


void _TBOX_PREFIX_App::prepareSettings(Settings *settings)
{
	settings->setTitle("ciXtract Sample");
	settings->setWindowSize( 900, 700 );
}


void _TBOX_PREFIX_App::setup()
{
    auto ctx = audio::Context::master();
    
    vector<audio::DeviceRef> devices = audio::Device::getInputDevices();
    console() << "List audio devices:" << endl;
    for( auto k=0; k < devices.size(); k++ )
        console() << devices[k]->getName() << endl;

    // find and initialise a device by name
    // audio::DeviceRef dev    = audio::Device::findDeviceByName( "Soundflower (2ch)" );
    // mInputDeviceNode        = ctx->createInputDeviceNode( dev );
    
    // initialise default input device
    mInputDeviceNode = ctx->createInputDeviceNode();
    
    // initialise MonitorNode to get the PCM data
	auto monitorFormat = audio::MonitorNode::Format().windowSize( CIXTRACT_PCM_SIZE );
	mMonitorNode = ctx->makeNode( new audio::MonitorNode( monitorFormat ) );
    
    // pipe the input device into the MonitorNode
	mInputDeviceNode >> mMonitorNode;
    
	// InputDeviceNode (and all InputNode subclasses) need to be enabled()'s to process audio. So does the Context:
	mInputDeviceNode->enable();
	ctx->enable();
 
	// Initialise ciXtract
    mXtract     = ciXtract::create();
    mFeatures   = mXtract->getFeatures();
    
	// Features are disabled by default, call enableFeature() to enable each feature and its dependencies
    // You may notice a couple of "FEATURE NOT FOUND!" messages in the console, some LibXtract features are not supported yet.
    for( auto k=0; k < XTRACT_FEATURES; k++ )
        mXtract->enableFeature( (xtract_features_)k );
}


void _TBOX_PREFIX_App::update()
{
    mPcmBuffer = mMonitorNode->getBuffer();
    
    if ( !mPcmBuffer.isEmpty() )
        mXtract->update( mPcmBuffer.getData() );
}


void _TBOX_PREFIX_App::draw()
{
	gl::clear( Color::gray( 0.85f ) ); 	
    gl::enableAlphaBlending();
    
	gl::color( Color::gray( 0.1f ) );
	drawPcmData();
    
    Vec2i   widgetSize  = Vec2f( 160, 40 );
    Vec2f   initPos     = Vec2f( 15, 100 );
    Vec2f   pos         = initPos;
    ColorA  bgCol       = ColorA( 0.0f, 0.0f, 0.0f, 0.1f );
    ColorA  plotCol;
    Rectf   rect;
    
    for( auto k=0; k < mFeatures.size(); k++ )
    {
        if ( !mFeatures[k]->isEnable() )
            continue;
        
        rect    = Rectf( pos, pos + widgetSize );
        plotCol = ColorA( 1.0f, rect.y1 / getWindowHeight(), rect.x1 / getWindowWidth(), 1.0f );
        
        mFeatures[k]->draw( rect, plotCol, bgCol );
        
        pos.y += widgetSize.y + 25;
        if ( pos.y >= getWindowHeight() - widgetSize.y )
            pos = Vec2i( pos.x + widgetSize.x + initPos.x, initPos.y );
    }
}


void _TBOX_PREFIX_App::drawPcmData()
{
    if ( mPcmBuffer.isEmpty() )
        return;
    
    // draw the first(left) channel in the PCM buffer
    // getData() returns a pointer to the first sample in the buffer
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


CINDER_APP_BASIC( _TBOX_PREFIX_App, RendererGl )


