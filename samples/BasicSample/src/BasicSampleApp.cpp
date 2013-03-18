#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
//#include "libxtract.h"
#include "cinder/Utilities.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/FftProcessor.h"
#include "cinder/audio/PcmBuffer.h"
#include "cinder/params/Params.h"
#include "cinder/audio/FftProcessor.h"
#include "cinder/audio/Input.h"

#include "ciLibXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define BLOCKSIZE 1024
#define SAMPLERATE 44100
#define PERIOD 100
#define MFCC_FREQ_BANDS 32
#define MFCC_FREQ_MIN 20
#define MFCC_FREQ_MAX 20000


class BasicSampleApp : public AppNative {
  public:
    void prepareSettings( Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );
    void keyDown( KeyEvent event );
	void update();
	void draw();
    void shutdown();
    

    

    
	void drawWaveForm();
	void drawFft();
	
	audio::TrackRef mTrack;
	audio::PcmBuffer32fRef mPcmBuffer;
    
    audio::Input            mInput;
	std::shared_ptr<float>  mFftDataRef;
	audio::PcmBuffer32fRef  mPcmBufferLive;
    
    params::InterfaceGl mParams;
    
    float           mVolume;
    float           mFftGain;
    
    ciLibXtract         mXtract;
    double              mMean;
    shared_ptr<double>  mSpectrum;
};


void BasicSampleApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1200, 800 );
}


void BasicSampleApp::shutdown() {}


void BasicSampleApp::setup()
{
    mXtract.init();
    
    mMean       = 0.0f;
    
    mVolume     = 0.0f;
    mFftGain    = 2.0f;
    
    const std::vector<audio::InputDeviceRef>& devices = audio::Input::getDevices();
	for( std::vector<audio::InputDeviceRef>::const_iterator iter = devices.begin(); iter != devices.end(); ++iter ) {
		console() << (*iter)->getName() << std::endl;
        if ( (*iter)->getName() == "Soundflower (2ch)" )
        {
            mInput = audio::Input( *iter );
            
            mInput.start();
            
            break;
        }
            
	}
    
    mParams = params::InterfaceGl( "Params", Vec2f( 0, 0 ) );
    mParams.addParam( "Fft gain", &mFftGain, "min=0.0 max=1000.0 step=0.1" );
}


void BasicSampleApp::keyDown( KeyEvent event )
{
    char c = event.getChar();
    
    if ( c == ' ' )
    {
        mVolume = ( mVolume == 0.0f ) ? 1.0f : 0.0f;
        mTrack->setVolume( mVolume );
    }
}


void BasicSampleApp::mouseDown( MouseEvent event )
{
}


void BasicSampleApp::update()
{
    mPcmBuffer = mInput.getPcmBuffer();
	if( ! mPcmBuffer )
		return;

	mXtract.setPcmData( mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT ) );
    
    mMean       = mXtract.getMean();
    mSpectrum   = mXtract.getSpectrum();
    
//    uint16_t bandCount = 512;
//	mFftDataRef = audio::calculateFft( mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT ), bandCount );
}


void BasicSampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::color( Color::white() );

    glPushMatrix();
    glTranslatef( 0.0, 0.0, 0.0 );
    drawWaveForm();
    glTranslatef( 0.0, 200.0, 0.0 );
    drawFft();
	glPopMatrix();

    
    float meanWidth = 300;
    gl::color( Color::white() );
    glBegin( GL_QUADS );
    glVertex2f( 0,                  0 );
    glVertex2f( mMean * meanWidth,  0 );
    glVertex2f( mMean * meanWidth,  30 );
    glVertex2f( 0,                  30 );
    glEnd();
    
    mParams.draw();
}


void BasicSampleApp::drawWaveForm()
{
	if( ! mPcmBuffer )
		return;
	
	uint32_t bufferLength = mPcmBuffer->getSampleCount();
	audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
	audio::Buffer32fRef rightBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_RIGHT );
    
	int displaySize = getWindowWidth();
	float scale = displaySize / (float)bufferLength;
	
	PolyLine<Vec2f>	leftBufferLine;
	PolyLine<Vec2f>	rightBufferLine;
	
	for( int i = 0; i < bufferLength; i++ ) {
		float x = ( i * scale );
        
		//get the PCM value from the left channel buffer
		float y = ( ( leftBuffer->mData[i] - 1 ) * - 100 );
		leftBufferLine.push_back( Vec2f( x , y) );
		
		y = ( ( rightBuffer->mData[i] - 1 ) * - 100 );
		rightBufferLine.push_back( Vec2f( x , y) );
	}
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	gl::draw( leftBufferLine );
	gl::draw( rightBufferLine );
	
}


void BasicSampleApp::drawFft()
{
    mPcmBuffer = mInput.getPcmBuffer();
	
    if( ! mPcmBuffer )
		return;
    
    uint16_t    bandCount   = mPcmBuffer->getSampleCount();
	float       ht          = 1000.0f;
	float       bottom      = 150.0f;
	
	double * spectrum = mSpectrum.get();
	
	for( int i = 0; i < ( bandCount ); i++ ) {
        
		float barY = spectrum[i] * mFftGain;
		glBegin( GL_QUADS );
        glColor3f( 255.0f, 255.0f, 0.0f );
        glVertex2f( i, bottom );
        glVertex2f( i + 1, bottom );
        glColor3f( 0.0f, 255.0f, 0.0f );
        glVertex2f( i + 1, bottom - barY );
        glVertex2f( i, bottom - barY );
		glEnd();
	}
}


CINDER_APP_NATIVE( BasicSampleApp, RendererGl )


