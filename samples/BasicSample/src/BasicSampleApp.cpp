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
#include "cinder/audio/Output.h"
#include "cinder/audio/Io.h"
#include "cinder/System.h"
#include "OscSender.h"

#include "ciLibXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class BasicSampleApp : public AppNative {
public:
    void shutdown();
    void prepareSettings( Settings *settings );
	void setup();
    void keyDown( KeyEvent event );
	void update();
	void draw();
    
	void drawWaveForm();
    
    void drawData( string label, float *data, int N, float gain, Rectf rect, Color col = Color::white(), bool clamp = true );
    
    void drawData( string label, double *data, int N, float gain, Rectf rect, Color col = Color::white(), bool clamp = true );
    
    void drawData( string label, float data, float gain, Rectf rect, Color col = Color::white(), bool clamp = true );
    
    void initGui();
    
    void processData();
    
    void sendOscData();
    
    void playTrack();
    
    void fileDrop( FileDropEvent event );
    
	audio::PcmBuffer32fRef mPcmBuffer;
    
    audio::Input            mInput;
	std::shared_ptr<float>  mFftDataRef;
    
    params::InterfaceGlRef  mParams;
    
    float               mFftGain;
    float               mPeakThreshold;
    float               mBarkGain;
    bool                mSpectrumNorm;
    float               mSubBandGain;
    
    ciLibXtract         mXtract;
    double              mMean;
    shared_ptr<double>  mSpectrum;
    shared_ptr<double>  mHarmonicSpectrum;
    shared_ptr<double>  mPeakSpectrum;
    shared_ptr<double>  mMfccs;
    shared_ptr<double>  mBark;
    shared_ptr<double>  mAutocorrelationFft;
    shared_ptr<double>  mSubBands;
    
    float               mF0;
    float               mFilesafeF0;
    float               mSpectralCentroid;
    float               mSpread;
    float               mLoudness;
    float               mIrregularityK;
    float               mIrregularityJ;
    float               mStandardDeviation;
    float               mAverageDeviation;
    float               mVariance;
    float               mFlatness;
    float               mFlatnessDb;
    float               mPower;
    float               mTonality;
    
    
    float               mMeanGain;
    float               mCentroidGain;
    float               mDeviationGain;
    float               mSpreadGain;
    float               mLoudnessGain;
    
    
    gl::TextureFontRef  mFontSmall;
    
    
    float               mFps;
    
    float               mDumping;
    
    xtract_spectrum_    mSpectrumMode;
    
    Color               mBgColor;
    
    
    osc::Sender         mOscSender;
	std::string         mOscHost;
	int                 mOscPort;
    bool                mOscEnable;
    
    audio::SourceRef    mAudioRef;
    audio::TrackRef     mTrackRef;
    string              mTrackName;
    bool                mIsTrackInput;
};


void BasicSampleApp::shutdown()
{
    mTrackRef.reset();
    mAudioRef.reset();
}


void BasicSampleApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1200, 800 );
}


void BasicSampleApp::setup()
{
    mXtract.init();
    
    mMean           = 0.0f;
    mFftGain        = 1.0f;
    mPeakThreshold  = 0.5f;
    mBarkGain       = 1.0f;
    mSubBandGain    = 1.0f;
    mSpectrumNorm   = false;
    mSpectrumMode   = XTRACT_MAGNITUDE_SPECTRUM;
    mDumping        = 0.92f;
    
    mBgColor        = Color::white() * 0.15f;
    mOscEnable      = false;
    mIsTrackInput   = false;
    
    mMeanGain       = 200.0f;
    mCentroidGain   = 500.0f;
    mDeviationGain  = 100.0f;
    mSpreadGain     = 100.0f;
    mLoudnessGain   = 100.0f;
    
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
    
    mFontSmall = gl::TextureFont::create( Font( "Helvetica", 12 ) );
    
    initGui();
    
	// assume the broadcast address is this machine's IP address but with 255 as the final value
	// so to multicast from IP 192.168.1.100, the host should be 192.168.1.255
	mOscHost = System::getIpAddress();
    mOscPort = 8000;
    
	if( mOscHost.rfind( '.' ) != string::npos )
		mOscHost.replace( mOscHost.rfind( '.' ) + 1, 3, "255" );
    mOscSender.setup( mOscHost, mOscPort, true );
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


void BasicSampleApp::processData()
{
    //    for( size_t k=0; k < FFT_SIZE; k++ )
    //        mSpectrum[k] = *= mDumping;
}


void BasicSampleApp::update()
{
    if ( mIsTrackInput && mTrackRef )
        mPcmBuffer = mTrackRef->getPcmBuffer();
    else if ( !mIsTrackInput )
        mPcmBuffer = mInput.getPcmBuffer();
    else
        return;
    
	if( ! mPcmBuffer )
		return;
    
    audio::Buffer32fRef leftBuffer = mPcmBuffer->getInterleavedData();
    
    //	audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
    //	audio::Buffer32fRef rightBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_RIGHT );
    
    //    console() << leftBuffer->mSampleCount << endl;
    
	mFftDataRef = audio::calculateFft( mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT ), FFT_SIZE );
	
    if ( !mFftDataRef )
        return;
    
    //    mXtract.setSpectrum( mFftDataRef );
    
	mXtract.setInterleavedData( leftBuffer );
    
    mMean               = mXtract.getMean();
    mSpectrum           = mXtract.getSpectrum( mSpectrumMode, mSpectrumNorm );
    mPeakSpectrum       = mXtract.getPeakSpectrum( mPeakThreshold );
    mMfccs              = mXtract.getMfcc();
    mBark               = mXtract.getBarkCoefficients();
    mF0                 = mXtract.getF0();
    mFilesafeF0         = mXtract.getFailsafeF0();
    mHarmonicSpectrum   = mXtract.getHarmonicSpectrum();
    mSubBands           = mXtract.getSubBands();
    mSpectralCentroid   = mXtract.getSpectralCentroid();
    mSpread             = mXtract.getSpread();
    mLoudness           = mXtract.getLoudness();
    mIrregularityK      = mXtract.getIrregularityK();
    mIrregularityJ      = mXtract.getIrregularityJ();
    mVariance           = mXtract.getVariance();
    mStandardDeviation  = mXtract.getStandardDeviation();
    mAverageDeviation   = mXtract.getAverageDeviation();
    mFlatness           = mXtract.getFlatness();
    mFlatnessDb         = mXtract.getFlatnessDb();
    mPower              = mXtract.getPower();
    mTonality           = mXtract.getTonality();
    
    //    mAutocorrelationFft = mXtract.getAutocorrelationFft();
    
    if ( mOscEnable )
        sendOscData();
    
    mFps = getAverageFps();
}


void BasicSampleApp::draw()
{
    //	gl::clear( Color( 0.85f, 0.85f, 0.85f ) );
    gl::clear( mBgColor );
    gl::color( Color::white() );
    gl::enableAlphaBlending();
    
    drawWaveForm();
    
    int margin = 5;
    Rectf startRect( margin, margin, 200, 40 );
    Rectf rect = startRect;
    Vec2f boxSize = Vec2f( ( getWindowWidth() - margin * 4 - startRect.getWidth() ) / 2.0f, 150 );
    
    drawData( "FPS", mFps, 0.0166f, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Mean", mMean, mMeanGain, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "F0", mF0, 1.0f, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Failsafe F0", mFilesafeF0, 1.0f, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Spectral Centroid", mSpectralCentroid, mCentroidGain, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Spread", mSpread, mSpreadGain, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Loudness", mF0, 1.0f, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Irregularity K", mIrregularityK, 1.0f, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Irregularity J", mIrregularityJ, 1.0f, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Variance", mVariance, 1.0f, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Standard Deviation", mStandardDeviation, mDeviationGain, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Average Deviation", mAverageDeviation, mDeviationGain, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Flatness", mFlatness, 1.0f, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Flatness Db", mFlatnessDb, 1.0f, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Power", mPower, 1.0f, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    drawData( "Tonality", mTonality, 1.0f, rect, Color::white(), true );
    rect.offset( Vec2f( 0, rect.getHeight() + margin ) );
    
    
    // Plot
    
    startRect.set( startRect.x2 + margin, startRect.y1, startRect.x2 + margin + boxSize.x , startRect.y1 + boxSize.y );
    rect = startRect;
    
    // no DC component, so -1?
    drawData( "Spectrum(Xtract)",   mSpectrum.get(), FFT_SIZE - 1, mFftGain, rect, Color( 0.2f, 0.7f, 1.0f ), true );
    
    rect.offset( Vec2i ( rect.getWidth() + 5, 0 ) );
    drawData( "Spectrum peaks",     mPeakSpectrum.get(), FFT_SIZE - 1, mFftGain, rect, Color( 1.0f, 0.2f, 0.15f ), true  );
    
    if ( mFftDataRef )
    {
        rect = startRect;
        rect.offset( Vec2i ( 0, rect.getHeight() + 5 ) );
        drawData( "Spectrum(cinder)", mFftDataRef.get(), FFT_SIZE, 1.0f, rect );
    }
    
    rect.offset( Vec2i ( rect.getWidth() + 5, 0 ) );
    drawData( "MEL", mMfccs.get(), MFCC_FREQ_BANDS, 1.0f, rect, Color::white(), true );    rect.offset( Vec2i ( rect.getWidth() + 5, 0 ) );
    
    rect = startRect;   rect.offset( 2 * Vec2i ( 0, rect.getHeight() + 5 ) );
    drawData( "BARK", mBark.get(), XTRACT_BARK_BANDS, mBarkGain, rect );
    
    rect.offset( Vec2i ( rect.getWidth() + 5, 0 ) );
    drawData( "Harmonic Spectrum", mHarmonicSpectrum.get(),  FFT_SIZE, mFftGain, rect, Color( 0.7f, 0.2f, 1.0f )  );
    
    rect = startRect;   rect.offset( 3 * Vec2i ( 0, rect.getHeight() + 5 ) );
    drawData( "Sub-bands", mSubBands.get(), SUB_BANDS, mSubBandGain, rect );
    
    //    rect.offset( Vec2i ( rect.getWidth() + 5, 0 ) );
    //    drawData( "Autocorrelation Fft", mAutocorrelationFft.get(), FFT_SIZE, mFftGain, rect );
    
    mParams->draw();
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


void BasicSampleApp::drawData( string label, double *data, int N, float gain, Rectf rect, Color col, bool clamp )
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
		float barY = data[i] * h;
        
        barY *= gain;
        
        if ( clamp )
            barY = math<float>::clamp( barY, 0.0f, h );
        
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


void BasicSampleApp::drawData( string label, float *data, int N, float gain, Rectf rect, Color col, bool clamp )
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
		float barY = data[i];
        
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


void BasicSampleApp::drawData( string label, float data, float gain, Rectf rect, Color col, bool clamp )
{
    float padding = 5;
    gl::color( ColorA( 0.3f, 0.3f, 0.3f, 0.15f ) );
    gl::drawSolidRect( rect );
    rect.inflate( - Vec2i::one() * padding );
    
    data *= gain;
    
    if ( clamp )
        data = math<float>::clamp( data, 0.0f, 1.0f );
    
    rect.set( rect.x1, rect.y1 + padding * 3.5,  rect.x1 + rect.getWidth() * data, rect.y2 );
    gl::color( col );
    gl::drawSolidRect( rect );
    gl::color( Color::white() );
    mFontSmall->drawString( label, rect.getUpperLeft() - Vec2f( 0, padding * 1.5 ) );
}


void BasicSampleApp::initGui()
{
    Vec2i guiSize( 250, 300 );
    Vec2i pos = getWindowSize() - guiSize - Vec2i::one() * 5;
    
    mParams = params::InterfaceGl::create( "params", guiSize );
	
    vector<string> strs; strs.push_back("Magnitude"); strs.push_back("Log Magnitude"); strs.push_back("Power"); strs.push_back("Log Power");
	mParams->addParam( "Spectrum", strs, (int*)&mSpectrumMode );
    mParams->addParam( "Fft norm", &mSpectrumNorm );
    mParams->addParam( "Fft gain", &mFftGain,               "min=0.1 max=100.0 step=0.1" );
    mParams->addParam( "Peaks threshold", &mPeakThreshold,  "min=0.0 max=100.0 step=0.1" );
    mParams->addParam( "Bark gain", &mBarkGain,             "min=0.1 max=100.0 step=0.1" );
    mParams->addParam( "Sub-band gain", &mSubBandGain,      "min=0.1 max=100.0 step=0.1" );
    mParams->addParam( "Dumping", &mDumping,                "min=0.1 max=1.0 step=0.01" );
    mParams->addParam( "Mean gain", &mMeanGain,             "min=0.1 max=500.0 step=0.1" );
    mParams->addParam( "Centroid gain", &mCentroidGain,             "min=0.1 max=500.0 step=0.1" );
    mParams->addParam( "Deviation gain", &mDeviationGain,             "min=0.1 max=500.0 step=0.1" );
    mParams->addParam( "Spread gain", &mSpreadGain,             "min=0.1 max=500.0 step=0.1" );
    mParams->addParam( "Loudness gain", &mLoudnessGain,             "min=0.1 max=500.0 step=0.1" );
    
    mParams->addSeparator();
	mParams->addParam( "Bg color", &mBgColor );
    
    mParams->addSeparator();
	mParams->addParam( "OSC", &mOscEnable );
    
    mParams->addSeparator();
    mParams->addParam( "Track", &mTrackName, "", true );
    mParams->addButton( "Play", std::bind( &BasicSampleApp::playTrack, this ) );
    
    mParams->setOptions( "", "position='" + toString( pos.x ) + " " + toString( pos.y ) + "'");
}


void BasicSampleApp::sendOscData()
{
	osc::Message message;
    for( int k=0; k < XTRACT_BARK_BANDS-1; k++ )
    {
        message.addFloatArg( (float)( mBark.get()[k] * mBarkGain ) );
    }
    message.setAddress("/bark");
    mOscSender.sendMessage( message );
}


void BasicSampleApp::playTrack()
{
    if ( !mAudioRef || !mTrackRef )
        return;
    
    if ( mTrackRef->isPlaying() )
    {
        mTrackRef->stop();
        mIsTrackInput = false;
    }
    else
    {
        mTrackRef->setTime( 0.0f );
        mTrackRef->play();
        mIsTrackInput = true;
    }
}


void BasicSampleApp::fileDrop( FileDropEvent event )
{
    fs::path filePath = event.getFile(0);
    
    if( filePath.extension() != ".mp3" )
        return;
    
    mTrackName  = filePath.filename().generic_string();
    mAudioRef   = ci::audio::load( filePath.generic_string() );
    mTrackRef   = ci::audio::Output::addTrack( mAudioRef, false );
    
    mTrackRef->enablePcmBuffering(true);
}


CINDER_APP_NATIVE( BasicSampleApp, RendererGl )

