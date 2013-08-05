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

#define CI_XTRACT_CALIBRATION_DURATION 3.0f

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
    
    std::vector<ciXtractFeatureRef> getFeatures() { return mFeatures; };
    
    ciXtractFeatureRef getFeature( xtract_features_ feature );
    
    std::shared_ptr<double> getFeatureResult( xtract_features_ feature )
    {
        return getFeature(feature)->getResult();
    }
    
    bool isCalibrating() { return !mCalibrationFeatures.empty(); }

    void calibrateFeatures();
    
    void calibrateFeature( ciXtractFeatureRef feature );
    
    void calibrateFeature( xtract_features_ featureEnum );
    
    std::shared_ptr<double> getPcmData() { return mPcmData; }
    

private:
    
    ciXtract( audio::Input source );

    void initFeatures();
    
    bool featureDependsOn( xtract_features_ this_feature, xtract_features_ test_feature );
    
    void updateCalibration();
    
    
private:
    
    struct ciXtractFeatureCalibration
    {
        ciXtractFeatureRef  feature;
        double              StartedAt;
    };
    
    std::vector<ciXtractFeatureRef>             mFeatures;
    std::vector<ciXtractFeatureCalibration>     mCalibrationFeatures;
    
    ci::audio::Input                            mInputSource;
	audio::PcmBuffer32fRef                      mPcmBuffer;
    std::shared_ptr<double>                     mPcmData;

    ci::gl::TextureFontRef                      mFontSmall;
    
};

#endif
