/*
 *  ScalarWidget.h
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#pragma once

#include "WidgetBase.h"

#define VECTOR_CONTROL_WIDTH        210
#define VECTOR_CONTROL_HEIGHT       80


class VectorWidget : public WidgetBase {
    
public:
    
	VectorWidget( Gwen::Controls::Base *parent, std::string label, ciLibXtract::FeatureCallback *cb, ciLibXtractRef xtract );

	virtual ~VectorWidget() {}
    
	virtual void Render( Gwen::Skin::Base* skin );
    
    
private:
    
    void toggleFeature( Gwen::Controls::Base* pControl );

};