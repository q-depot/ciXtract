/*
 *  ciLibXtract.h
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef CILIBXTRACT
#define CILIBXTRACT

#pragma once

#include "libxtract.h"
#include "cinder/audio/Input.h"

#include "cinder/gl/TextureFont.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define PCM_SIZE            1024
#define SAMPLERATE          44100

#define PERIOD              100
#define MFCC_FREQ_BANDS     13
#define MFCC_FREQ_MIN       20
#define MFCC_FREQ_MAX       20000
//#define SUB_BANDS           32


class ciLibXtract;
typedef std::shared_ptr<ciLibXtract>    ciLibXtractRef;


class ciLibXtract {
    
public:
    
    static ciLibXtractRef create( audio::Input source ) { return ciLibXtractRef( new ciLibXtract( source ) ); }
    
    ~ciLibXtract();
    
    void update();
    
    void enableFeature( xtract_features_ feature );
    
    void disableFeature( xtract_features_ feature );
    
    void debug();
    
    std::shared_ptr<double> getVectorFeature( xtract_features_ feature );
    
    double getScalarFeature( xtract_features_ feature )
    {
        return mScalarValues[feature];
    }
    
    
public:
    
    static std::map<xtract_features_,std::vector<xtract_features_>> xtract_features_dependencies;
    
    
    enum FeatureType {
        VECTOR_FEATURE,
        SCALAR_FEATURE
    };
    
    struct FeatureParams {
        double      *data;
        int         N,
        void        *argv,
        double      *result
    };
    
    
    struct FeatureCallback {
        std::string             name;
        std::function<void()>   cb;
        bool                    enable;
        FeatureType             type;
        FeatureParams           params;
    };

private:
    
    ciLibXtract( audio::Input source );
    
    void init();

    void updateCallbacks();
    
    void xtractFeature( xtract_features_ feature );
    
    bool featureDependsOn( xtract_features_ this_feature, xtract_features_ test_feature );
    
    // ------------------------------------ //
    //              Callbacks               //
    // ------------------------------------ //
    
private:
    
    void updateMean();
    void updateVariance();
    void updateStandardDeviation();
    void updateAverageDeviation();
    void updateSkewness();
    void updateKurtosis();
    void updateSpectralMean();
    void updateSpectralVariance();
    void updateSpectralStandardDeviation();
    void updateSpectralSkewness();
    void updateSpectralKurtosis();
    void updateSpectralCentroid();
    void updateIrregularityK();
    void updateIrregularityJ();
    void updateTristimulus1();

    void updateSmoothness();
    void updateSpread();
    void updateZcr();
    void updateRollOff();
    void updateLoudness();
    void updateFlatness();
    void updateFlatnessDb();
    
    void updateTonality();
//    void updateCrest();
//    void updateNoisiness();
    void updateRmsAmplitude();
    void updateSpectralInharmonicity();
    void updatePower();
    void updateOddEvenRatio();
    void updateSharpness ();
    
    
    
    
    void updateSpectralSlope();
    void updateLowestValue();
    void updateHighestValue();
    void updateSum();
    void updateNonZeroCount();
//    void updateHps();
    void updateF0();
//    void updateFailsafeF0();
//    void updateLnorm();
//    void updateFlux();
//    void updateAttackTime();
//    void updateDecayTime();
//    void updateDifferenceVector();

    
    void updateSpectrum();
    

//    void updateAutoCorrelationFft();
    void updateMfcc();
//     void updateDct();
//     void updateAutocorrelation();
//     void updateAmdf();
//     void updateAsdf();
     void updateBarkCoefficients();
//
    void updatePeakSpectrum();
    void updateHarmonicSpectrum();
//    void updateLpc();
//    void updateLpcc();
//    void updateSubbands();
//    
//    void updateWindowed();

    

    // TEMPORARY PUBLIC !!! <<<<<<<<<<<<<<
public:
    std::map<xtract_features_,FeatureCallback>  mCallbacks;
    
private:
    
    ci::audio::Input                            mInputSource;
	audio::PcmBuffer32fRef                      mPcmBuffer;
    std::shared_ptr<double>                     mPcmData;
    std::shared_ptr<double>                     mSpectrum;
    std::shared_ptr<double>                     mPeakSpectrum;
    std::shared_ptr<double>                     mHarmonicSpectrum;
    std::shared_ptr<double>                     mBarks;
    
    std::shared_ptr<int>                        mBarkBandLimits;

    std::shared_ptr<double>                     mMfccs;
    xtract_mel_filter                           mMelFilters;
    
    double  mScalarValues[XTRACT_FEATURES];     // the array is bigger but at least you can refer to each item using xtract_features_
    
    std::map<std::string,double>                mParams;
    
    double                                      _argd[4];
    
    ci::gl::TextureFontRef                      mFontSmall;
    
};

#endif
