#include "cinder/app/AppNative.h"
#include "cinder/Xml.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "OscSender.h"
#include "cinder/audio2/Voice.h"
#include "cinder/audio2/Context.h"
#include "cinder/audio2/Scope.h"
#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class XtractSenderOSCAudio2App : public AppBasic {

public:
	
	void prepareSettings( Settings *settings );
	void setup();
	void shutdown();
	void keyDown( KeyEvent event );
	void update();
	void draw();
	void drawPcmData();
    void initFromSettings( fs::path path );
    void loadAudio( fs::path filePath );
    void fileDrop (FileDropEvent event);

    string                          mInputName;
    
	audio2::VoiceRef				mVoice;
	audio2::ScopeSpectralRef		mScopeSpectral;
	bool							mIsPlaying;

    ciXtractRef                     mXtract;
    vector<ciXtractFeatureRef>      mFeatures;
	
    gl::TextureFontRef              mFontMedium;
    
    gl::TextureRef                  mActiveIcon;
    
	osc::Sender                     mOscSender;
    std::string                     mOscHost;
    int                             mOscPort;
};


void XtractSenderOSCAudio2App::prepareSettings(Settings *settings)
{
	settings->setTitle("ciXtract Sample");
	settings->setWindowSize( 300, 600 );
}


void XtractSenderOSCAudio2App::setup()
{    
    mFontMedium = gl::TextureFont::create( Font( "Arial", 18 ) );
    
    mActiveIcon = gl::Texture::create( loadImage( getAssetPath( "active_icon.png" ) ) );
	
	initFromSettings( getAssetPath( "default.xml" ) );
}


void XtractSenderOSCAudio2App::shutdown()
{
}


void XtractSenderOSCAudio2App::update()
{
	if ( !mScopeSpectral )
		return;

	if ( mVoice && !mVoice->isPlaying() && mIsPlaying )			// loop the sound
		mVoice->play();

	mXtract->update( mScopeSpectral->getBuffer().getData() );
    
    // Send OSC data
    ciXtractFeatureRef  feature;
    
    for( size_t k=0; k < mFeatures.size(); k++ )
    {
        osc::Message message;
        
        feature = mFeatures[k];
        
        message.setAddress( "/" + feature->getEnumStr() );
        
        for( size_t i=0; i < feature->getResultN(); i++ )
            message.addFloatArg( feature->getResult().get()[i] );
        
        mOscSender.sendMessage( message );
    }
}


void XtractSenderOSCAudio2App::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();
	gl::clear( Color::gray( 0.85f ) );

	gl::color( Color::white() );

    Color col;
    Vec2i pos( 15, 25 );
        
    gl::color( Color::black() );
    
	mFontMedium->drawString( to_string( (int)getAverageFps() ) + " FPS", Vec2i( getWindowWidth() - 60, pos.y ) );

    // Settings
    mFontMedium->drawString( mInputName, pos );														pos.y += 25;
    mFontMedium->drawString( mOscHost + "@" + to_string( mOscPort ), Vec2f( pos.x - 2, pos.y ) );   pos.y += 25;
    
    // Features
    for( size_t k=0; k < mFeatures.size(); k++ )
    {
        if ( mFeatures[k]->getResult().get()[0] == 0 || isnan( mFeatures[k]->getResult().get()[0] ) )
            col = Color::black();
        else
            col = Color( 0.96f, 0.0f, 0.47f );

        gl::color( col );
        gl::draw( mActiveIcon, pos );
        
        gl::color( Color::black() );
        mFontMedium->drawString( mFeatures[k]->getName(), pos + Vec2f( 20, 11 ) );
        pos.y += 20;
    }

	drawPcmData();
}


void XtractSenderOSCAudio2App::drawPcmData()
{
	audio2::Buffer buff = mScopeSpectral->getBuffer();
	uint32_t bufferLength           = buff.getSize() / 2;	// take the left channel
 
	int     displaySize = getWindowWidth();
	float   scale       = displaySize / (float)bufferLength;
	
	PolyLine<Vec2f>	leftBufferLine;
	PolyLine<Vec2f>	rightBufferLine;

	int height = 25;
	int offset = getWindowHeight() - 160;


	// left channel
	mFontMedium->drawString( "Left channel", Vec2i( 5, offset - 25 ) );
	for( int i = 0; i < bufferLength; i++ )
    {
		float x = i * scale;
		float y = offset + buff.getData()[i] * height;
		leftBufferLine.push_back( Vec2f( x , y) );
	}
	gl::draw( leftBufferLine );

	offset += 80;

	// right channel
	mFontMedium->drawString( "Right channel", Vec2i( 5, offset - 25 ) );
	for( int i = 0; i < bufferLength; i++ )
    {
		float x = i * scale;
		float y = offset + buff.getData()[bufferLength+i] * height;
		rightBufferLine.push_back( Vec2f( x , y) );
	}
	gl::draw( rightBufferLine );
}


void XtractSenderOSCAudio2App::initFromSettings(  fs::path path )
{
    try
    {
        XmlTree doc;
        doc = XmlTree( loadFile( path ) );
        
        // Settings
        mOscHost    = doc.getChild("settings/oscHost").getAttributeValue<string>("value");
        mOscPort    = atof( doc.getChild("settings/oscPort").getAttributeValue<string>("value").c_str() );
        mInputName  = doc.getChild("settings/inputSource").getAttributeValue<string>("value");
		
		// load track
		loadAudio( getAssetPath( mInputName ) );

        // OSC
        mOscSender.setup( mOscHost, mOscPort );
        
		mXtract = ciXtract::create();
        
        vector<ciXtractFeatureRef>  features = mXtract->getFeatures();
        string                      enumStr;
        
        // Features
        for(XmlTree::Iter node = doc.begin("features/feature"); node != doc.end(); ++node)
        {
            enumStr = node->getAttributeValue<string>("name");
            
            for( size_t k=0; k < features.size(); k++ )
                if ( features[k]->getEnumStr() == enumStr )
                {
                    mXtract->enableFeature( features[k]->getEnum() );
                    mFeatures.push_back( mXtract->getFeature( features[k]->getEnum() ) );
                }
        }
    }
    
    catch ( ... )
    {
        console() << "ERROR loading xml settings" << endl;
        exit(-1);
    }
}


void XtractSenderOSCAudio2App::loadAudio( fs::path filePath )
{
	if ( mVoice && mVoice->isPlaying() )
	{		
		mVoice->stop();
		mVoice->getNode()->disconnectAll();
		mVoice->getNode()->disconnect();
	}

	console() << "Load track: " << filePath << endl;

	auto ctx = audio2::Context::master();

	auto scopeFmt = audio2::ScopeSpectral::Format().fftSize( FFT_SIZE ).windowSize( PCM_SIZE );
	mScopeSpectral = ctx->makeNode( new audio2::ScopeSpectral( scopeFmt ) );
	
	mVoice = audio2::Voice::create( audio2::load( DataSourcePath::create( filePath.string() ) ) );

	mVoice->setVolume( 1.0f );
	mVoice->setPan( 0.5f );

	mVoice->play();
	
	mIsPlaying = true;

	mVoice->getNode() >> mScopeSpectral >> ctx->getOutput();

	ctx->start();
}


void XtractSenderOSCAudio2App::keyDown( KeyEvent event )
{
    char c = event.getChar();
    
    if ( c == 'c' && mXtract )
		mXtract->calibrateFeatures();

	else if ( c == ' ' && mVoice )
	{
		// By stopping the Voice first if it is already playing, we always begin playing from the beginning
		if( mVoice->isPlaying() )
		{
			mVoice->pause();
			mIsPlaying = false;
			console() << mVoice->isPlaying() << endl;
		}
		else
		{
			mVoice->play();
			mIsPlaying = true;
		}
	}

//	else if ( c = 'l' )
//		loadAudio( getAssetPath("Blank__Kytt_-_08_-_RSPN.mp3") );

}


void XtractSenderOSCAudio2App::fileDrop( FileDropEvent event )
{
	if ( event.getNumFiles() < 1 )
		return;

	loadAudio( event.getFile(0) );
}


CINDER_APP_BASIC( XtractSenderOSCAudio2App, RendererGl( RendererGl::AA_MSAA_8 ) )
