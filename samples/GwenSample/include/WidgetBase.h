/*
 *  WidgetBase.h
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */

#pragma once

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/CheckBox.h"
#include "Gwen/Controls/VerticalSlider.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/WindowControl.h"
#include "Gwen/Controls/Properties.h"
#include "Gwen/Controls/Property/Checkbox.h"
#include "Gwen/Controls/Property/Text.h"

//#include <boost/lexical_cast.hpp>

#include "ciXtract.h"

#define CI_XTRACT_WIDGET_WIDTH        210
#define CI_XTRACT_WIDGET_HEIGHT       80


class WidgetBase : public Gwen::Controls::Base {
    
public:
    
	WidgetBase( Gwen::Controls::Base *parent, std::string label, ciXtractFeatureRef feature, ciXtractRef xtract )
    : WidgetBase::WidgetBase( parent )
    {
        mLabel      = label;
        mXtract     = xtract;
        mFeature    = feature;
        mBuffCol    = Color( 0.27f, 0.47f, 0.98f );
        mBuffBgCol  = Color( 0.9f, 0.9f, 0.9f );
        mLabelCol   = Color( 0.27f, 0.27f, 0.27f );
        mValCol     = Color( 0.27f, 0.27f, 0.27f );
        mClamp      = true;
        mMin        = 0.0f;
        mMax        = 1.0f;
        
        mOscEnable  = false;;
        mOscAddress = "/" + mLabel;
        
        // Properties window
        mOptionsButton  = new Gwen::Controls::Button( this );
        mOptionsButton->SetText( "o" );
        mOptionsButton->SetBounds( CI_XTRACT_WIDGET_WIDTH - 10, 0, 10, 10 );
        mOptionsButton->onDown.Add( this, &WidgetBase::toggleProperties );
        
        mPropertiesWindow  = new Gwen::Controls::WindowControl( parent );
        mPropertiesWindow->SetTitle( mFeature->getName() );
        mPropertiesWindow->SetSize( CI_XTRACT_WIDGET_WIDTH, 200 );
        mPropertiesWindow->Hide();
        
        mProperties = new Gwen::Controls::Properties( mPropertiesWindow );
        mProperties->Dock( Gwen::Pos::Fill );
        
        Gwen::Controls::PropertyRow* pRow;
        
        pRow = mProperties->Add( "Min" );
        pRow->onChange.Add( this, &WidgetBase::onPropertyChange );
        pRow->GetProperty()->SetPropertyValue( to_string( mMin ) );
        
        pRow = mProperties->Add( "Max", new Gwen::Controls::Property::Text( mProperties ) );
        pRow->onChange.Add( this, &WidgetBase::onPropertyChange );
        pRow->GetProperty()->SetPropertyValue( to_string( mMax ) );
        
        pRow = mProperties->Add( "Clamp", new Gwen::Controls::Property::Checkbox( mProperties ), "1" );
        pRow->onChange.Add( this, &WidgetBase::onPropertyChange );
        pRow->GetProperty()->SetPropertyValue( to_string( mClamp ) );
        
        pRow = mProperties->Add( "OSC addr", new Gwen::Controls::Property::Text( mProperties ) );
        pRow->onChange.Add( this, &WidgetBase::onPropertyChange );
        pRow->GetProperty()->SetPropertyValue( mOscAddress );
        
        pRow = mProperties->Add( "OSC", new Gwen::Controls::Property::Checkbox( mProperties ), "1" );
        pRow->onChange.Add( this, &WidgetBase::onPropertyChange );
        pRow->GetProperty()->SetPropertyValue( to_string( mOscEnable ) );
        
        // Calibration button
        mCalibButton  = new Gwen::Controls::Button( this );
        mCalibButton->SetText( "-" );
        mCalibButton->SetBounds( CI_XTRACT_WIDGET_WIDTH - 10, 12, 10, 10 );
        mCalibButton->onDown.Add( this, &WidgetBase::triggerCalibration );
    }
    
    WidgetBase( Gwen::Controls::Base *parent ) : Gwen::Controls::Base( parent, "cigwen sample ScalarWidget" ) {}

	virtual ~WidgetBase() {}

	virtual void Render( Gwen::Skin::Base* skin ) {}
	virtual void RenderUnder( Gwen::Skin::Base* skin ) {}
    
    
protected:

    virtual bool update() {
        
        if ( !mFeature->isEnable() )
            return false;
    
        if ( mXtract->isCalibrating() )
        {
            mMin = mFeature->getResultMin();
            mMax = mFeature->getResultMax();
            mProperties->Find( "Min" )->GetProperty()->SetPropertyValue( to_string(mMin) );
            mProperties->Find( "Max" )->GetProperty()->SetPropertyValue( to_string(mMax) );
        }
        
        return true;
    }
    
    virtual void onPropertyChange( Gwen::Controls::Base* pControl )
    {
        Gwen::Controls::PropertyRow* pRow = ( Gwen::Controls::PropertyRow* ) pControl;
        string label = pRow->GetLabel()->GetText().c_str();
        
        if ( label == "Clamp" )
            mClamp = atof( pRow->GetProperty()->GetPropertyValue().c_str() );
        
        else if ( label == "Min" )
            mMin = atof( pRow->GetProperty()->GetPropertyValue().c_str() ); // mMin = boost::lexical_cast<double>( pRow->GetProperty()->GetPropertyValue().c_str() );
        
        else if ( label == "Max" )
            mMax = atof( pRow->GetProperty()->GetPropertyValue().c_str() );
        
        else if ( label == "OSC" )
            mOscEnable = atof( pRow->GetProperty()->GetPropertyValue().c_str() );
        
        else if ( label == "OSC addr" )
            mOscAddress = atof( pRow->GetProperty()->GetPropertyValue().c_str() );
    }
    
    
    virtual void toggleFeature( Gwen::Controls::Base* pControl ) {}
    
    void toggleProperties()
    {
        if ( mPropertiesWindow->Hidden() )
        {
            mPropertiesWindow->Show();
            mPropertiesWindow->SetPos( GetPos().x, GetPos().y + GetSize().y + 3 );
        }
        else
            mPropertiesWindow->Hide();
    }
    
    void triggerCalibration()
    {
        mXtract->calibrateFeature( mFeature );
    }
    
    Gwen::Controls::VerticalSlider  *mGainSlider;
    Gwen::Controls::Button          *mOptionsButton;
    Gwen::Controls::Button          *mCalibButton;
    Gwen::Controls::Properties      *mProperties;
    Gwen::Controls::WindowControl   *mPropertiesWindow;
    Gwen::Controls::CheckBox        *mEnableCheckBox;

    ciXtractRef                     mXtract;
    ciXtractFeatureRef              mFeature;
    
    ci::Rectf                       mBuffRect;
    ci::Rectf                       mValRect;
    ci::Rectf                       mWidgetRect;
    
    ci::Color                       mBuffCol;
    ci::Color                       mBuffBgCol;
    ci::Color                       mLabelCol;
    ci::Color                       mValCol;
    std::shared_ptr<double>         mData;
    
    std::string                     mLabel;
    bool                            mClamp;
    double                          mMin;
    double                          mMax;
    
    bool                            mOscEnable;
    std::string                     mOscAddress;
};