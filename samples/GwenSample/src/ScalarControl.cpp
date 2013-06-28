
#include "ScalarControl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"

#include "cigwen/CinderGwen.h"

using namespace Gwen;
using namespace ci;

#define CONTROL_WIDTH   200
#define CONTROL_HEIGHT  45

ScalarControl::ScalarControl( Gwen::Controls::Base *parent, std::string label, double *val)
: ScalarControl::ScalarControl( parent )
{
    mLabel  = label;
    mVal    = val;
    
    mValBar = new Gwen::Controls::ProgressBar( this );
    mValBar->SetBounds( Gwen::Rect( 0, 22, CONTROL_WIDTH, 20 ) );
    mValBar->SetValue( *mVal );
    SetBounds( 0, 0, CONTROL_WIDTH, CONTROL_HEIGHT );
}


ScalarControl::ScalarControl( Gwen::Controls::Base *parent ) : Controls::Base( parent, "cigwen sample ScalarControl" ) {}


ScalarControl::~ScalarControl()
{
//	app::console() << "ScalarControl destroyed." << std::endl;
}


void ScalarControl::Render( Skin::Base* skin )
{
    mValBar->SetValue( *mVal );
    
	Vec2f pos( cigwen::fromGwen( LocalPosToCanvas() ) );

//	draw2d();
    char buff[50];
    sprintf( buff, "%.2f", *mVal );
    std::string valStr = buff;
    
	gl::pushMatrices();
	gl::translate( pos );
    Vec2i offset( 12, 22 );
    gl::drawString( mLabel, Vec2f(0,11), ci::Color::black() );                                               offset += Vec2i( 0, 15 );
	gl::drawString( valStr, Vec2f( CONTROL_WIDTH - 22, 12 ), ci::Color::black() );                                offset += Vec2i( 0, 15 );

	gl::popMatrices();
}

void ScalarControl::RenderUnder( Skin::Base* skin )
{
}

void ScalarControl::draw2d()
{
//	static float rot = 0;
//	rot += 0.5f;
//
//	float w = 40;
//	Rectf r( -w, -w, w, w );
//	ci::Rectf bounds( cigwen::fromGwen( GetBounds() ) );
//
//	gl::pushMatrices();
//
//	gl::translate( cigwen::fromGwen( LocalPosToCanvas() ) );
//	gl::translate( bounds.getCenter() );
//	gl::rotate( rot );
//	gl::color( ci::Color( 0, 0.8, 0 ) );
//	gl::drawSolidRect( r );
//	gl::color( ci::Color( 0, 0, 0.8 ) );
//	gl::drawStrokedRect( r );
//
//	gl::popMatrices();
}


