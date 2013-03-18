#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "libxtract.h"

#include "cinder/audio/Io.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/FftProcessor.h"
#include "cinder/audio/PcmBuffer.h"
#include "cinder/params/Params.h"


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
    void runX();
    void shutdown();
    
    double mInput[BLOCKSIZE];
    double mSpectrum[BLOCKSIZE];
    double mMfccs[MFCC_FREQ_BANDS];
    double mArgd[4];
    double mMean;
    
    xtract_mel_filter mel_filters;
    
	void drawWaveForm();
	void drawFft();
	
	audio::TrackRef mTrack;
	audio::PcmBuffer32fRef mPcmBuffer;
    
    params::InterfaceGl mParams;
    
    float           mVolume;
    float           mFftGain;
};


void BasicSampleApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1200, 800 );
}


void BasicSampleApp::shutdown()
{
    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
        free( mel_filters.filters[n] );
    free( mel_filters.filters );
}


void BasicSampleApp::setup()
{
    mMean       = 0.0f;
    
    mVolume     = 0.0f;
    mFftGain    = 2.0f;
    
    
    //add the audio track the default audio output
	mTrack = audio::Output::addTrack( audio::load( loadAsset( "lords.mp3" ) ) );
	mTrack->setVolume( mVolume );
    
	//you must enable enable PCM buffering on the track to be able to call getPcmBuffer on it later
	mTrack->enablePcmBuffering( true );
    
    
    xtract_init_fft( BLOCKSIZE, XTRACT_SPECTRUM );
    
    
    mArgd[0] = SAMPLERATE / (double)BLOCKSIZE;
    mArgd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    mArgd[2] = 0.f; /* No DC component */
    mArgd[3] = 0.f; /* No Normalisation */
  
    for(int n = 0; n < BLOCKSIZE; ++n)
        mInput[n] = ((n % PERIOD) / (double)PERIOD) - .5;
    
    mel_filters.n_filters = MFCC_FREQ_BANDS;
    
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    
    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
        mel_filters.filters[n] = (double *)malloc(BLOCKSIZE * sizeof(double));
    
    xtract_init_mfcc( BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters );
    
    
    mParams = params::InterfaceGl( "Params", Vec2f( 0, 0 ) );
    mParams.addParam( "Fft gain", &mFftGain, "min=0.0 max=10.0" );
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


void BasicSampleApp::runX()
{
    
	if( ! mPcmBuffer ) return;
//    
//	std::shared_ptr<float> fftRef = audio::calculateFft( mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT ), MFCC_FREQ_BANDS );
//	if( ! fftRef ) {
//		return;
//	}
//	float * fftBuffer = fftRef.get();
//    
//    for( size_t k=0; k < BLOCKSIZE; k++ )
//        mInput[k] = fftBuffer[k];
    
	uint32_t bufferLength = mPcmBuffer->getSampleCount();
	audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
    
    
	for( size_t k=0; k < BLOCKSIZE; k++ )
        mInput[k] = leftBuffer->mData[k];
        
    xtract[XTRACT_MEAN]( mInput, BLOCKSIZE, NULL, &mMean );
    
    xtract[XTRACT_SPECTRUM]( mInput, BLOCKSIZE, mArgd, mSpectrum );
    
    xtract_mfcc( mSpectrum, BLOCKSIZE >> 1, &mel_filters, mMfccs );    
}


void BasicSampleApp::update()
{    
	mPcmBuffer = mTrack->getPcmBuffer();
    
    runX();
}

void BasicSampleApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

    gl::color( Color::white() );
//    
//    Vec2f offset( 20, 200 );
//    
//    float w = 10;
//    
//    glBegin( GL_QUADS );
//    for( size_t k=0; k < MFCC_FREQ_BANDS; k++ )
//    {
//        gl::vertex( offset + Vec2f( k * w, mMfccs[k] ) );
//        gl::vertex( offset + Vec2f( ( k + 1 ) * w, mMfccs[k] ) );
//        gl::vertex( offset + Vec2f( ( k + 1 ) * w, 0 ) );
//        gl::vertex( offset + Vec2f( k * w, 0 ) );
//    }
//    glEnd();
    
    glPushMatrix();
    glTranslatef( 0.0, 0.0, 0.0 );
    drawWaveForm();
    glTranslatef( 0.0, 200.0, 0.0 );
    drawFft();
	glPopMatrix();

    
    mParams.draw();
}


void BasicSampleApp::drawWaveForm()
{
	//if the buffer is null, for example if this gets called before any PCM data has been buffered
	//don't do anything
	if( ! mPcmBuffer ) {
		return;
	}
	
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
    float ht = 200;//getWindowHeight();
    float w = 1;
    int c = 0;
    int z =0;
    gl::pushMatrices();
    
	//draw the bands
	for( int i = 0; i < ( BLOCKSIZE ); i+=2 )
    {
//        float barY = mSpectrum[i] / MFCC_FREQ_BANDS * ht;
        float barY = mFftGain * mSpectrum[i] / MFCC_FREQ_BANDS * ht;
//        float barY = mFftGain * mSpectrum[i] * ht;
		glBegin( GL_QUADS );
        glColor3f( 255.0f, 255.0f, 0.0f );
        glVertex2f( c, ht );
        glVertex2f( c + w, ht );
        glColor3f( 0.0f, 255.0f, 0.0f );
        glVertex2f( c + w, ht - barY );
        glVertex2f( c, ht - barY );
		glEnd();
        
        c += w+1;
        
        z++;
	}
    
    gl::popMatrices();
    return;
    
    console() << z << endl;
    gl::translate( 0, 250 );

    c = 0;
	for( int i = 1; i < ( BLOCKSIZE ); i+=2 )
    {
        float barY = mFftGain * mSpectrum[i] * ht;
		glBegin( GL_QUADS );
        glColor3f( 255.0f, 255.0f, 0.0f );
        glVertex2f( c, ht );
        glVertex2f( c + w, ht );
        glColor3f( 0.0f, 255.0f, 0.0f );
        glVertex2f( c + w, ht - barY );
        glVertex2f( c, ht - barY );
		glEnd();
        
        c += w+1;
	}
    
    gl::popMatrices();
}


CINDER_APP_NATIVE( BasicSampleApp, RendererGl )


