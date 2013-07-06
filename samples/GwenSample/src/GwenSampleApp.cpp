/*
 *  ciXtract GwenSampleApp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/params/Params.h"
#include "cinder/Utilities.h"

#include "ciXtract.h"

#include "cigwen/GwenRendererGl.h"
#include "cigwen/GwenInput.h"
#include "cigwen/CinderGwen.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/WindowControl.h"
#include "Gwen/Controls/CollapsibleList.h"
#include "Gwen/Controls/CheckBox.h"
#include "Gwen/Events.h"


#include "ScalarWidget.h"
#include "VectorWidget.h"

using namespace ci;
using namespace ci::app;
using namespace std;


gl::TextureFontRef      mFontSmall;
gl::TextureFontRef      mFontMedium;
gl::TextureFontRef      mFontBig;



class GwenSampleApp : public AppNative, public Gwen::Event::Handler {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	void update();
	void draw();
    
    void initGui();
    
    void toggleAllFeatures( Gwen::Controls::Base* pControl );
    void toggleCalibration( Gwen::Controls::Base* pControl );

    void drawPcmData();
    
    audio::Input                    mInput;
    ciXtractRef                     mXtract;
    
    params::InterfaceGlRef          mParams;
    
    gl::TextureRef                  mLogoTex;
    
	cigwen::GwenRendererGl          *mRenderer;
	cigwen::GwenInputRef            mGwenInput;
	Gwen::Controls::Canvas          *mCanvas;
    Gwen::Controls::WindowControl   *mWindow;
};


void GwenSampleApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1365, 880 );
}


void GwenSampleApp::setup()
{
    const std::vector<audio::InputDeviceRef>& devices = audio::Input::getDevices();
	for( std::vector<audio::InputDeviceRef>::const_iterator iter = devices.begin(); iter != devices.end(); ++iter )
    {
        if ( (*iter)->getName() == "Soundflower (2ch)" )
        {
            mInput = audio::Input( *iter );
            mInput.start();
            break;
        }
	}
    
    if ( !mInput )
        exit(-1);
    
    mXtract = ciXtract::create( mInput );
    mXtract->enableFeature( XTRACT_SPECTRUM );

    initGui();
    
    mFontSmall  = gl::TextureFont::create( Font( "Helvetica", 12 ) );
    mFontMedium = gl::TextureFont::create( Font( "Helvetica", 14 ) );
    mFontBig    = gl::TextureFont::create( Font( "Helvetica", 18 ) );
    
    //    setFullScreen(true, FullScreenOptions().kioskMode() );
    
    mLogoTex    = gl::Texture::create( loadImage( loadAsset( "nocte.png" ) ) );
}


void GwenSampleApp::mouseDown( MouseEvent event )
{
}


void GwenSampleApp::keyDown( KeyEvent event )
{
    char c = event.getChar();
    
    if ( event.isAltDown() )
    {
        if ( c == 'f' )
            setFullScreen( !isFullScreen() );
    }
}


void GwenSampleApp::update()
{
    mXtract->update();
}


void GwenSampleApp::draw()
{
    //	gl::clear( Color( 0.91f, 0.94f, 0.96f ) );
    //    gl::clear( Color( 0.94f, 0.94f, 0.94f ) );
    gl::clear( Color( 0.95f, 0.95f, 0.95f ) );

    gl::enableAlphaBlending();
    
	mCanvas->RenderCanvas();
    
    gl::color( Color::white() );
    gl::draw( mLogoTex, Vec2f( 15, 8 ) );
    
    gl::color( Color::gray( 0.2f ) );
    mFontSmall->drawString( toString( (int)getAverageFps() ) + " FPS",  Vec2f( 110, 28 ) );
    
    if ( mXtract->isCalibrating() )
    {
        gl::color( Color::gray( 0.2f ) );
        mFontSmall->drawString( "CALIBRATION IN PROGRESS",  Vec2f( 925, 26 ) );
    }
    
    drawPcmData();
}


void GwenSampleApp::initGui()
{
    // TODO: find a better way.. WTF is this for?
	fs::path rootPath = getAppPath().parent_path().parent_path().parent_path().parent_path().parent_path().parent_path();
	addAssetDirectory( rootPath / "assets" );
    
	mRenderer = new cigwen::GwenRendererGl();
	mRenderer->Init();
    
	Gwen::Skin::TexturedBase* skin = new Gwen::Skin::TexturedBase( mRenderer );
	skin->Init( "DefaultSkin.png" );
    
	mCanvas = new Gwen::Controls::Canvas( skin );
	mCanvas->SetSize( getWindowWidth(), getWindowHeight() );
	mGwenInput = cigwen::GwenInput::create( mCanvas );
    
    Vec2f offset( getWindowWidth() - 180, 12 );
    
    // Toggle all
    Gwen::Controls::Button* pButtonOn = new Gwen::Controls::Button( mCanvas );
    pButtonOn->SetText( "All on" );
    pButtonOn->SetBounds( offset.x, offset.y, 80, 20 );
    pButtonOn->onPress.Add( this, &GwenSampleApp::toggleAllFeatures );
    
    Gwen::Controls::Button* pButtonOff = new Gwen::Controls::Button( mCanvas );
    pButtonOff->SetText( "All off" );
    pButtonOff->SetBounds( offset.x + 85, offset.y, 80, 20 );
    pButtonOff->onPress.Add( this, &GwenSampleApp::toggleAllFeatures );
    
    
    Gwen::Controls::Button* pButtonCalibration = new Gwen::Controls::Button( mCanvas );
    pButtonCalibration->SetText( "Calibration" );
    pButtonCalibration->SetBounds( offset.x - 85, offset.y, 80, 20 );
    pButtonCalibration->onPress.Add( this, &GwenSampleApp::toggleCalibration );
    
    
    vector<ciXtractFeatureRef> features = mXtract->getFeatures();
    vector<ciXtractFeatureRef>::iterator itr;
    
    // Scalar Features
    Vec2f initOffset( 15, 50 );
    offset = Vec2f( CI_XTRACT_WIDGET_WIDTH + 30, initOffset.y );

    for( itr = features.begin(); itr != features.end(); ++itr )
    {
        if ( (*itr)->getType() != CI_XTRACT_SCALAR )
            continue;
        
        // Custom Control
        ScalarWidget *control = new ScalarWidget( mCanvas, (*itr)->getName(), (*itr), mXtract );
        control->SetPos( offset.x, offset.y );
        
        offset.y += CI_XTRACT_WIDGET_HEIGHT + 25;

        if ( offset.y >= getWindowHeight() - CI_XTRACT_WIDGET_HEIGHT )
        {
            offset.x += CI_XTRACT_WIDGET_WIDTH + 15;
            offset.y = initOffset.y;
        }
    }
    
    // Vector Features
    offset = initOffset;
    for( itr = features.begin(); itr != features.end(); ++itr )
    {
        if ( (*itr)->getType() != CI_XTRACT_VECTOR )
            continue;

        // Custom Control
        VectorWidget *control = new VectorWidget( mCanvas, (*itr)->getName(), (*itr), mXtract );
        control->SetPos( offset.x, offset.y );
        
        offset.y += CI_XTRACT_WIDGET_HEIGHT + 25;
        
        if ( offset.y >= getWindowHeight() - CI_XTRACT_WIDGET_HEIGHT )
        {
            offset.x += CI_XTRACT_WIDGET_WIDTH + 15;
            offset.y = 15;
        }
    }

}


void GwenSampleApp::toggleAllFeatures( Gwen::Controls::Base* pControl )
{
    Gwen::Controls::Button  *button = (Gwen::Controls::Button*)pControl;
    bool                    enable  = false;
    
    if ( button->GetText() == "All on" )
        enable = true;
    
    vector<ciXtractFeatureRef> features = mXtract->getFeatures();
    
    std::vector<ciXtractFeatureRef>::iterator it;
    for( it = features.begin(); it != features.end(); ++it )
              mXtract->enableFeature( (*it)->getEnum() );
}


void GwenSampleApp::toggleCalibration( Gwen::Controls::Base* pControl )
{
    mXtract->autoCalibration();
}

void GwenSampleApp::drawPcmData()
{
    audio::PcmBuffer32fRef pcmBuffer = mInput.getPcmBuffer();
    
	if( !pcmBuffer )
		return;
    
	uint32_t bufferLength           = pcmBuffer->getSampleCount();
	audio::Buffer32fRef leftBuffer  = pcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
    
	int displaySize = getWindowWidth();
	float scale = displaySize / (float)bufferLength;
	
	PolyLine<Vec2f>	leftBufferLine;
	
	for( int i = 0; i < bufferLength; i++ ) {
		float x = ( i * scale );
        
		//get the PCM value from the left channel buffer
		float y = ( ( leftBuffer->mData[i] - 1 ) * - 100 );
		leftBufferLine.push_back( Vec2f( x , y) );
	}
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	gl::draw( leftBufferLine );
    
    Vec2f offset = getWindowSize() - Vec2f( 200, 350 );

    mFontBig->drawString( to_string( bufferLength ), offset ); offset.y += 25;
    mFontBig->drawString( to_string( leftBuffer->mSampleCount ), offset ); offset.y += 25;
    mFontBig->drawString( to_string( leftBuffer->mNumberChannels ), offset ); offset.y += 25;
}



CINDER_APP_NATIVE( GwenSampleApp, RendererGl )


