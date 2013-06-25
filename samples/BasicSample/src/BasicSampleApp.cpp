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
    
    
    audio::Input            mInput;
    ciLibXtractRef          mLibXtract;
    
    params::InterfaceGlRef  mParams;
    
    bool                    mDebug;
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
    
    if ( mInput )
    {
        mLibXtract = ciLibXtract::create( mInput );
        mLibXtract->enableFeature( XTRACT_SPECTRUM );
    }
    
    
    mDebug  = true;
    
    mParams = params::InterfaceGl::create( "Params", Vec2f( 250, 300 ) );
    mParams->addParam( "Debug", &mDebug );
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

    std::shared_ptr<double> spectrum = mLibXtract->getVectorFeature( XTRACT_SPECTRUM );
    int buffSize = PCM_SIZE >> 1;
    
    Rectf rect(0, 0, 200, 150 );
    float step = rect.getWidth() / (float)buffSize;
    float h = rect.getHeight();
    gl::color( Color::white() );
    glBegin( GL_QUADS );
    for( auto k=0; k < buffSize; k++ )
    {
		float barY = spectrum.get()[k] * h;
        barY = math<float>::clamp( barY, 0.0f, h );
        glVertex2f( k * step,           h );
        glVertex2f( ( k + 1 ) * step,   h );
        glVertex2f( ( k + 1 ) * step,   h-barY );
        glVertex2f( k * step,           h-barY );
	}
    glEnd();
        
        
    if ( mDebug )
        mLibXtract->debug();
    
    mParams->draw();
}


CINDER_APP_NATIVE( BasicSampleApp, RendererGl )