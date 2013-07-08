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
    //    pSlider->onValueChanged.Add( this, &Slider::SliderMoved );

    if( !mFeature->isEnable() )
    {
        mGainSlider->Hide();
    }
    
//    mSurfOffset     = Vec2i( mBuffRect.x2 + 3, 0 );
//    mSurf           = Surface32f( VECTOR_WIDGET_WIDTH - mSurfOffset.x, mBuffRect.getHeight(), true, SurfaceChannelOrder::RGBA );
//    mSurfPosX       = 0.0f;
//    
//    for( auto x=0; x < mSurf.getWidth(); x++ )
//        for( auto y=0; y < mSurf.getHeight(); y++ )
//            mSurf.setPixel( Vec2i( x, y ), ci::ColorA::white() );
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
        
//        ci::Vec2f tl;
        
        for( int i = 0; i < mFeature->getResultN(); i++ )
        {
            val = (float)mGainSlider->GetFloatValue() * ( data.get()[i] - mMin ) / ( mMax - mMin );

            if ( mClamp )
                val = math<float>::clamp( val, 0.0f, 1.0f ) * h;
            
            glVertex2f( i * step,           h );
            glVertex2f( ( i + 1 ) * step,   h );
            glVertex2f( ( i + 1 ) * step,   h - val );
            glVertex2f( i * step,           h - val );
            
//            tl.x = w - val;
//            tl.y = i * step;
//            glVertex2f( tl.x,   tl.y );
//            glVertex2f( w,      tl.y );
//            glVertex2f( w,      tl.y + 1 );
//            glVertex2f( tl.x,   tl.y + 1 );
            
//            val /= w;
//            mSurf.setPixel( Vec2i( mSurfPosX, i * step ), ci::ColorA( 1.0f, 1.0f - val, 1.0f - val, 1.0f ) );
        }
        
        glEnd();
    }
        
//    gl::translate( mSurfOffset );
//    gl::color( ci::Color::white() );
//    gl::Texture tex = gl::Texture( mSurf );
//    tex.enableAndBind();
//    
//    glBegin( GL_QUADS );
//    
//    float texOffset = 0;//mSurfPosX / mSurf.getWidth();
//    
//    glTexCoord2f( texOffset + 0.0f, 0.0f );     glVertex2f( 0,                  0 );
//    glTexCoord2f( texOffset + 1.0f, 0.0f );     glVertex2f( mSurf.getWidth(),   0 );
//    glTexCoord2f( texOffset + 1.0f, 1.0f );     glVertex2f( mSurf.getWidth(),   mSurf.getHeight() );
//    glTexCoord2f( texOffset + 0.0f, 1.0f );     glVertex2f( 0,                  mSurf.getHeight() );
//    
//    glEnd();
//
//    tex.disable();
//    
//    mSurfPosX = fmod( mSurfPosX + 1, mSurf.getWidth() );
    
    gl::popMatrices();
    
}
