#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/params/Params.h"
#include "cinder/gl/TextureFont.h"

#include "ciXtract.h"
#include "ciXtractUtilities.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class BasicSampleApp : public AppNative {
  public:

    void prepareSettings(Settings *settings);
    void setup();
    void update();
    void draw();
    
    
    ciXtractRef                     mXtract;
    
    vector<ciXtractFeatureRef>      mFeatures;
    
    audio::InputDeviceNodeRef       mInputDeviceNode;
    audio::MonitorNodeRef           mMonitorNode;
    audio::Buffer                   mPcmBuffer;
    
    params::InterfaceGl             mParams;
    float                           mGain, mOffset, mDamping, mPcmGain;
  
    gl::TextureFontRef              mFont;
};


void BasicSampleApp::prepareSettings(Settings *settings)
{
    settings->setTitle("ciXtract Sample");
    settings->setWindowSize( 1200, 800 );
}


void BasicSampleApp::setup()
{
    mFont = gl::TextureFont::create( Font( "Arial", 12 ) );
    
    auto ctx = audio::Context::master();

    vector<audio::DeviceRef> devices = audio::Device::getInputDevices();
    console() << "List audio devices:" << endl;
    for( size_t k=0; k < devices.size(); k++ )
        console() << devices[k]->getName() << endl;

    // find and initialise a device by name
     audio::DeviceRef dev;

     dev = audio::Device::findDeviceByName( "Soundflower (2ch)" );                              // on OSX i use Soundflower to hijack the system audio
     
     if ( !dev )                                                                                
         dev = audio::Device::findDeviceByName( "CABLE Output (VB-Audio Virtual Cable)" );      // on Windows there is similar tool called VB Cable
    
     if ( !dev )                                                                                // initialise default input device
        mInputDeviceNode = ctx->createInputDeviceNode();
     else
         mInputDeviceNode = ctx->createInputDeviceNode( dev );

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

    // List all available features, this prints out the enumerator that can be used to select or toggle the feature
    mXtract->listFeatures();

    // Features are disabled by default, call enableFeature() to enable each feature and its dependencies
    // You may notice a couple of "FEATURE NOT FOUND!" messages in the console, some LibXtract features are not supported yet.
    mXtract->enableAllFeatures();

    mGain       = 1.0f;
    mOffset     = 0.0f;
    mDamping    = 0.02f;
    mPcmGain    = 1.0f;

    mParams = params::InterfaceGl( "params", Vec2i( 230, 250 ) );
    mParams.setPosition( Vec2i( getWindowWidth() - 270, 90 ) );
    mParams.addParam( "Features Gain",      &mGain ).step( 0.01f );
    mParams.addParam( "Features Offset",    &mOffset ).step( 0.01f ).min( -1.0f ).max( 1.0f );
    mParams.addParam( "Features Damping",   &mDamping ).step( 0.01f ).max( 1.0f );
    mParams.addParam( "Pcm Gain",           &mPcmGain ).step( 0.01f );
}


void BasicSampleApp::update()
{
    mPcmBuffer = mMonitorNode->getBuffer();
    
    if ( !mPcmBuffer.isEmpty() )
        mXtract->update( mPcmBuffer.getData(), mPcmGain );
    else
        console() << "no PCM buffer" << endl;
}


void BasicSampleApp::draw()
{
    gl::enableAlphaBlending();
    
    gl::clear( Color::gray( 0.1f ) );
    
    ciXtractUtilities::drawPcm( Rectf( 0.0f, 0.0f, getWindowWidth(), 60.0f ), &mPcmBuffer );
    
     Vec2f   widgetSize  = Vec2f( 160.0f, 40.0f );
     Vec2f   initPos     = Vec2f( 15.0f, 100.0f );
     Vec2f   pos         = initPos;
     ColorA  bgCol       = ColorA( 0.0f, 0.0f, 0.0f, 0.1f );
     ColorA  labelCol    = Color::gray( 0.35f );
     ColorA  plotCol;
     Rectf   rect;
     
     for( size_t k=0; k < mFeatures.size(); k++ )
     {
         if ( !mFeatures[k]->isEnable() )
             continue;
         
         mFeatures[k]->setGain( mGain );
         mFeatures[k]->setOffset( mOffset );
         mFeatures[k]->setDamping( mDamping );
         
         rect    = Rectf( pos, pos + widgetSize );
         plotCol = ColorA( 1.0f, rect.y1 / getWindowHeight(), rect.x1 / getWindowWidth(), 1.0f );
         
         ciXtractUtilities::drawData( mFeatures[k], rect, mFont, plotCol, bgCol, labelCol );
         
         pos.y += widgetSize.y + 25;
         if ( pos.y >= getWindowHeight() - widgetSize.y * 2 )
             pos = Vec2i( pos.x + widgetSize.x + initPos.x, initPos.y );
     }
    
    mParams.draw();
    
    mFont->drawString( to_string( getAverageFps() ), Vec2f( getWindowWidth() - 80, 15 ) );
}


CINDER_APP_NATIVE( BasicSampleApp, RendererGl )
