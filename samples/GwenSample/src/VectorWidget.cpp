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


VectorWidget::VectorWidget( Gwen::Controls::Base *parent, std::string label, ciXtractFeatureRef feature, ciXtractRef xtract )
: WidgetBase::WidgetBase( parent, label, feature, xtract )
{
    SetBounds( 0, 0, VECTOR_CONTROL_WIDTH, VECTOR_CONTROL_HEIGHT );
    
    mWidgetRect = Rectf( 0, 0, VECTOR_CONTROL_WIDTH, VECTOR_CONTROL_HEIGHT );
    mValRect    = Rectf( mWidgetRect.x1 + 18,   mWidgetRect.y1 + 25,    mWidgetRect.x1 + 18 + 5,    mWidgetRect.y2 );
    mBuffRect   = Rectf( mValRect.x2 + 3,       mWidgetRect.y1 + 25,    mWidgetRect.x2,             mWidgetRect.y2 );
    
    mCheckBox = new Gwen::Controls::CheckBox( this );
    mCheckBox->SetPos( 0, 0 );
    mCheckBox->onCheckChanged.Add( this, &VectorWidget::toggleFeature  );
    
    mGainSlider = new Gwen::Controls::VerticalSlider( this );
    mGainSlider->SetPos( 0, mValRect.y1 );
    mGainSlider->SetSize( 15, mValRect.getHeight() );
    mGainSlider->SetRange( 0.0f, 2.0f );
    mGainSlider->SetFloatValue( 1.0f );
    //    pSlider->onValueChanged.Add( this, &Slider::SliderMoved );
    
    mNumericMin = new Gwen::Controls::TextBoxNumeric( this );
    mNumericMin->SetBounds( mBuffRect.x2 - 76, mBuffRect.y1 + 3, 35, 20 );
    mNumericMin->SetText( to_string( mFeature->getResultMin() ) );
    
    mNumericMax = new Gwen::Controls::TextBoxNumeric( this );
    mNumericMax->SetBounds( mBuffRect.x2 - 38, mBuffRect.y1 + 3, 35, 20 );
    mNumericMax->SetText( to_string( mFeature->getResultMax() ) );

    if( !mFeature->isEnable() )
    {
        mGainSlider->Hide();
        mNumericMin->Hide();
        mNumericMax->Hide();
    }
}


void VectorWidget::toggleFeature( Gwen::Controls::Base* pControl )
{
    Gwen::Controls::CheckBox* checkbox = ( Gwen::Controls::CheckBox* )pControl;
    
    if ( checkbox->IsChecked() )
    {
        mXtract->enableFeature( mFeature->getEnum() );
        
        mGainSlider->Show();
        mNumericMin->Show();
        mNumericMax->Show();
    }
    else
    {
        mXtract->disableFeature( mFeature->getEnum() );
//        *mVal = 0.0f; reset buffer instead?
        
        mGainSlider->Hide();
        mNumericMin->Hide();
        mNumericMax->Hide();
    }
}


void VectorWidget::Render( Skin::Base* skin )
{
    if ( mCheckBox->IsChecked() ^ mFeature->isEnable() )
        mCheckBox->SetChecked( mFeature->isEnable() );
    
    Vec2f widgetPos( cigwen::fromGwen( LocalPosToCanvas() ) );
    
    std::shared_ptr<double> data = mXtract->getVectorFeature( mFeature->getEnum() );
    
    glPushMatrix();
    
    gl::translate( widgetPos );
    
    gl::color( mLabelCol );
    mFontSmall->drawString( mLabel, Vec2f( 20, 11 ) );
    
    if( mFeature->isEnable() )
    {
        gl::translate( mBuffRect.getUpperLeft() );
      
        gl::color( mBuffCol );
        
        glBegin( GL_QUADS );
        
        float step  = mBuffRect.getWidth() / mFeature->getResultN();
        float h     = mBuffRect.getHeight();
//        float min   = mXtract->getFeatureMin( mCb->feature );
//        float max   = mXtract->getFeatureMax( mCb->feature );
        float min   = mNumericMin->GetFloatFromText();
        float max   = mNumericMax->GetFloatFromText();
        float val;

        for( int i = 0; i < mFeature->getResultN(); i++ )
        {
            val = (float)mGainSlider->GetFloatValue() * ( data.get()[i] - min ) / ( max - min );
            val = math<float>::clamp( val, 0.0f, 1.0f ) * h;
            
            glVertex2f( i * step,           h );
            glVertex2f( ( i + 1 ) * step,   h );
            glVertex2f( ( i + 1 ) * step,   h - val );
            glVertex2f( i * step,           h - val );
        }
        
        glEnd();
    }
    
    gl::popMatrices();

}
