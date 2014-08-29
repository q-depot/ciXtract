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
#include "cinder/app/App.h"
#include "ciXtractFeature.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define CI_XTRACT_CALIBRATION_DURATION 3.0f

class ciXtract;
typedef std::shared_ptr<ciXtract>       ciXtractRef;


class ciXtract {

public:
    
	static ciXtractRef create() { return ciXtractRef( new ciXtract() ); }
    
    ~ciXtract();
    
    void update( const float *pcmData );
    
    void enableFeature( xtract_features_ feature );
    
    void disableFeature( xtract_features_ feature );
    
    void toggleFeature( xtract_features_ feature );
    
    std::vector<ciXtractFeatureRef> getFeatures() { return mFeatures; };
    
    ciXtractFeatureRef getFeature( xtract_features_ feature );
    
    std::shared_ptr<double> getFeatureResultsRaw( xtract_features_ feature )
    {
        return getFeature(feature)->getResultsRaw();
    }
    
    bool isCalibrating() { return !mCalibrationFeatures.empty(); }

    void calibrateFeatures();
    
    void calibrateFeature( ciXtractFeatureRef feature );
    
    void calibrateFeature( xtract_features_ featureEnum );
    
    std::shared_ptr<double> getPcmData() { return mPcmData; }
    

private:
    
    ciXtract();

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
    
    std::shared_ptr<double>                     mPcmData;
    
};

#endif
