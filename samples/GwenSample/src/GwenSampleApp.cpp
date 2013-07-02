/*
 *  ciLibXtract GwenSampleApp
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

#include "ciLibXtract.h"

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

    void toggleFeature( Gwen::Event::Info info );
    
    audio::Input            mInput;
    ciLibXtractRef          mLibXtract;
    
    params::InterfaceGlRef  mParams;
    
    float                   mGain[XTRACT_FEATURES];
    
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
    
    mLibXtract = ciLibXtract::create( mInput );
    mLibXtract->enableFeature( XTRACT_SPECTRUM );
    
    for( auto k=0; k < XTRACT_FEATURES; k++ )
        mGain[k] = 1.0f;
    
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
    mLibXtract->update();
}


void GwenSampleApp::draw()
{
	gl::clear( Color( 0.91f, 0.94f, 0.96f ) );
    
    gl::enableAlphaBlending();
    
	mCanvas->RenderCanvas();
    
    gl::color( Color::white() );
    gl::draw( mLogoTex, Vec2f( 15, 8 ) );

    gl::color( Color::gray( 0.2f ) );
//    mFontSmall->drawString( toString( (int)getAverageFps() ) + " FPS",  Vec2f( 110, 28 ) );
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
    
    string label;
    
    // Scalar Features
    Vec2f initOffset( 15, 50 );
    offset = Vec2f( VECTOR_CONTROL_WIDTH + 30, initOffset.y );
    std::vector<ciLibXtract::FeatureCallback>::iterator itr;
    
    for( itr = mLibXtract->mCallbacks.begin(); itr != mLibXtract->mCallbacks.end(); ++itr )
    {
        if ( itr->type != ciLibXtract::SCALAR_FEATURE )
            continue;

        label = itr->name;
        label.erase( 0, 7 );
        
        // Custom Control
        ScalarWidget *control = new ScalarWidget( mCanvas, label, &(*itr), mLibXtract );
        control->SetPos( offset.x, offset.y );
        
        offset.y += SCALAR_CONTROL_HEIGHT + 25;

        if ( offset.y >= getWindowHeight() - SCALAR_CONTROL_HEIGHT )
        {
            offset.x += SCALAR_CONTROL_WIDTH + 15;
            offset.y = initOffset.y;
        }
    }
    
    // Vector Features
    offset = initOffset;
    for( itr = mLibXtract->mCallbacks.begin(); itr != mLibXtract->mCallbacks.end(); ++itr )
    {
        if ( itr->type != ciLibXtract::VECTOR_FEATURE )
            continue;
        
        label = itr->name;
        label.erase( 0, 7 );
        
        // Custom Control
        VectorWidget *control = new VectorWidget( mCanvas, label, &(*itr), mLibXtract );
        control->SetPos( offset.x, offset.y );
        
        offset.y += VECTOR_CONTROL_HEIGHT + 25;
        
        if ( offset.y >= getWindowHeight() - VECTOR_CONTROL_HEIGHT )
        {
            offset.x += VECTOR_CONTROL_WIDTH + 15;
            offset.y = 15;
        }
    }

}


void GwenSampleApp::toggleFeature( Gwen::Event::Info info )
{
//    if ( info.Packet->Integer == -1 )
//        toggleAllFeatures(true);
//    else if ( info.Packet->Integer == -2 )
//        toggleAllFeatures(false);
//    else
//    {
//        mLibXtract->toggleFeature( (xtract_features_)info.Packet->Integer );
//    }
}


void GwenSampleApp::toggleAllFeatures( Gwen::Controls::Base* pControl )
{
    Gwen::Controls::Button  *button = (Gwen::Controls::Button*)pControl;
    bool                    enable  = false;
    
    if ( button->GetText() == "All on" )
        enable = true;

    std::vector<ciLibXtract::FeatureCallback>::iterator it;
    for( it = mLibXtract->mCallbacks.begin(); it != mLibXtract->mCallbacks.end(); ++it )
        if ( enable && it->type == ciLibXtract::SCALAR_FEATURE )
            mLibXtract->enableFeature( it->feature );
        else
            mLibXtract->disableFeature( it->feature );
}

CINDER_APP_NATIVE( GwenSampleApp, RendererGl )


