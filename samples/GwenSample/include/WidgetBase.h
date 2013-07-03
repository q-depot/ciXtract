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
#include "Gwen/Controls/NumericUpDown.h"

#include "ciXtract.h"


class WidgetBase : public Gwen::Controls::Base {
    
public:
    
	WidgetBase( Gwen::Controls::Base *parent, std::string label, ciXtractFeatureRef feature, ciXtractRef xtract )
    : WidgetBase::WidgetBase( parent )
    {
        mLabel      = label;
        mXtract     = xtract;
        mFeature    = feature;
        mVal        = mXtract->getScalarFeaturePtr( mFeature->getEnum() );
        mBuffCol    = Color( 0.27f, 0.47f, 0.98f );
        mBuffBgCol  = Color( 0.9f, 0.9f, 0.9f );
        mLabelCol   = Color( 0.27f, 0.27f, 0.27f );
        mValCol     = Color( 0.27f, 0.27f, 0.27f );
    }
    
    WidgetBase( Gwen::Controls::Base *parent ) : Gwen::Controls::Base( parent, "cigwen sample ScalarWidget" ) {}

	virtual ~WidgetBase() {}

	virtual void Render( Gwen::Skin::Base* skin ) {}
	virtual void RenderUnder( Gwen::Skin::Base* skin ) {}
    
    
protected:

    virtual void toggleFeature( Gwen::Controls::Base* pControl ) {}
    
    Gwen::Controls::VerticalSlider  *mGainSlider;
    Gwen::Controls::CheckBox        *mCheckBox;
    Gwen::Controls::TextBoxNumeric  *mNumericMin;
    Gwen::Controls::TextBoxNumeric  *mNumericMax;

    std::string                     mLabel;
    double                          *mVal;
    
    ciXtractRef                     mXtract;
    ciXtractFeatureRef              mFeature;
    
    ci::Rectf                       mBuffRect;
    ci::Rectf                       mValRect;
    ci::Rectf                       mWidgetRect;
    
    ci::Color                       mBuffCol;
    ci::Color                       mBuffBgCol;
    ci::Color                       mLabelCol;
    ci::Color                       mValCol;
    
};