
#include "ScalarControl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/TextureFont.h"

#include "cigwen/CinderGwen.h"

using namespace Gwen;
using namespace ci;
using namespace ci::app;
using namespace std;


extern gl::TextureFontRef      mFontSmall;
extern gl::TextureFontRef      mFontMedium;
extern gl::TextureFontRef      mFontBig;


ScalarControl::ScalarControl( Gwen::Controls::Base *parent, std::string label, ciLibXtract::FeatureCallback *cb, ciLibXtractRef xtract )
: ScalarControl::ScalarControl( parent )
{
    mLabel      = label;
    mXtract     = xtract;
    mCb         = cb;
    mVal        = mXtract->getScalarFeaturePtr( cb->feature );
 
    
    mBuff.resize( SCALAR_CONTROL_BUFF_SIZE );
    
    mWidgetRect = Rectf( 0, 0, SCALAR_CONTROL_WIDTH, SCALAR_CONTROL_HEIGHT );
    mValRect    = Rectf( mWidgetRect.x1 + 18,   mWidgetRect.y1 + 35,    mWidgetRect.x1 + 18 + 5,    mWidgetRect.y2 );
    mBuffRect   = Rectf( mValRect.x2 + 3,       mValRect.y1,            mWidgetRect.x2,             mValRect.y2 );
    
    SetBounds( 0, 0, SCALAR_CONTROL_WIDTH, SCALAR_CONTROL_HEIGHT );
    
    
    
    Gwen::Controls::CheckBox* check = new Gwen::Controls::CheckBox( this );
    check->SetPos( SCALAR_CONTROL_WIDTH - 15, 0 );
    check->onCheckChanged.Add( this, &ScalarControl::toggleFeature  );
    
    
    mGainSlider = new Gwen::Controls::VerticalSlider( this );
    mGainSlider->SetPos( 0, mValRect.y1 );
    mGainSlider->SetSize( 15, mValRect.getHeight() );
    mGainSlider->SetRange( 0.0f, 2.0f );
    mGainSlider->SetFloatValue( 0.4f );
//    pSlider->onValueChanged.Add( this, &Slider::SliderMoved );
}

ScalarControl::ScalarControl( Gwen::Controls::Base *parent ) : Controls::Base( parent, "cigwen sample ScalarControl" ) {}


ScalarControl::~ScalarControl() {}


void ScalarControl::toggleFeature( Gwen::Controls::Base* pControl )
{
    Gwen::Controls::CheckBox*           checkbox    = ( Gwen::Controls::CheckBox* )pControl;

    if ( checkbox->IsChecked() )
        mXtract->enableFeature( mCb->feature );
    else
    {
        mXtract->disableFeature( mCb->feature );
        *mVal = 0.0f;
    }
}


void ScalarControl::Render( Skin::Base* skin )
{
    Vec2f widgetPos( cigwen::fromGwen( LocalPosToCanvas() ) );
    
    
//    double val  = ( ( *mVal ) - mCb->min ) / ( mCb->max - mCb->min );//* (float)mGainSlider->GetFloatValue();
    double val  = ( *mVal ) * (float)mGainSlider->GetFloatValue();
    val         = math<double>::clamp( val, 0.0f, 1.0f );
    mBuff.push_front( val );
    
    
    
    if ( mCb->feature == XTRACT_IRREGULARITY_K )
    {
        console() << mCb->min << " " << mCb->max << endl;
        
    }
    
    
    
    
    
    
    
    PolyLine<Vec2f>	buffLine;
	float step  = mBuffRect.getWidth() / mBuff.size();

	for( int i = 0; i < mBuff.size(); i++ )
		buffLine.push_back( Vec2f( mBuffRect.x1 + i * step , mBuffRect.y1 + ( 1.0f - mBuff[i] ) * mBuffRect.getHeight() ) );
    
    char buff[50];
    sprintf( buff, "%.2f", val );
    std::string valStr = buff;
    
    gl::pushMatrices();
    gl::translate( widgetPos );
    
    // buffer
    gl::color( ci::ColorA( 0.0f, 0.0f, 0.0f, 0.1f ) );
    gl::drawSolidRect( mBuffRect );
	ci::gl::color( ci::Color( 1.0f, 0.5f, 0.25f ) );
	gl::draw( buffLine );
    
    // label
    gl::color( ci::Color::gray( 0.5f ) );
    mFontMedium->drawString( mLabel, Vec2f( 0, 10 ) );
    mFontBig->drawString( valStr, Vec2f( 0, 27 ) );
    
    // value bar
    float h = mValRect.y2 - val * mValRect.getHeight();
    glBegin( GL_QUADS );
    gl::color( ColorA::gray( 0.8f ) );
    glVertex2f( mValRect.x1, mValRect.y1 );
    glVertex2f( mValRect.x2, mValRect.y1 );
    glVertex2f( mValRect.x2, mValRect.y2 );
    glVertex2f( mValRect.x1, mValRect.y2 );
    gl::color( ColorA::black() );
    glVertex2f( mValRect.x1, h );
    glVertex2f( mValRect.x2, h );
    glVertex2f( mValRect.x2, mValRect.y2 );
    glVertex2f( mValRect.x1, mValRect.y2 );
    glEnd();
    
    gl::popMatrices();
}


void ScalarControl::RenderUnder( Skin::Base* skin )
{
}

