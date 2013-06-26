/*
 *  ciLibXtract BasicSampleApp
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

#include "ciLibXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicSampleApp : public AppNative {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();

    void drawData( string label, std::shared_ptr<double> data, int N, float gain, Rectf rect, Color col, bool clamp );
    
    void initGui();
    
    audio::Input            mInput;
    ciLibXtractRef          mLibXtract;
    
    params::InterfaceGlRef  mParams;
    
    bool                    mDebug;
    
    gl::TextureFontRef      mFontSmall;
};


void BasicSampleApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1200, 800 );
}


void BasicSampleApp::setup()
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
    
    
    mDebug  = true;
    
    initGui();
    
    mFontSmall = gl::TextureFont::create( Font( "Helvetica", 12 ) );
}


void BasicSampleApp::mouseDown( MouseEvent event )
{
}


void BasicSampleApp::update()
{
    mLibXtract->update();
}


void BasicSampleApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    gl::enableAlphaBlending();
    
    std::shared_ptr<double> spectrum = mLibXtract->getVectorFeature( XTRACT_SPECTRUM );
    int buffSize = PCM_SIZE >> 1;
    
    drawData( "Spectrum", mLibXtract->getVectorFeature( XTRACT_SPECTRUM ), buffSize, 500.0f, Rectf( 15, 15, 350, 150 ), Color::white(), true );
        
    if ( mDebug )
        mLibXtract->debug();
    
    mParams->draw();
}


void BasicSampleApp::drawData( string label, std::shared_ptr<double> data, int N, float gain, Rectf rect, Color col, bool clamp )
{
    int padding = 5;
    
    glPushMatrix();
    
    gl::color( ColorA( 0.3f, 0.3f, 0.4f, 0.15f ) );
    gl::drawSolidRect( rect );
    
    rect.inflate( - Vec2i::one() * padding );
    gl::translate( rect.getUpperLeft() );
    
    glBegin( GL_QUADS );
    
    float step = rect.getWidth() / N;
    float h = rect.getHeight();
    
    for( int i = 0; i < N; i++ )
    {
		float barY = data.get()[i];
        
        if ( clamp )
            barY = math<float>::clamp( barY, 0.0f, h );
        
        barY *= gain;
        
        gl::color( col );
        glVertex2f( i * step,           h );
        glVertex2f( ( i + 1 ) * step,   h );
        
        gl::color( col + Color( (float)i / (float)N, 0.0f, 1.0f - (float)i / (float)N ) );
        glVertex2f( ( i + 1 ) * step,   h-barY );
        glVertex2f( i * step,           h-barY );
		
	}
    
    glEnd();
    
    gl::color( Color::white() );
    mFontSmall->drawString( label, Vec2f( 5, 10 ) );
    
    gl::popMatrices();
}


void BasicSampleApp::initGui()
{
    mParams = params::InterfaceGl::create( "Params", Vec2f( 350, getWindowHeight() - 45 ) );
    mParams->addParam( "Debug", &mDebug );
    
    mParams->addSeparator();
    
    std::map<xtract_features_,ciLibXtract::FeatureCallback>::iterator it;
    
    mParams->addText( "Enable" );
    for( it = mLibXtract->mCallbacks.begin(); it != mLibXtract->mCallbacks.end(); ++it )
        mParams->addButton( "on_" + it->second.name, std::bind( &ciLibXtract::enableFeature, mLibXtract, it->first ) );
    
    mParams->addText( "Disable" );
    for( it = mLibXtract->mCallbacks.begin(); it != mLibXtract->mCallbacks.end(); ++it )
        mParams->addButton( "off_" + it->second.name, std::bind( &ciLibXtract::disableFeature, mLibXtract, it->first ) );
    
}


CINDER_APP_NATIVE( BasicSampleApp, RendererGl )

