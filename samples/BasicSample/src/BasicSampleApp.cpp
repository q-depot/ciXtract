#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/FftProcessor.h"
#include "cinder/audio/PcmBuffer.h"
#include "cinder/params/Params.h"
#include "cinder/audio/FftProcessor.h"
#include "cinder/audio/Input.h"
#include "cinder/gl/TextureFont.h"

#include "ciLibXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class BasicSampleApp : public AppNative {
  public:
    void shutdown();
    void prepareSettings( Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );
    void keyDown( KeyEvent event );
	void update();
	void draw();
    
	void drawWaveForm();
	
    void drawData( double *data, int N, Vec2i pos, float height, int step, Color col );
    
    void drawData( float *data, int N, Vec2i pos, float height, int step, Color col );
    
    void drawData( double *data, int N, Rectf rect, Color col = Color::white(), bool clamp = true );

	audio::TrackRef mTrack;
	audio::PcmBuffer32fRef mPcmBuffer;
    
    audio::Input            mInput;
	std::shared_ptr<float>  mFftDataRef;
	audio::PcmBuffer32fRef  mPcmBufferLive;
    
    params::InterfaceGl mParams;
    
    float               mFftGain;
    float               mPeakThreshold;
    bool                mRenderCinderFft;
    
    ciLibXtract         mXtract;
    double              mMean;
    shared_ptr<double>  mSpectrum;
    shared_ptr<double>  mHarmonicSpectrum;
    shared_ptr<double>  mPeakSpectrum;
    shared_ptr<double>  mMfccs;
    shared_ptr<double>  mBarks;
    
    float               mF0;
    
    gl::TextureFontRef  mFontSmall;

};


void BasicSampleApp::shutdown()
{
}


void BasicSampleApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1200, 800 );
}


void BasicSampleApp::setup()
{
    mXtract.init();
    
    mMean           = 0.0f;
    mFftGain        = 2.0f;
    mPeakThreshold  = 0.5f;
    
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
    
    mParams = params::InterfaceGl( "Params", Vec2f( 0, 0 ) );
    mParams.addParam( "Peak threshold",    &mPeakThreshold,   "min=0.0 max=1000.0 step=0.1" );
    
    mParams.addParam( "ci Fft",         &mRenderCinderFft );
    
    mFontSmall = gl::TextureFont::create( Font( "Helvetica", 12 ) );
}


void BasicSampleApp::keyDown( KeyEvent event )
{
    char c = event.getChar();
    
    if ( c == ' ' )
    {
//        mVolume = ( mVolume == 0.0f ) ? 1.0f : 0.0f;
//        mTrack->setVolume( mVolume );
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
	
	audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
//	audio::Buffer32fRef rightBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_RIGHT );
    
    
	mFftDataRef = audio::calculateFft( mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT ), BLOCKSIZE >> 2 );
	
    if ( !mFftDataRef )
        return;
    
    mXtract.setSpectrum( mFftDataRef );

	mXtract.setPcmData( leftBuffer );
    
    mMean           = mXtract.getMean();
    mSpectrum       = mXtract.getSpectrum();
    mPeakSpectrum   = mXtract.getPeakSpectrum( mPeakThreshold );
    mMfccs          = mXtract.getMfcc();
    mBarks          = mXtract.getBarkCoefficients();
    mF0             = mXtract.getF0();
    mHarmonicSpectrum   = mXtract.getHarmonicSpectrum();
    
    mXtract.getAutocorrelationFft();
    
    
}


void BasicSampleApp::draw()
{
//	gl::clear( Color( 0.85f, 0.85f, 0.85f ) );
    gl::clear( Color::white() * 0.25f );
    gl::color( Color::white() );
    gl::enableAlphaBlending();
    
    
    drawWaveForm();
    
//    if ( mFftDataRef && mRenderCinderFft )
//        drawData( mFftDataRef.get(), BLOCKSIZE >> 2, Vec2i( 10, 600 ), 1.0f, 1, Color( 1.0f, 1.0f, 1.0f ) );
    
    Rectf rect( 15, 15, 15 + ( getWindowWidth() - 35 ) / 2, 150 );
    
    drawData( mSpectrum.get(),          BLOCKSIZE >> 2, rect, Color( 0.2f, 0.7f, 1.0f ) );      rect.offset( Vec2i ( rect.getWidth() + 5, 0 ) );
    drawData( mPeakSpectrum.get(),      BLOCKSIZE >> 2, rect, Color( 1.0f, 0.2f, 0.15f )  );
    
    rect = Rectf( 15, 15, 15 + ( getWindowWidth() - 35 ) / 2, 150 );
    rect.offset( Vec2i ( 0, rect.getHeight() + 5 ) );
    drawData( mHarmonicSpectrum.get(),  BLOCKSIZE >> 2, rect, Color( 0.7f, 0.2f, 1.0f )  );     rect.offset( Vec2i ( rect.getWidth() + 5, 0 ) );
    drawData( mMfccs.get(),             MFCC_FREQ_BANDS, rect );
    
    rect = Rectf( 15, 15, 15 + ( getWindowWidth() - 35 ) / 2, 150 );
    rect.offset( 2 * Vec2i ( 0, rect.getHeight() + 5 ) );
    drawData( mBarks.get(),             XTRACT_BARK_BANDS, rect );                              
//    drawData( mXtract.getAutocorrelationFft().get(),  XTRACT_BARK_BANDS,  rect );
    
    float w = 300;
    float h = 30;
    
    gl::pushMatrices();
    
    // Mean
    gl::color( Color::white() );
    glBegin( GL_QUADS );
    glVertex2f( 0,          0 );
    glVertex2f( mMean * w,  0 );
    glVertex2f( mMean * w,  h );
    glVertex2f( 0,          h );
    glEnd();
    
    gl::translate( 0, h + 5 );
    
    // F0
    glBegin( GL_QUADS );
    glVertex2f( 0,          0 );
    glVertex2f( mF0 * w,    0 );
    glVertex2f( mF0 * w,    h );
    glVertex2f( 0,          h );
    glEnd();
    
    gl::translate( 0, h + 5 );
    
    // Spectral Centroid
    float sc = mXtract.getSpectralCentroid();
    glBegin( GL_QUADS );
    glVertex2f( 0,      0 );
    glVertex2f( sc * w, 0 );
    glVertex2f( sc * w, h );
    glVertex2f( 0,      h );
    glEnd();
    
    
    gl::popMatrices();
    
    
//    mParams.draw();
}


void BasicSampleApp::drawWaveForm()
{
	if( ! mPcmBuffer )
		return;
	
	int bufferLength = mPcmBuffer->getSampleCount();
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
    
	gl::color( Color( 0.0f, 0.69f, 0.78f ) );
	gl::draw( leftBufferLine );
	gl::draw( rightBufferLine );
}


void BasicSampleApp::drawData( double *data, int N, Rectf rect, Color col, bool clamp )
{
    int padding = 5;
    
    glPushMatrix();
    
    gl::color( ColorA( 0.3f, 0.3f, 0.3f, 0.15f ) );
    gl::drawSolidRect( rect );
    
    rect.inflate( - Vec2i::one() * padding );
    gl::translate( rect.getUpperLeft() );
    
    glBegin( GL_QUADS );
    
    float step = rect.getWidth() / N;
    float h = rect.getHeight();
    
    for( int i = 0; i < N; i++ )
    {
		float barY = clamp ? math<float>::min( data[i], h ) : data[i];
//        barY *= h;
        
        gl::color( col );
        glVertex2f( i * step,           h );
        glVertex2f( ( i + 1 ) * step,   h );
        
        gl::color( col + Color( (float)i / (float)N, 0.0f, 1.0f - (float)i / (float)N ) );
        glVertex2f( ( i + 1 ) * step,   h-barY );
        glVertex2f( i * step,           h-barY );
		
	}
    
    glEnd();
    
    gl::popMatrices();
}


CINDER_APP_NATIVE( BasicSampleApp, RendererGl )


