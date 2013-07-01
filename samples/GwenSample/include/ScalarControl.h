#pragma once

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/ProgressBar.h"
#include "Gwen/Controls/NumericUpDown.h"
#include "Gwen/Controls/CheckBox.h"

#include "ciLibXtract.h"

#define SCALAR_CONTROL_WIDTH   250
#define SCALAR_CONTROL_HEIGHT  50

class ScalarControl : public Gwen::Controls::Base {
    
public:
    
	ScalarControl( Gwen::Controls::Base *parent, std::string label, ciLibXtract::FeatureCallback *cb, ciLibXtractRef xtract );
    ScalarControl( Gwen::Controls::Base *parent );
	virtual ~ScalarControl();

	virtual void Render( Gwen::Skin::Base* skin );
	virtual void RenderUnder( Gwen::Skin::Base* skin );
    
    
private:

    void toggleFeature( Gwen::Controls::Base* pControl );
    
    
    Gwen::Controls::ProgressBar     *mValBar;
    Gwen::Controls::NumericUpDown   *mGainNumeric;
    Gwen::Controls::CheckBox        *mCheckBox;
    
    std::string                     mLabel;
    double                          *mVal;
    
    ciLibXtractRef                  mXtract;
    ciLibXtract::FeatureCallback    *mCb;
};