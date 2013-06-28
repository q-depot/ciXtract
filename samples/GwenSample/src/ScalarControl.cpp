
#include "ScalarControl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"

#include "cigwen/CinderGwen.h"

using namespace Gwen;
using namespace ci;

ScalarControl::ScalarControl( Gwen::Controls::Base *parent, std::string label, double *val)
: ScalarControl::ScalarControl( parent )
{
    mLabel  = label;
    mVal    = val;
    
    mValBar = new Gwen::Controls::ProgressBar( parent );
    mValBar->SetBounds( Gwen::Rect( 12, 40, 200, 20 ) );
    mValBar->SetValue( *mVal );
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

	gl::pushMatrices();
    ci::app::console() << pos << std::endl;
	gl::translate( pos );
    Vec2i offset( 12, 22 );
    gl::drawString( mLabel, offset, ci::Color::black() );                                               offset += Vec2i( 0, 15 );
	gl::drawString( ci::toString( *mVal ), offset, ci::Color::black() );                                offset += Vec2i( 0, 15 );

    //	ci::Rectf bounds( cigwen::fromGwen( GetBounds() ) );
    //	gl::drawString( std::string( "bounds: " ) + ci::toString( bounds ), offset, ci::Color::black() );	offset += Vec2i( 0, 15 );

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


