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

#include <boost/circular_buffer.hpp>
#include "WidgetBase.h"

#define SCALAR_CONTROL_WIDTH        210
#define SCALAR_CONTROL_HEIGHT       80
#define SCALAR_CONTROL_BUFF_SIZE    100


class ScalarWidget : public WidgetBase {
    
public:
    
	ScalarWidget( Gwen::Controls::Base *parent, std::string label, ciLibXtract::FeatureCallback *cb, ciLibXtractRef xtract );

	virtual ~ScalarWidget() {}
    
	virtual void Render( Gwen::Skin::Base* skin );
    
    
private:
    
    void toggleFeature( Gwen::Controls::Base* pControl );
    
    boost::circular_buffer<double>  mBuff;
    
};