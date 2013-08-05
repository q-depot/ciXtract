
#include "cinder/app/AppNative.h"
#include "cinder/Xml.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/gl/Texture.h"
#include "OscSender.h"

#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class XtractSenderOSCApp : public AppBasic {

public:
	
	void prepareSettings( Settings *settings );
	
	void setup();
	
	void update();
	void draw();
	
	void drawPcmData();
	
    void initFromSettings( fs::path path );
    void initAudio( string name );
    
    audio::Input                    mInput;
    string                          mInputName;
    
    ciXtractRef                     mXtract;
    vector<ciXtractFeatureRef>      mFeatures;
	
    gl::TextureFontRef              mFontSmall;
    gl::TextureFontRef              mFontMedium;
    gl::TextureFontRef              mFontBig;
    
    gl::TextureRef                  mLogoTex;
    gl::TextureRef                  mActiveIcon;
    
	osc::Sender                     mOscSender;
    std::string                     mOscHost;
    int                             mOscPort;
};


void XtractSenderOSCApp::prepareSettings(Settings *settings)
{
	settings->setTitle("ciXtract Sample");
	settings->setWindowSize( 200, 600 );
}


void XtractSenderOSCApp::setup()
{    
    initFromSettings( getAssetPath( "default.xml" ) );
    
    mFontSmall  = gl::TextureFont::create( Font( "Helvetica", 12 ) );
    mFontMedium = gl::TextureFont::create( Font( "Helvetica", 14 ) );
    mFontBig    = gl::TextureFont::create( Font( "Helvetica", 16 ) );
    
    mLogoTex    = gl::Texture::create( loadImage( getAssetPath( "nocte.png" ) ) );
    mActiveIcon = gl::Texture::create( loadImage( getAssetPath( "active_icon.png" ) ) );
}


void XtractSenderOSCApp::update()
{
	mXtract->update();
    
    // Send OSC data
    ciXtractFeatureRef  feature;
    
    for( auto k=0; k < mFeatures.size(); k++ )
    {
        osc::Message message;
        
        feature = mFeatures[k];
        
        message.setAddress( "/" + feature->getEnumStr() );
        
        for( auto i=0; i < feature->getResultN(); i++ )
            message.addFloatArg( feature->getResult().get()[i] );
        
        mOscSender.sendMessage( message );
    }
}


void XtractSenderOSCApp::draw()
{
	gl::clear( Color::gray( 0.85f ) ); 	
    gl::enableAlphaBlending();
    
	gl::color( Color::white() );

    Color col;
    Vec2i pos( 15, 15 );
    
    gl::draw( mLogoTex, pos );
    pos.y += mLogoTex->getHeight() + 30;
    
    gl::color( Color::black() );
    
    mFontSmall->drawString( to_string( (int)getAverageFps() ) + " FPS", Vec2i( 148, 35 ) );

    // Settings
    mFontBig->drawString( mInputName, pos );                                                        pos.y += 25;
    mFontMedium->drawString( mOscHost + "@" + to_string( mOscPort ), Vec2f( pos.x - 2, pos.y ) );   pos.y += 25;
    
    // Features
    for( auto k=0; k < mFeatures.size(); k++ )
    {
        if ( mFeatures[k]->getResult().get()[0] == 0 || isnan( mFeatures[k]->getResult().get()[0] ) )
            col = Color::black();
        else
            col = Color( 0.96f, 0.0f, 0.47f );

        gl::color( col );
        gl::draw( mActiveIcon, pos );
        
        gl::color( Color::black() );
        mFontSmall->drawString( mFeatures[k]->getName(), pos + Vec2f( 20, 11 ) );
        pos.y += 20;
    }
}


void XtractSenderOSCApp::drawPcmData()
{
    audio::PcmBuffer32fRef pcmBuffer = mInput.getPcmBuffer();
 
	if( !pcmBuffer )
		return;
 
	uint32_t bufferLength           = pcmBuffer->getSampleCount();
	audio::Buffer32fRef leftBuffer  = pcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
 
	int     displaySize = getWindowWidth();
	float   scale       = displaySize / (float)bufferLength;

	PolyLine<Vec2f>	leftBufferLine;

	for( int i = 0; i < bufferLength; i++ )
    {
		float x = i * scale;
        float y = 50 + leftBuffer->mData[i] * 60;
		leftBufferLine.push_back( Vec2f( x , y) );
	}

	gl::draw( leftBufferLine );
}


void XtractSenderOSCApp::initFromSettings(  fs::path path )
{
    try
    {
        XmlTree doc;
        doc = XmlTree( loadFile( path ) );
        
        // Settings
        mOscHost    = doc.getChild("settings/oscHost").getAttributeValue<string>("value");
        mOscPort    = atof( doc.getChild("settings/oscPort").getAttributeValue<string>("value").c_str() );
        mInputName  = doc.getChild("settings/inputSource").getAttributeValue<string>("value");
        
        // OSC
        mOscSender.setup( mOscHost, mOscPort );
        
        // Init audio
        initAudio( mInputName );
        
        // Init Xtract
        if ( !mInput )
        {
            console() << "ERROR initialising the audio device" << endl;
            exit(-1);
        }
        mXtract = ciXtract::create( mInput );
        
        vector<ciXtractFeatureRef>  features = mXtract->getFeatures();
        string                      enumStr;
        
        // Features
        for(XmlTree::Iter node = doc.begin("features/feature"); node != doc.end(); ++node)
        {
            enumStr         = node->getAttributeValue<string>("name");
            
            for( auto k=0; k < features.size(); k++ )
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


void XtractSenderOSCApp::initAudio( string name )
{
    const std::vector<audio::InputDeviceRef>& devices = audio::Input::getDevices();
    for( std::vector<audio::InputDeviceRef>::const_iterator iter = devices.begin(); iter != devices.end(); ++iter )
    {
        if ( (*iter)->getName() == name )
        {
            mInput = audio::Input( *iter );
            mInput.start();
            break;
        }
    }    
}


CINDER_APP_BASIC( XtractSenderOSCApp, RendererGl )

