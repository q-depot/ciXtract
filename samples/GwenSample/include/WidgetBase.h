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
#include "Gwen/Controls/Property/ComboBox.h"
#include "Gwen/Utility.h"

#include <boost/algorithm/string.hpp>
//#include <boost/lexical_cast.hpp>

#include "ciXtract.h"


class WidgetBase : public Gwen::Controls::Base {
    
public:
    
	WidgetBase( Gwen::Controls::Base *parent, std::string label, ciXtractFeatureRef feature, ciXtractRef xtract, Vec2i size )
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
        mDamping    = 0.85f;
        mOscEnable  = false;
        mRawData    = false;
        
        std::string addr = mLabel;
        boost::replace_all( addr, " ", "_");
        boost::algorithm::to_lower( addr );

        mOscAddress = "/" + addr;
        
        SetBounds( 0, 0, size.x, size.y );
        
        // Properties window
        mOptionsButton  = new Gwen::Controls::Button( this );
        mOptionsButton->SetText( "o" );
        mOptionsButton->SetBounds( GetSize().x - 10, 0, 10, 10 );
        mOptionsButton->onDown.Add( this, &WidgetBase::toggleProperties );
        
        mPropertiesWindow  = new Gwen::Controls::WindowControl( parent );
        mPropertiesWindow->SetTitle( mFeature->getName() );
        mPropertiesWindow->SetSize( GetSize().x, 200 );
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
        
        pRow = mProperties->Add( "Clamp", new Gwen::Controls::Property::Checkbox( mProperties ), std::to_string( mClamp ) );
        pRow->onChange.Add( this, &WidgetBase::onPropertyChange );
        pRow->GetProperty()->SetPropertyValue( to_string( mClamp ) );
        
        pRow = mProperties->Add( "Raw data", new Gwen::Controls::Property::Checkbox( mProperties ), std::to_string( mRawData ) );
        pRow->onChange.Add( this, &WidgetBase::onPropertyChange );
        pRow->GetProperty()->SetPropertyValue( to_string( mRawData ) );
        
        pRow = mProperties->Add( "Damping", new Gwen::Controls::Property::Text( mProperties ) );
        pRow->onChange.Add( this, &WidgetBase::onPropertyChange );
        pRow->GetProperty()->SetPropertyValue( to_string( mDamping ) );
        
        pRow = mProperties->Add( "OSC addr", new Gwen::Controls::Property::Text( mProperties ) );
        pRow->onChange.Add( this, &WidgetBase::onPropertyChange );
        pRow->GetProperty()->SetPropertyValue( mOscAddress );
        
        pRow = mProperties->Add( "OSC", new Gwen::Controls::Property::Checkbox( mProperties ), "1" );
        pRow->onChange.Add( this, &WidgetBase::onPropertyChange );
        pRow->GetProperty()->SetPropertyValue( to_string( mOscEnable ) );
        
        // Calibration button
        mCalibButton  = new Gwen::Controls::Button( this );
        mCalibButton->SetText( "-" );
        mCalibButton->SetBounds( GetSize().x - 10, 12, 10, 10 );
        mCalibButton->onDown.Add( this, &WidgetBase::triggerCalibration );
        
        
        // Feature properties        
        std::map<std::string,ciXtractFeatureParam>  params = mFeature->getParams();
        ciXtractFeatureParam                        p;
        std::string                                 pname;
        for ( std::map<std::string,ciXtractFeatureParam>::iterator it = params.begin(); it != params.end(); ++it )
        {
            pname   = it->first;
            p       = it->second;
            
            if ( p.type == CI_XTRACT_PARAM_BOOL )
            {
                pRow = mProperties->Add( pname, new Gwen::Controls::Property::Checkbox( mProperties ) );
            }
            else if ( p.type == CI_XTRACT_PARAM_DOUBLE )
            {
                pRow = mProperties->Add( pname );
            }
            else if ( p.type == CI_XTRACT_PARAM_ENUM )
            {
                Gwen::Controls::Property::ComboBox* pCombo = new Gwen::Controls::Property::ComboBox( mProperties );
                
                for ( std::map<std::string,double>::iterator opIt = p.options.begin(); opIt != p.options.end(); ++opIt )
                    pCombo->GetComboBox()->AddItem( Gwen::Utility::StringToUnicode( opIt->first ), std::to_string( opIt->second ) );
                
                pRow = mProperties->Add( pname, pCombo, "1" );
            }
            
            pRow->onChange.Add( this, &WidgetBase::onPropertyParamChange );
            pRow->GetProperty()->SetPropertyValue( to_string( p.val ) );
        }

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
        
        else if ( label == "Raw data" )
            mRawData = atof( pRow->GetProperty()->GetPropertyValue().c_str() );
        
        else if ( label == "Damping" )
            mDamping = atof( pRow->GetProperty()->GetPropertyValue().c_str() );
        
        else if ( label == "OSC addr" )
            mOscAddress = atof( pRow->GetProperty()->GetPropertyValue().c_str() );
    }
    
    virtual void onPropertyParamChange( Gwen::Controls::Base* pControl )
    {
        Gwen::Controls::PropertyRow* pRow   = ( Gwen::Controls::PropertyRow* ) pControl;
        string label                        = pRow->GetLabel()->GetText().c_str();
        double val                          = atof( pRow->GetProperty()->GetPropertyValue().c_str() );
        
        mFeature->setParam( label, val );
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
    
    double                          mDamping;
    
    bool                            mOscEnable;
    std::string                     mOscAddress;
    
    bool                            mRawData;
};