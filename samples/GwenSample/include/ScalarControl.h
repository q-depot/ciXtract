#pragma once

#include "Gwen/Controls/Base.h"
#include "cinder/Matrix44.h"
#include "cinder/Camera.h"
#include "cinder/gl/Texture.h"

#include "Gwen/Controls/ProgressBar.h"

class ScalarControl : public Gwen::Controls::Base {
    
public:
    
	ScalarControl( Gwen::Controls::Base *parent, std::string label, double *val );
    ScalarControl( Gwen::Controls::Base *parent );
	virtual ~ScalarControl();

	virtual void Render( Gwen::Skin::Base* skin );
	virtual void RenderUnder( Gwen::Skin::Base* skin );
    
    
private:

	void draw2d();
	void draw3d();

	ci::Matrix44f           mCubeRotation;
	ci::CameraPersp         mCamera;
	ci::gl::Texture         mImageTex;
    
    Gwen::Controls::ProgressBar *mValBar;
	

    std::string             mLabel;
    double                  *mVal;
};