#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class GwenSampleApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void GwenSampleApp::setup()
{
}

void GwenSampleApp::mouseDown( MouseEvent event )
{
}

void GwenSampleApp::update()
{
}

void GwenSampleApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( GwenSampleApp, RendererGl )
