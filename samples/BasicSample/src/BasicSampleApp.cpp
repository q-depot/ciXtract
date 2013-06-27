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
#include "cinder/Utilities.h"

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
    
    void drawVectorData( string label, std::shared_ptr<double> data, int N, bool enable, float gain, Rectf rect, bool clamp );
    
    void drawScalarData( string label, double val, bool enable, float gain, Vec2f pos, bool clamp );
    
    void initGui();
    
    audio::Input            mInput;
    ciLibXtractRef          mLibXtract;
    
    params::InterfaceGlRef  mParams;
    
    gl::TextureFontRef      mFontSmall;
    
    float                   mGain[XTRACT_FEATURES];
};


void BasicSampleApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1200, 880 );
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
    
    for( auto k=0; k < XTRACT_FEATURES; k++ )
        mGain[k] = 1.0f;
    
    initGui();
    
    mFontSmall = gl::TextureFont::create( Font( "Helvetica", 12 ) );

//    setFullScreen(true, FullScreenOptions().kioskMode() );
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
    
    int fftSize = PCM_SIZE >> 1;
    
    
    
    std::map<xtract_features_,ciLibXtract::FeatureCallback>::iterator it;
    ciLibXtract::FeatureCallback f;
    
    Vec2f scalarDataOffset( 25, 15 );
    float margin = 35;
    string name;
    Rectf vectorDataOffset( 480, 15, 760, 100 );
    for( it = mLibXtract->mCallbacks.begin(); it != mLibXtract->mCallbacks.end(); ++it )
    {
        f = it->second;
        name = f.name; name.erase( name.begin(), name.begin() + 7 );
        
        if ( f.type == ciLibXtract::SCALAR_FEATURE )
        {
            drawScalarData( name, mLibXtract->getScalarFeature( it->first ), f.enable, 500.0f, scalarDataOffset, true );
            scalarDataOffset += Vec2f( 0, margin );
        }
        else if ( it->first == XTRACT_SPECTRUM || it->first == XTRACT_PEAK_SPECTRUM || it->first == XTRACT_HARMONIC_SPECTRUM )
        {
            drawVectorData( name, mLibXtract->getVectorFeature( it->first ), fftSize, f.enable, 500.0f, vectorDataOffset, true );
            vectorDataOffset += Vec2f( 0, 15 + vectorDataOffset.getHeight() );
        }
        
        else if ( it->first == XTRACT_BARK_COEFFICIENTS )
        {
            drawVectorData( name, mLibXtract->getVectorFeature( it->first ), XTRACT_BARK_BANDS, f.enable, 500.0f, vectorDataOffset, true );
            vectorDataOffset += Vec2f( 0, 15 + vectorDataOffset.getHeight() );
        }
        
        else if ( it->first == XTRACT_MFCC )
        {
            drawVectorData( name, mLibXtract->getVectorFeature( it->first ), MFCC_FREQ_BANDS, f.enable, 500.0f, vectorDataOffset, true );
            vectorDataOffset += Vec2f( 0, 15 + vectorDataOffset.getHeight() );
        }
        
        
        if ( scalarDataOffset.y > getWindowHeight() - 45 )
        {
            scalarDataOffset.y = 25;
            scalarDataOffset.x += 230;
        }
    }
    
    
    mParams->draw();
}


void BasicSampleApp::drawVectorData( string label, std::shared_ptr<double> data, int N, bool enable, float gain, Rectf rect, bool clamp )
{
    int padding = 5;
    
    Color col = enable ? Color::white() : Color::gray(0.5f);
    
    glPushMatrix();
    
    gl::color( col * 0.2f );
    gl::drawSolidRect( rect );
    
    rect.inflate( - Vec2i::one() * padding );
    gl::translate( rect.getUpperLeft() );
    
    glBegin( GL_QUADS );
    
    float step = rect.getWidth() / N;
    float h = rect.getHeight();
    
    for( int i = 0; i < N; i++ )
    {
		float barY = data.get()[i] * gain;
        
        if ( clamp )
            barY = math<float>::clamp( barY, 0.0f, h );
        
        gl::color( col );
        glVertex2f( i * step,           h );
        glVertex2f( ( i + 1 ) * step,   h );
        glVertex2f( ( i + 1 ) * step,   h-barY );
        glVertex2f( i * step,           h-barY );
	}
    
    glEnd();
    
    gl::color( col );
    mFontSmall->drawString( label, Vec2f( 5, 10 ) );
    
    gl::popMatrices();
}


void BasicSampleApp::drawScalarData( string label, double val, bool enable, float gain, Vec2f pos, bool clamp )
{
    float h         = 5;
    float w         = 200;
    float margin    = 10;
    
    Color col = enable ? Color::white() : Color::gray(0.5f);
    
    val *= gain;
    
    if ( clamp )
        val = math<float>::clamp( val, 0.0f, 1.0f );
    
    glPushMatrix();
    
    gl::translate( pos );
    
    gl::color( col );
    mFontSmall->drawString( label, Vec2f::zero() );
    
    glBegin( GL_QUADS );
    
    gl::color( col * 0.1f );
    glVertex2f( 0, margin );
    glVertex2f( w, margin );
    glVertex2f( w, margin + h );
    glVertex2f( 0, margin + h );
    
    gl::color( col );
    glVertex2f( 0, margin );
    glVertex2f( w * val, margin );
    glVertex2f( w * val, margin + h );
    glVertex2f( 0, margin + h );
    
    glEnd();
    
    gl::popMatrices();
}


void BasicSampleApp::initGui()
{
    mParams = params::InterfaceGl::create( "Params", Vec2f( 350, getWindowHeight() - 45 ) );
    mParams->setOptions( "", "position='" + toString( getWindowWidth() - 380 ) + " " + toString( 15 ) + "'");
    
    mParams->addSeparator();
    
    std::map<xtract_features_,ciLibXtract::FeatureCallback>::iterator it;
    string name;
    
    for( it = mLibXtract->mCallbacks.begin(); it != mLibXtract->mCallbacks.end(); ++it )
    {
        name = it->second.name; name.erase( name.begin(), name.begin()+7 );
        mParams->addButton( name, std::bind( &ciLibXtract::toggleFeature, mLibXtract, it->first ) );
    }
}


CINDER_APP_NATIVE( BasicSampleApp, RendererGl )

