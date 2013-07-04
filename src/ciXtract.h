/*
 *  ciXtract.h
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef CI_XTRACT
#define CI_XTRACT

#pragma once

#include "libxtract.h"
#include "cinder/audio/Input.h"

#include "cinder/gl/TextureFont.h"
#include "ciXtractFeature.h"

using namespace ci;
using namespace ci::app;
using namespace std;



class ciXtract;
typedef std::shared_ptr<ciXtract>       ciXtractRef;


class ciXtract {

public:
    
    static ciXtractRef create( audio::Input source ) { return ciXtractRef( new ciXtract( source ) ); }
    
    ~ciXtract();
    
    void update();
    
    void enableFeature( xtract_features_ feature );
    
    void disableFeature( xtract_features_ feature );
    
    void toggleFeature( xtract_features_ feature );
    
    ciXtractFeatureRef getFeature( xtract_features_ feature );
    
    bool isCalibrating() { return mRunCalibration > 0; }

    void autoCalibration();
    
    std::vector<ciXtractFeatureRef> getFeatures() { return mFeatures; };
    
    std::shared_ptr<double> getPcmData() { return mPcmData; }
    
    
private:
    
    void updateCalibration();

    double mRunCalibration;

    
private:
    
    ciXtract( audio::Input source );
    
    void init();

    void initFeatures();
    
    bool featureDependsOn( xtract_features_ this_feature, xtract_features_ test_feature );
    
private:

    std::vector<ciXtractFeatureRef>             mFeatures;
    
    ci::audio::Input                            mInputSource;
	audio::PcmBuffer32fRef                      mPcmBuffer;
    std::shared_ptr<double>                     mPcmData;

    ci::gl::TextureFontRef                      mFontSmall;
    
};

#endif
