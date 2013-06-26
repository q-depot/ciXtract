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
    
    void drawVectorData( string label, std::shared_ptr<double> data, int N, float gain, Rectf rect, bool clamp );
    
    void drawScalarData( string label, double val, float gain, Vec2f pos, bool clamp );
    
    void initGui();
    
    audio::Input            mInput;
    ciLibXtractRef          mLibXtract;
    
    params::InterfaceGlRef  mParams;
    
    bool                    mDebug;
    
    gl::TextureFontRef      mFontSmall;
    
    float                   mGain[XTRACT_FEATURES];
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
    
    for( auto k=0; k < XTRACT_FEATURES; k++ )
        mGain[k] = 1.0f;
    
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
    
    drawVectorData( "Spectrum", mLibXtract->getVectorFeature( XTRACT_SPECTRUM ), buffSize, 500.0f, Rectf( 15, 15, 350, 150 ), true );

    Vec2f scalarPos( 15, 200 );
    float margin = 35;
    
    drawScalarData( "Mean", mLibXtract->getScalarFeature( XTRACT_MEAN ), 500.0f, scalarPos, true );                                                             scalarPos += Vec2f( 0, margin );
    drawScalarData( "Variance", mLibXtract->getScalarFeature( XTRACT_VARIANCE ), 500.0f, scalarPos, true );                                                     scalarPos += Vec2f( 0, margin );
    drawScalarData( "Standard deviation", mLibXtract->getScalarFeature( XTRACT_STANDARD_DEVIATION ), 500.0f, scalarPos, true );                                 scalarPos += Vec2f( 0, margin );
    drawScalarData( "Average deviation", mLibXtract->getScalarFeature( XTRACT_AVERAGE_DEVIATION ), 500.0f, scalarPos, true );                                   scalarPos += Vec2f( 0, margin );
    drawScalarData( "Skewness", mLibXtract->getScalarFeature( XTRACT_SKEWNESS ), 500.0f, scalarPos, true );                                                     scalarPos += Vec2f( 0, margin );
    drawScalarData( "Kurtosis", mLibXtract->getScalarFeature( XTRACT_KURTOSIS ), 500.0f, scalarPos, true );                                                     scalarPos += Vec2f( 0, margin );
    drawScalarData( "Spectral mean", mLibXtract->getScalarFeature( XTRACT_SPECTRAL_MEAN ), 500.0f, scalarPos, true );                                           scalarPos += Vec2f( 0, margin );
    drawScalarData( "Spectral variance", mLibXtract->getScalarFeature( XTRACT_SPECTRAL_VARIANCE ), 500.0f, scalarPos, true );                                   scalarPos += Vec2f( 0, margin );
    drawScalarData( "Spectral standard deviation deviation", mLibXtract->getScalarFeature( XTRACT_SPECTRAL_STANDARD_DEVIATION ), 500.0f, scalarPos, true );     scalarPos += Vec2f( 0, margin );    
    drawScalarData( "Spectral skewness", mLibXtract->getScalarFeature( XTRACT_SPECTRAL_SKEWNESS ), 500.0f, scalarPos, true );                                   scalarPos += Vec2f( 0, margin );
    drawScalarData( "Spectral kurtosis", mLibXtract->getScalarFeature( XTRACT_SPECTRAL_KURTOSIS ), 500.0f, scalarPos, true );                                   scalarPos += Vec2f( 0, margin );
    drawScalarData( "Spectral centroid", mLibXtract->getScalarFeature( XTRACT_SPECTRAL_CENTROID ), 500.0f, scalarPos, true );                                   scalarPos += Vec2f( 0, margin );
    drawScalarData( "Irregularity K", mLibXtract->getScalarFeature( XTRACT_IRREGULARITY_K ), 500.0f, scalarPos, true );                                         scalarPos += Vec2f( 0, margin );
    drawScalarData( "Irregularity J", mLibXtract->getScalarFeature( XTRACT_IRREGULARITY_J ), 500.0f, scalarPos, true );                                         scalarPos += Vec2f( 0, margin );
    
    
    if ( mDebug )
        mLibXtract->debug();
    
    mParams->draw();
}


void BasicSampleApp::drawVectorData( string label, std::shared_ptr<double> data, int N, float gain, Rectf rect, bool clamp )
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
        
        gl::color( Color::white() );
        glVertex2f( i * step,           h );
        glVertex2f( ( i + 1 ) * step,   h );
        
        gl::color( Color::white() + Color( (float)i / (float)N, 0.0f, 1.0f - (float)i / (float)N ) );
        glVertex2f( ( i + 1 ) * step,   h-barY );
        glVertex2f( i * step,           h-barY );
		
	}
    
    glEnd();
    
    gl::color( Color::white() );
    mFontSmall->drawString( label, Vec2f( 5, 10 ) );
    
    gl::popMatrices();
}


void BasicSampleApp::drawScalarData( string label, double val, float gain, Vec2f pos, bool clamp )
{
    float h         = 5;
    float w         = 200;
    float margin    = 10;
    
    val *= gain;
    
    if ( clamp )
        val = math<float>::clamp( val, 0.0f, 1.0f );
    
    glPushMatrix();
    
    gl::translate( pos );
    
    gl::color( Color::white() );
    mFontSmall->drawString( label, Vec2f::zero() );
    
    glBegin( GL_QUADS );
    
    gl::color( Color::gray(0.2f) );
    glVertex2f( 0, margin );
    glVertex2f( w, margin );
    glVertex2f( w, margin + h );
    glVertex2f( 0, margin + h );
    
    gl::color( Color::gray(0.8f) );
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

