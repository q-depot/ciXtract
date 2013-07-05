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


ScalarWidget::ScalarWidget( Gwen::Controls::Base *parent, std::string label, ciXtractFeatureRef feature, ciXtractRef xtract )
: WidgetBase::WidgetBase( parent, label, feature, xtract )
{
    SetBounds( 0, 0, SCALAR_CONTROL_WIDTH, SCALAR_CONTROL_HEIGHT * 5);
    
    mBuff.resize( SCALAR_CONTROL_BUFF_SIZE );
    
    mWidgetRect = Rectf( 0, 0, SCALAR_CONTROL_WIDTH, SCALAR_CONTROL_HEIGHT );
    mValRect    = Rectf( mWidgetRect.x1 + 18,   mWidgetRect.y1 + 35,    mWidgetRect.x1 + 18 + 5,    mWidgetRect.y2 );
    mBuffRect   = Rectf( mValRect.x2 + 3,       mValRect.y1,            mWidgetRect.x2,             mValRect.y2 );
    
    mEnableCheckBox = new Gwen::Controls::CheckBox( this );
    mEnableCheckBox->SetPos( 0, 0 );
    mEnableCheckBox->onCheckChanged.Add( this, &ScalarWidget::toggleFeature  );
    
    mGainSlider = new Gwen::Controls::VerticalSlider( this );
    mGainSlider->SetPos( 0, mValRect.y1 );
    mGainSlider->SetSize( 15, mValRect.getHeight() );
    mGainSlider->SetRange( 0.0f, 2.0f );
    mGainSlider->SetFloatValue( 1.0f );
    //    pSlider->onValueChanged.Add( this, &Slider::SliderMoved );
    
    float w = 70;
    mNumericMin = new Gwen::Controls::TextBoxNumeric( this );
    mNumericMin->SetBounds( mBuffRect.x2 - w * 2 - 3, mWidgetRect.y1 + 13, w, 20 );
    mNumericMin->SetText( to_string( feature->getResultMin() ) );
    
    mNumericMax = new Gwen::Controls::TextBoxNumeric( this );
    mNumericMax->SetBounds( mBuffRect.x2 - w, mWidgetRect.y1 + 13, w, 20 );
    mNumericMax->SetText( to_string( feature->getResultMax() ) );
    
    if ( !mFeature->isEnable() )
    {
        mGainSlider->Hide();
        mNumericMin->Hide();
        mNumericMax->Hide();
    }
    
    
    
    mOptionsButton  = new Gwen::Controls::Button( this );
    mOptionsButton->SetText( "options" );
    mOptionsButton->SetBounds( SCALAR_CONTROL_WIDTH - 50, 0, 50, 10 );
    mOptionsButton->onDown.Add( this, &ScalarWidget::toggleProperties );
    
    mPropertiesWindow  = new Gwen::Controls::WindowControl( parent );
    mPropertiesWindow->SetTitle( mFeature->getName() );
    mPropertiesWindow->SetBounds( 0, 0, 250, 200 );
    mPropertiesWindow->Hide();
    
    mProperties = new Gwen::Controls::Properties( mPropertiesWindow );
    mProperties->Dock( Gwen::Pos::Fill );
    mProperties->Add( L"More Items" );
    mProperties->Add( L"Checkbox", new Gwen::Controls::Property::Checkbox( mProperties ), L"1" );
    mProperties->Add( L"To Fill" );
    mProperties->Add( L"Out Here" );
}


void ScalarWidget::toggleFeature( Gwen::Controls::Base* pControl )
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
//        *mVal = 0.0f;
        
        mGainSlider->Hide();
        mNumericMin->Hide();
        mNumericMax->Hide();
    }
}


void ScalarWidget::Render( Skin::Base* skin )
{
    if ( mEnableCheckBox->IsChecked() ^ mFeature->isEnable() )
        mEnableCheckBox->SetChecked( mFeature->isEnable() );
    
    Vec2f widgetPos( cigwen::fromGwen( LocalPosToCanvas() ) );
    
    gl::pushMatrices();
    gl::translate( widgetPos );
    
    gl::color( mLabelCol );
    mFontSmall->drawString( mLabel, Vec2f( 20, 11 ) );
    
    
    if ( mFeature->isEnable() )
    {
        if ( mXtract->isCalibrating() )
        {
            mNumericMin->SetText( to_string( mFeature->getResultMin() ) );
            mNumericMax->SetText( to_string( mFeature->getResultMax() ) );
        }
        
        float min   = mNumericMin->GetFloatFromText(); //boost::lexical_cast<double>( mNumericMin->GetText().c_str() );
        float max   = mNumericMax->GetFloatFromText(); //boost::lexical_cast<double>( mNumericMax->GetText().c_str() );

//        float min   = mFeature->getResultMin();
//        float max   = mFeature->getResultMax();
        
        float val   = (float)mGainSlider->GetFloatValue() * ( (*mFeature->getResult().get()) - min ) / ( max - min );
//        val         = math<float>::clamp( val, 0.0f, 1.0f );
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
