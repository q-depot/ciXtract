
#include "cinder/app/AppNative.h"

#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class _TBOX_PREFIX_App : public AppBasic {

public:
	
	void prepareSettings( Settings *settings );
	
	void setup();
	
	void update();
	void draw();
	
	void drawPcmData();
	

    audio::Input                    mInput;
    ciXtractRef                     mXtract;
	
};


void _TBOX_PREFIX_App::prepareSettings(Settings *settings)
{
	settings->setTitle("ciXtract Sample");
	settings->setWindowSize( 1280, 720 );
}


void _TBOX_PREFIX_App::setup()
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
 
    mXtract = ciXtract::create( mInput );
}


void _TBOX_PREFIX_App::update()
{
	
}


void _TBOX_PREFIX_App::draw()
{
	gl::clear( Color::gray( 0.85f ) ); 	
	
	gl::color( Color::gray( 0.1f ) );
	drawPcmData();
}


void _TBOX_PREFIX_App::drawPcmData()
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


CINDER_APP_BASIC( _TBOX_PREFIX_App, RendererGl )

