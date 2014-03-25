#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/audio2/Voice.h"
#include "cinder/audio2/Context.h"
#include "cinder/audio2/Scope.h"
#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define WIDGET_SIZE Vec2i( 150, 40 )

class BasicAudio2App : public AppNative {
  public:
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();
	void drawData( ciXtractFeatureRef feature, Rectf rect );
    
	audio2::VoiceRef			mVoice;
	audio2::ScopeSpectralRef	mScopeSpectral;

    ciXtractRef                 mXtract;
    vector<ciXtractFeatureRef>  mFeatures;
};


void BasicAudio2App::setup()
{
	mVoice = audio2::Voice::create( audio2::load( loadAsset( "Blank__Kytt_-_08_-_RSPN.mp3" ) ) );

	mVoice->setVolume( 1.0f );
	mVoice->setPan( 0.5f );

	mVoice->play();

	auto ctx = audio2::Context::master();

	auto scopeFmt = audio2::ScopeSpectral::Format().fftSize( FFT_SIZE ).windowSize( PCM_SIZE );
	mScopeSpectral = ctx->makeNode( new audio2::ScopeSpectral( scopeFmt ) );

	mVoice->getNode() >> mScopeSpectral >> ctx->getOutput();

	ctx->start();

	// Initialise xtract and get feature refs.
    mXtract     = ciXtract::create();
    mFeatures   = mXtract->getFeatures();

	// Features are disabled by default, enableFeature() also enable each feature dependencies
    for( auto k=0; k < XTRACT_FEATURES; k++ )
        mXtract->enableFeature( (xtract_features_)k );

	setWindowSize( 800, 800 );
}


void BasicAudio2App::update()
{

	mXtract->update( mScopeSpectral->getBuffer().getData() );
}


void BasicAudio2App::draw()
{
	gl::clear( Color::gray( 0.85f ) ); 	
    gl::enableAlphaBlending();
    
    if ( mXtract->isCalibrating() )
        gl::drawString( "CALIBRATION IN PROGRESS", Vec2f( 15, getWindowHeight() - 20 ), Color::black() );
    else
        gl::drawString( "Press 'c' to run the calibration", Vec2f( 15, getWindowHeight() - 20 ), Color::black() );
	
    Vec2f initPos( 15, 25 );
    Vec2f pos = initPos;
    
    for( auto k=0; k < mFeatures.size(); k++ )
    {
        drawData( mFeatures[k], Rectf( pos, pos + WIDGET_SIZE ) );
        
        pos.y += WIDGET_SIZE.y + 25;
        
        if ( pos.y >= getWindowHeight() - WIDGET_SIZE.y * 2 )
        {
            pos.x += WIDGET_SIZE.x + initPos.x;
            pos.y = initPos.y;
        }
    }
}


void BasicAudio2App::drawData( ciXtractFeatureRef feature, Rectf rect )
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


void BasicAudio2App::keyDown( KeyEvent event )
{
    char c = event.getChar();
    
    if ( c == 'c' && mXtract )
		mXtract->calibrateFeatures();

	else if ( c == ' ' )
	{
		// By stopping the Voice first if it is already playing, we always begin playing from the beginning
		if( mVoice->isPlaying() )
			mVoice->stop();

		mVoice->play();
	}
}


CINDER_APP_NATIVE( BasicAudio2App, RendererGl )
