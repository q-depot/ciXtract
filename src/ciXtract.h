/*
 *  ciXtract.h
 *
 *  Created by Andrea Cuius
 *  The MIT License (MIT)
 *  Copyright (c) 2014 Nocte Studio Ltd.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef CI_XTRACT
#define CI_XTRACT

#pragma once
#include "cinder/app/App.h"
#include "ciXtractScalarFeature.h"
#include "ciXtractVectorFeature.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciXtract;
typedef std::shared_ptr<ciXtract>       ciXtractRef;


class ciXtract {

public:
    
	static ciXtractRef create() { return ciXtractRef( new ciXtract() ); }
    
    ~ciXtract();
    
    //! process the data
    void update( const float *pcmData, float pcmGain = 1.0f );
    
    //! enable a feature
    bool enableFeature( xtract_features_ feature );
    
    //! disable a feature
    void disableFeature( xtract_features_ feature );
    
    //! toggle a feature
    void toggleFeature( xtract_features_ feature );
    
    //! return all the available features
    std::vector<ciXtractFeatureRef> getFeatures() { return mFeatures; };
    
    //! return all the features enabled
    std::vector<ciXtractFeatureRef> getActiveFeatures()
    {
        std::vector<ciXtractFeatureRef> features;
        for( size_t k=0; k < mFeatures.size(); k++ )
            if ( mFeatures[k]->isEnable() )
                features.push_back( mFeatures[k] );
        
        return features;
    };
    
    //! get a specific feature
    ciXtractFeatureRef getFeature( xtract_features_ feature );
    
    //! get raw feature results
    std::shared_ptr<double> getFeatureDataRaw( xtract_features_ feature )
    {
        return getFeature(feature)->getDataRaw();
    }

    //! get feature results
    std::shared_ptr<double> getFeatureData( xtract_features_ feature )
    {
        return getFeature(feature)->getData();
    }

    //! get PCM data
    std::shared_ptr<double> getPcmData() { return mPcmData; }
    
    //! list all the available features, the enumarators can be used to identify or toggle the features
    void listFeatures();
    
    void enableAllFeatures()
    {
        for( size_t k=0; k < mFeatures.size(); k++ )
            enableFeature( mFeatures[k]->getEnum() );
    }
    
    
private:
    
    ciXtract();

    void initFeatures();
    
    bool featureDependsOn( xtract_features_ this_feature, xtract_features_ test_feature );
    
    
private:
    
    std::vector<ciXtractFeatureRef>     mFeatures;
    std::shared_ptr<double>             mPcmData, mPcmDataRaw;
    double                              *mWindowFunc;
    
};

#endif
