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
: WidgetBase::WidgetBase( parent, label, feature, xtract, Vec2i( VECTOR_WIDGET_WIDTH, VECTOR_WIDGET_HEIGHT ) )
{
    mWidgetRect = Rectf( 0, 0, GetSize().x, GetSize().y );
    mValRect    = Rectf( mWidgetRect.x1 + 18,   mWidgetRect.y1 + 25,    mWidgetRect.x1 + 18 + 5,    mWidgetRect.y2 );
    mBuffRect   = Rectf( mValRect.x2 + 3,       mWidgetRect.y1 + 25,    mWidgetRect.x2,             mWidgetRect.y2 );
    
    mEnableCheckBox = new Gwen::Controls::CheckBox( this );
    mEnableCheckBox->SetPos( 0, 0 );
    mEnableCheckBox->onCheckChanged.Add( this, &VectorWidget::toggleFeature  );
    
    mGainSlider = new Gwen::Controls::VerticalSlider( this );
    mGainSlider->SetPos( 0, mValRect.y1 );
    mGainSlider->SetSize( 15, mValRect.getHeight() );
    mGainSlider->SetRange( 0.0f, 2.0f );
    mGainSlider->SetFloatValue( 1.0f );

    if( !mFeature->isEnable() )
        mGainSlider->Hide();
    
    mPrevData = std::shared_ptr<double>( new double[ mFeature->getResultN() ] );
    for( auto k=0; k < mFeature->getResultN(); k++ )
        mPrevData.get()[k] = 0.0f;
}


void VectorWidget::toggleFeature( Gwen::Controls::Base* pControl )
{
    Gwen::Controls::CheckBox* checkbox = ( Gwen::Controls::CheckBox* )pControl;
    
    if ( checkbox->IsChecked() )
    {
        mXtract->enableFeature( mFeature->getEnum() );
        mGainSlider->Show();
    }
    else
    {
        mXtract->disableFeature( mFeature->getEnum() );
        mGainSlider->Hide();
        gl::color( mBuffBgCol );
    }
}


void VectorWidget::Render( Skin::Base* skin )
{
    if ( mEnableCheckBox->IsChecked() ^ mFeature->isEnable() )
        mEnableCheckBox->SetChecked( mFeature->isEnable() );
    
    Vec2f widgetPos( cigwen::fromGwen( LocalPosToCanvas() ) );
    
    glPushMatrix();
    
    gl::translate( widgetPos );
    
    gl::color( mLabelCol );
    mFontSmall->drawString( mLabel, Vec2f( 20, 11 ) );
    
    if ( WidgetBase::update() )
    {
        float step  = mBuffRect.getWidth() / mFeature->getResultN();
        float h     = mBuffRect.getHeight();
        float val;
        
        std::shared_ptr<double> data = mFeature->getResult();
        
        gl::translate( mBuffRect.getUpperLeft() );
      
        gl::color( mBuffCol );
        
        glBegin( GL_QUADS );
        
        for( int i = 0; i < mFeature->getResultN(); i++ )
        {
            val = (float)mGainSlider->GetFloatValue() * ( data.get()[i] - mMin ) / ( mMax - mMin );

            if ( mClamp )
                val = math<float>::clamp( val, 0.0f, 1.0f ) * h;
            
            // Damping
            if ( mDamping > 0.0f && val < mPrevData.get()[i] )
                val = mPrevData.get()[i] * mDamping;
            
            mPrevData.get()[i] = val;
            
            glVertex2f( i * step,           h );
            glVertex2f( ( i + 1 ) * step,   h );
            glVertex2f( ( i + 1 ) * step,   h - val );
            glVertex2f( i * step,           h - val );
        }
        
        glEnd();
    }
    else
    {
        gl::color( mBuffBgCol );
        gl::drawSolidRect( Rectf( 0, mBuffRect.y1, GetSize().x, mBuffRect.y2  ) );
    }
    
    gl::popMatrices();
    
}
