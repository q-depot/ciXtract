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

//#define BLOCKSIZE           2048
#define PCM_SIZE            1024
#define SAMPLERATE          44100
#define PERIOD              100
#define MFCC_FREQ_BANDS     32
#define MFCC_FREQ_MIN       20
#define MFCC_FREQ_MAX       20000
#define SUB_BANDS           32


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
    
    
private:
    
    static std::map<xtract_features_,std::vector<xtract_features_>> xtract_features_dependencies;
    
    struct FeatureCallback {
        std::string             name;
        std::function<void()>   cb;
        bool                    enable;
    };

private:
    
    ciLibXtract( audio::Input source );
    
    void init();

    void updateCallbacks();
    
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
    
    
    void updateSpectrum();
    
    
private:
    
    ci::audio::Input                            mInputSource;
	audio::PcmBuffer32fRef                      mPcmBuffer;
    std::shared_ptr<double>                     mPcmData;
    std::shared_ptr<double>                     mSpectrum;
    
    double                                      mMean;
    double                                      mVariance;
    double                                      mStandardDeviation;
    double                                      mAverageDeviation;
    
    
    std::map<xtract_features_,FeatureCallback>  mCallbacks;
    std::map<std::string,double>                mParams;
    
    double                                      _argd[4];
    
    ci::gl::TextureFontRef                      mFontSmall;
    
};

#endif
