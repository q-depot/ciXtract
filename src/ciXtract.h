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
    
    //! process the data
    void update( const float *pcmData );
    
    //! enable a feature
    void enableFeature( xtract_features_ feature );
    
    //! disable a feature
    void disableFeature( xtract_features_ feature );
    
    //! return all the features enabled
    std::vector<ciXtractFeatureRef> getActiveFeatures() { return mActiveFeatures; };
    
    //! get the active feature
    ciXtractFeatureRef getActiveFeature( xtract_features_ feature );
    
    //! get the available feature
    ciXtractFeatureRef getAvailableFeature( xtract_features_ feature )
    {
        if ( feature < XTRACT_FEATURES )
            return mAvailableFeatures[feature];
        else
            return ciXtractFeatureRef();
    }
    
    //! get raw feature results
    DataBuffer getFeatureDataRaw( xtract_features_ feature )
    {
        ciXtractFeatureRef f = getActiveFeature(feature);
        
        if ( f )
            return f->getDataRaw();
        else
            return DataBuffer();
    }

    //! get feature results
    DataBuffer getFeatureData( xtract_features_ feature )
    {
        ciXtractFeatureRef f = getActiveFeature(feature);
        
        if ( f )
            return f->getData();
        else
            return DataBuffer();
    }
        
    //! get PCM data
    DataBuffer getPcmData() { return mPcmData; }
    
    //! list all the available features, the enumarators can be used to identify or toggle the features
    void listFeatures();
    
    //! draw the PCM waveform
	static void drawPcm( ci::Rectf rect, const float *pcmData, size_t pcmSize );

    //! draw the feature data
    static void drawData(   ciXtractFeatureRef  feature,
                            Rectf               rect,
                            bool                drawRaw     = false,
                            ci::ColorA          plotCol     = ci::ColorA::white(),
                            ci::ColorA          bgCol       = ci::ColorA( 1.0f, 1.0f, 1.0f, 0.1f ),
                            ci::ColorA          labelCol    = ci::ColorA::white() );
    
private:
    
    ciXtract();

    void initFeatures();
    
    bool featureDependsOn( xtract_features_ this_feature, xtract_features_ test_feature );
    
    void processData();
    
private:
    
//    std::vector<ciXtractFeatureRef>     mFeatures;
    
    ciXtractFeatureRef                  mAvailableFeatures[XTRACT_FEATURES];    //! an array of XTRACT_FEATURES with all the available features
    std::vector<ciXtractFeatureRef>     mActiveFeatures;                        //! a vector contaning a pointer to each feature enabled
    
    DataBuffer                          mPcmData;
    int                                 mLastUpdateAt;
    
};

#endif
