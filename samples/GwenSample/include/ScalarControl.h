#pragma once

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/CheckBox.h"
#include "Gwen/Controls/VerticalSlider.h"
#include "Gwen/Controls/NumericUpDown.h"

#include <boost/circular_buffer.hpp>

#include "ciLibXtract.h"

#define SCALAR_CONTROL_WIDTH        210
#define SCALAR_CONTROL_HEIGHT       80
#define SCALAR_CONTROL_PADDING      5
#define SCALAR_CONTROL_BUFF_SIZE    100

class ScalarControl : public Gwen::Controls::Base {
    
public:
    
	ScalarControl( Gwen::Controls::Base *parent, std::string label, ciLibXtract::FeatureCallback *cb, ciLibXtractRef xtract );
    ScalarControl( Gwen::Controls::Base *parent );
	virtual ~ScalarControl();

	virtual void Render( Gwen::Skin::Base* skin );
	virtual void RenderUnder( Gwen::Skin::Base* skin );
    
    
private:

    void toggleFeature( Gwen::Controls::Base* pControl );
    
    Gwen::Controls::VerticalSlider  *mGainSlider;
    Gwen::Controls::CheckBox        *mCheckBox;
    Gwen::Controls::TextBoxNumeric  *mNumericMin;
    Gwen::Controls::TextBoxNumeric  *mNumericMax;

    std::string                     mLabel;
    double                          *mVal;
    
    ciLibXtractRef                  mXtract;
    ciLibXtract::FeatureCallback    *mCb;
    
    boost::circular_buffer<double>  mBuff;
    
    ci::Rectf                       mBuffRect;
    ci::Rectf                       mValRect;
    ci::Rectf                       mWidgetRect;
};