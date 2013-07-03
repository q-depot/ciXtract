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

#include "ScalarWidget.h"

using namespace Gwen;
using namespace ci;
using namespace ci::app;
using namespace std;

extern gl::TextureFontRef      mFontSmall;
extern gl::TextureFontRef      mFontMedium;
extern gl::TextureFontRef      mFontBig;


ScalarWidget::ScalarWidget( Gwen::Controls::Base *parent, std::string label, ciLibXtract::FeatureCallback *cb, ciLibXtractRef xtract )
: WidgetBase::WidgetBase( parent, label, cb, xtract )
{
    SetBounds( 0, 0, SCALAR_CONTROL_WIDTH, SCALAR_CONTROL_HEIGHT );
    
    mBuff.resize( SCALAR_CONTROL_BUFF_SIZE );
    
    mWidgetRect = Rectf( 0, 0, SCALAR_CONTROL_WIDTH, SCALAR_CONTROL_HEIGHT );
    mValRect    = Rectf( mWidgetRect.x1 + 18,   mWidgetRect.y1 + 35,    mWidgetRect.x1 + 18 + 5,    mWidgetRect.y2 );
    mBuffRect   = Rectf( mValRect.x2 + 3,       mValRect.y1,            mWidgetRect.x2,             mValRect.y2 );
    
    mCheckBox = new Gwen::Controls::CheckBox( this );
    mCheckBox->SetPos( 0, 0 );
    mCheckBox->onCheckChanged.Add( this, &ScalarWidget::toggleFeature  );
    
    mGainSlider = new Gwen::Controls::VerticalSlider( this );
    mGainSlider->SetPos( 0, mValRect.y1 );
    mGainSlider->SetSize( 15, mValRect.getHeight() );
    mGainSlider->SetRange( 0.0f, 2.0f );
    mGainSlider->SetFloatValue( 1.0f );
    //    pSlider->onValueChanged.Add( this, &Slider::SliderMoved );
    
    mNumericMin = new Gwen::Controls::TextBoxNumeric( this );
//    mNumericMin->SetBounds( mBuffRect.x2 - 76, mBuffRect.y1 + 3, 35, 20 );
    mNumericMin->SetBounds( mBuffRect.x2 - 93, mWidgetRect.y1 + 13, 45, 20 );
    mNumericMin->SetText( to_string( mCb->min ) );
    
    mNumericMax = new Gwen::Controls::TextBoxNumeric( this );
    mNumericMax->SetBounds( mBuffRect.x2 - 45, mWidgetRect.y1 + 13, 45, 20 );
    mNumericMax->SetText( to_string( mCb->max ) );
    
    if ( !mCb->enable )
    {
        mGainSlider->Hide();
        mNumericMin->Hide();
        mNumericMax->Hide();
    }
}


void ScalarWidget::toggleFeature( Gwen::Controls::Base* pControl )
{
    Gwen::Controls::CheckBox* checkbox = ( Gwen::Controls::CheckBox* )pControl;
    
    if ( checkbox->IsChecked() )
    {
        mXtract->enableFeature( mCb->feature );
        
        mGainSlider->Show();
        mNumericMin->Show();
        mNumericMax->Show();
    }
    else
    {
        mXtract->disableFeature( mCb->feature );
        *mVal = 0.0f;
        
        mGainSlider->Hide();
        mNumericMin->Hide();
        mNumericMax->Hide();
    }
}


void ScalarWidget::Render( Skin::Base* skin )
{
    if ( mCheckBox->IsChecked() ^ mCb->enable )
        mCheckBox->SetChecked( mCb->enable );
    
    Vec2f widgetPos( cigwen::fromGwen( LocalPosToCanvas() ) );
    
    gl::pushMatrices();
    gl::translate( widgetPos );
    
    gl::color( mLabelCol );
    mFontSmall->drawString( mLabel, Vec2f( 20, 11 ) );
    
    
    if ( mCb->enable )
    {
        float min   = mXtract->getFeatureMin( mCb->feature ); //mNumericMin->GetFloatFromText();
        float max   = mXtract->getFeatureMax( mCb->feature ); //mNumericMax->GetFloatFromText();
        float val   = (float)mGainSlider->GetFloatValue() * ( (*mVal) - min ) / ( max - min );
        val         = math<float>::clamp( val, 0.0f, 1.0f );
        mBuff.push_front( val );
        
        mNumericMin->SetText( to_string(min) );
        mNumericMax->SetText( to_string(max) );
        
        //    if ( isnan(val) )
        //        console() << mCb->name << " " << (*mVal);    
        
        PolyLine<Vec2f>	buffLine;
        float step  = mBuffRect.getWidth() / mBuff.size();
        
        for( int i = 0; i < mBuff.size(); i++ )
            buffLine.push_back( Vec2f( mBuffRect.x1 + i * step , mBuffRect.y1 + ( 1.0f - mBuff[i] ) * mBuffRect.getHeight() ) );
        
        char buff[50];
        sprintf( buff, "%.2f", val );
        std::string valStr = buff;

        // buffer
        gl::color( mBuffBgCol );
        gl::drawSolidRect( mBuffRect );
        ci::gl::color( mBuffCol );
        gl::draw( buffLine );

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
            
        gl::color( mValCol );
        mFontBig->drawString( valStr, Vec2f( 0, 30 ) );
        
    }
    else
    {
//        gl::color( mLabelCol );
//        mFontSmall->drawString( mLabel, widgetPos + Vec2f( 0, 10 ) );
    }
    
    gl::popMatrices();
}
