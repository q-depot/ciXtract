
#include "ScalarControl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"

#include "cigwen/CinderGwen.h"

using namespace Gwen;
using namespace ci;


ScalarControl::ScalarControl( Gwen::Controls::Base *parent, std::string label, ciLibXtract::FeatureCallback *cb, ciLibXtractRef xtract )
: ScalarControl::ScalarControl( parent )
{
    mLabel      = label;
    mXtract     = xtract;
    mCb         = cb;
    mVal        = mXtract->getScalarFeaturePtr( cb->feature );
    
    Gwen::Controls::CheckBoxWithLabel* labeled = new Gwen::Controls::CheckBoxWithLabel( this );
    labeled->SetPos( 12, 0 );
    labeled->Label()->SetText( label );
    
    labeled->Checkbox()->onCheckChanged.Add( this, &ScalarControl::toggleFeature  );
    
    mValBar = new Gwen::Controls::ProgressBar( this );
    mValBar->SetBounds( Gwen::Rect( 12, 19, SCALAR_CONTROL_WIDTH - 70, 20 ) );
    mValBar->SetAutoLabel( false );
    mValBar->SetValue( *mVal );

    mGainNumeric = new Controls::NumericUpDown( this );
    mGainNumeric->SetBounds( SCALAR_CONTROL_WIDTH - 50, 19, 50, 20 );
    mGainNumeric->SetValue( 50 );
    mGainNumeric->SetMax( 1000 );
    mGainNumeric->SetMin( -1000 );

    SetBounds( 0, 0, SCALAR_CONTROL_WIDTH, SCALAR_CONTROL_HEIGHT );
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
    float val = (*mVal) * std::atof( mGainNumeric->GetValue().c_str() );

    mValBar->SetValue( val );
    
	Vec2f pos( cigwen::fromGwen( LocalPosToCanvas() ) );

    char buff[50];
    sprintf( buff, "%.2f", val );
    std::string valStr = buff;
	gl::drawString( valStr, pos + Vec2f( SCALAR_CONTROL_WIDTH - 22, 6 ), ci::Color::gray( 0.7f ) );
}


void ScalarControl::RenderUnder( Skin::Base* skin )
{
}

