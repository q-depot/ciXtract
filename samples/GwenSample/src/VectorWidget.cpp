/*
 *  ScalarWidget.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */

#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/TextureFont.h"
#include "cigwen/CinderGwen.h"

#include "VectorWidget.h"

using namespace Gwen;
using namespace ci;
using namespace ci::app;
using namespace std;

extern gl::TextureFontRef      mFontSmall;
extern gl::TextureFontRef      mFontMedium;
extern gl::TextureFontRef      mFontBig;


VectorWidget::VectorWidget( Gwen::Controls::Base *parent, std::string label, ciLibXtract::FeatureCallback *cb, ciLibXtractRef xtract )
: WidgetBase::WidgetBase( parent, label, cb, xtract )
{
    SetBounds( 0, 0, VECTOR_CONTROL_WIDTH, VECTOR_CONTROL_HEIGHT );
    
    mWidgetRect = Rectf( 0, 0, VECTOR_CONTROL_WIDTH, VECTOR_CONTROL_HEIGHT );
    mValRect    = Rectf( mWidgetRect.x1 + 18,   mWidgetRect.y1 + 35,    mWidgetRect.x1 + 18 + 5,    mWidgetRect.y2 );
    mBuffRect   = Rectf( mValRect.x2 + 3,       mValRect.y1,            mWidgetRect.x2,             mValRect.y2 );
    
    mCheckBox = new Gwen::Controls::CheckBox( this );
    mCheckBox->SetPos( VECTOR_CONTROL_WIDTH - 15, 0 );
    mCheckBox->onCheckChanged.Add( this, &VectorWidget::toggleFeature  );
    
    mGainSlider = new Gwen::Controls::VerticalSlider( this );
    mGainSlider->SetPos( 0, mValRect.y1 );
    mGainSlider->SetSize( 15, mValRect.getHeight() );
    mGainSlider->SetRange( 0.0f, 2.0f );
    mGainSlider->SetFloatValue( 1.0f );
    //    pSlider->onValueChanged.Add( this, &Slider::SliderMoved );
    
    mNumericMin = new Gwen::Controls::TextBoxNumeric( this );
    mNumericMin->SetBounds( mBuffRect.x2 - 76, mBuffRect.y1 + 3, 35, 20 );
    mNumericMin->SetText( to_string( mCb->min ) );
    
    mNumericMax = new Gwen::Controls::TextBoxNumeric( this );
    mNumericMax->SetBounds( mBuffRect.x2 - 38, mBuffRect.y1 + 3, 35, 20 );
    mNumericMax->SetText( to_string( mCb->max ) );
}


void VectorWidget::toggleFeature( Gwen::Controls::Base* pControl )
{
    Gwen::Controls::CheckBox* checkbox = ( Gwen::Controls::CheckBox* )pControl;
    
    if ( checkbox->IsChecked() )
        mXtract->enableFeature( mCb->feature );
    else
    {
        mXtract->disableFeature( mCb->feature );
//        *mVal = 0.0f; reset buffer instead?
    }
}

void VectorWidget::Render( Skin::Base* skin )
{
    if ( mCheckBox->IsChecked() ^ mCb->enable )
        mCheckBox->SetChecked( mCb->enable );
    
    Vec2f       widgetPos( cigwen::fromGwen( LocalPosToCanvas() ) );
    Rectf       rect( widgetPos.x, widgetPos.y, widgetPos.x + VECTOR_CONTROL_WIDTH, widgetPos.y + VECTOR_CONTROL_HEIGHT );
    int         padding = 5;
    ci::Color   col     = ci::Color(1,0,0);
    
    std::shared_ptr<double> data = mXtract->getVectorFeature( mCb->feature );
    
    glPushMatrix();
    
    gl::color( mBuffBgCol );
//    gl::drawSolidRect( rect );

    gl::translate( rect.getUpperLeft() );
    
    gl::color( mBuffCol );
    glBegin( GL_QUADS );
    
    float step  = rect.getWidth() / mCb->buffSize;
    float h     = rect.getHeight();
    
    float min   = mNumericMin->GetFloatFromText();
    float max   = mNumericMax->GetFloatFromText();
    float val;
//    = (float)mGainSlider->GetFloatValue() * ( (*mVal) - min ) / ( max - min );
//    val         = math<float>::clamp( val, 0.0f, 1.0f );
    
    for( int i = 0; i < mCb->buffSize; i++ )
    {
        val = (float)mGainSlider->GetFloatValue() * ( data.get()[i] - min ) / ( max - min );
        val = math<float>::clamp( val, 0.0f, 1.0f ) * h;
        
        glVertex2f( i * step,           h );
        glVertex2f( ( i + 1 ) * step,   h );
        glVertex2f( ( i + 1 ) * step,   h - val );
        glVertex2f( i * step,           h - val );
	}
    
    glEnd();
    
    // label
    gl::color( mLabelCol );
    mFontMedium->drawString( mLabel, Vec2f( 0, 10 ) );
    
    gl::popMatrices();

}
