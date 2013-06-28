
#include "ScalarControl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"

#include "cigwen/CinderGwen.h"

using namespace Gwen;
using namespace ci;

ScalarControl::ScalarControl( Gwen::Controls::Base *parent )
: Controls::Base( parent, "cigwen sample ScalarControl" )
{
	mImageTex = loadImage( app::loadAsset( "gwen64.png" ) ); // gwen/assets should already be added to asset path during setup..
}

ScalarControl::~ScalarControl()
{
	app::console() << "ScalarControl destroyed." << std::endl;
}

void ScalarControl::Render( Skin::Base* skin )
{
	Vec2f pos( cigwen::fromGwen( LocalPosToCanvas() ) );
	ci::Rectf bounds( cigwen::fromGwen( GetBounds() ) );

//	draw2d();
	draw3d();

	gl::pushMatrices();

	gl::translate( pos );
	float yOffset = 10;
	float yHeight = 20;
	gl::drawString( std::string( "pos: " ) + ci::toString( pos ), Vec2f( 10, yOffset ), ci::Color::black() );		yOffset += yHeight;
	gl::drawString( std::string( "bounds: " ) + ci::toString( bounds ), Vec2f( 10, yOffset ), ci::Color::black() );		yOffset += yHeight;

	gl::popMatrices();
}

void ScalarControl::RenderUnder( Skin::Base* skin )
{
}

void ScalarControl::draw2d()
{
	static float rot = 0;
	rot += 0.5f;

	float w = 40;
	Rectf r( -w, -w, w, w );
	ci::Rectf bounds( cigwen::fromGwen( GetBounds() ) );

	gl::pushMatrices();

	gl::translate( cigwen::fromGwen( LocalPosToCanvas() ) );
	gl::translate( bounds.getCenter() );
	gl::rotate( rot );
	gl::color( ci::Color( 0, 0.8, 0 ) );
	gl::drawSolidRect( r );
	gl::color( ci::Color( 0, 0, 0.8 ) );
	gl::drawStrokedRect( r );

	gl::popMatrices();
}

void ScalarControl::draw3d()
{
	mCubeRotation.rotate( Vec3f( 1, 1, 1 ), 0.03f );
	float aspect = (float)m_InnerBounds.w / (float)m_InnerBounds.h;
	Vec2f origin( cigwen::fromGwen( LocalPosToCanvas() ) );
	Area viewport = gl::getViewport();
	glViewport( origin.x, m_InnerBounds.h - origin.y, m_InnerBounds.w, m_InnerBounds.h );
	gl::pushMatrices();
	gl::enableDepthRead();

	mCamera.lookAt( Vec3f( 1, 1, -3 ), Vec3f( 0, 0.8, 0 ) );
	mCamera.setPerspective( 60, aspect, 1, 1000 );

	gl::setMatrices( mCamera );
	gl::multModelView( mCubeRotation );

	mImageTex.enableAndBind();

	gl::color( ci::Color::white() );
	gl::drawCube( Vec3f::zero(), Vec3f( 1.0f, 1.0f, 1.0f ) );

	mImageTex.unbind();
	
	gl::disableDepthRead();
	gl::popMatrices();
	gl::setViewport( viewport );
}
