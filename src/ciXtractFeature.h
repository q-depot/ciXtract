/*
 *  ciXtractFeature.h
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef CI_XTRACT_FEATURE
#define CI_XTRACT_FEATURE

#pragma once
#include <boost/algorithm/string.hpp>
#include <map>
#include <vector>
#include "xtract/libxtract.h"

static const std::string xtract_features_names[XTRACT_FEATURES] = {
    "XTRACT_MEAN",
    "XTRACT_VARIANCE",
    "XTRACT_STANDARD_DEVIATION",
    "XTRACT_AVERAGE_DEVIATION",
    "XTRACT_SKEWNESS",
    "XTRACT_KURTOSIS",
    "XTRACT_SPECTRAL_MEAN",
    "XTRACT_SPECTRAL_VARIANCE",
    "XTRACT_SPECTRAL_STANDARD_DEVIATION",
    "XTRACT_SPECTRAL_SKEWNESS",
    "XTRACT_SPECTRAL_KURTOSIS",
    "XTRACT_SPECTRAL_CENTROID",
    "XTRACT_IRREGULARITY_K",
    "XTRACT_IRREGULARITY_J",
    "XTRACT_TRISTIMULUS_1",
    "XTRACT_TRISTIMULUS_2",
    "XTRACT_TRISTIMULUS_3",
    "XTRACT_SMOOTHNESS",
    "XTRACT_SPREAD",
    "XTRACT_ZCR",
    "XTRACT_ROLLOFF",
    "XTRACT_LOUDNESS",
    "XTRACT_FLATNESS",
    "XTRACT_FLATNESS_DB",
    "XTRACT_TONALITY",
    "XTRACT_CREST",
    "XTRACT_NOISINESS",
    "XTRACT_RMS_AMPLITUDE",
    "XTRACT_SPECTRAL_INHARMONICITY",
    "XTRACT_POWER",
    "XTRACT_ODD_EVEN_RATIO",
    "XTRACT_SHARPNESS",
    "XTRACT_SPECTRAL_SLOPE",
    "XTRACT_LOWEST_VALUE",
    "XTRACT_HIGHEST_VALUE",
    "XTRACT_SUM",
    "XTRACT_NONZERO_COUNT",
    "XTRACT_HPS",
    "XTRACT_F0",
    "XTRACT_FAILSAFE_F0",
    "XTRACT_WAVELET_F0",
    "XTRACT_LNORM",
    "XTRACT_FLUX",
    "XTRACT_ATTACK_TIME",
    "XTRACT_DECAY_TIME",
    "XTRACT_DIFFERENCE_VECTOR",
    "XTRACT_AUTOCORRELATION",
    "XTRACT_AMDF",
    "XTRACT_ASDF",
    "XTRACT_BARK_COEFFICIENTS",
    "XTRACT_PEAK_SPECTRUM",
    "XTRACT_SPECTRUM",
    "XTRACT_AUTOCORRELATION_FFT",
    "XTRACT_MFCC",
    "XTRACT_DCT",
    "XTRACT_HARMONIC_SPECTRUM",
    "XTRACT_LPC",
    "XTRACT_LPCC",
    "XTRACT_SUBBANDS",
    "XTRACT_WINDOWE"
};


#define CIXTRACT_PCM_SIZE           1024
#define CIXTRACT_FFT_SIZE           512
#define CIXTRACT_SAMPLERATE         22050 // 44100
#define CIXTRACT_SAMPLERATE_N       CIXTRACT_SAMPLERATE / (double)CIXTRACT_PCM_SIZE

#define CIXTRACT_PERIOD             100
#define CIXTRACT_MFCC_FREQ_BANDS    13
#define CIXTRACT_MFCC_FREQ_MIN      20
#define CIXTRACT_MFCC_FREQ_MAX      20000
#define CIXTRACT_SUBBANDS_N         32

#ifdef _MSC_VER
#ifndef isnan
#define isnan(x) ((x)!=(x))
#endif
#ifndef isinf
#define isinf(x) ((x)!=(x))
#endif
#endif


class ciXtractFeature;
typedef std::shared_ptr<ciXtractFeature>       ciXtractFeatureRef;

class ciXtractSpectrum;

class ciXtract;

typedef std::shared_ptr<double>     DataBuffer;

class FeatureParam;
typedef std::shared_ptr<FeatureParam>   FeatureParamRef;

class FeatureParam {
    
public:
    
    enum ParamType {
        PARAM_READONLY,
        PARAM_EDITABLE
    };
    
    static FeatureParamRef create( std::string name, double initValue, ParamType pType = PARAM_EDITABLE )
    {
        return FeatureParamRef( new FeatureParam( name, initValue, pType ) );
    }
    
    FeatureParam* addOption( std::string label, double value )
    {
        mOptions[label] = value;
        return this;
    }
    
    double getValue()       { return mVal; }
    double *getValuePtr()   { return &mVal; }
    
    std::string getName() { return mName; }
    
private:
    
    FeatureParam( std::string name, double initValue, ParamType pType ) : mName(name), mVal(initValue), mType(pType) {}
    
private:
    
    std::string                     mName;
    double                          mVal;
    std::map<std::string,double>    mOptions;
    ParamType                       mType;
    
    
};



class ciXtractFeature {

public:
    
    friend class ciXtract;
    
    virtual void update( int frameN );
    
public:

    template <class T>
    static ciXtractFeatureRef create( ciXtract *xtract )
    {
        return ciXtractFeatureRef( new T( xtract ) );
    }
    
    
    ///////////////////////////
    
    bool isEnable() { return mIsEnable; }
    
    xtract_features_ getEnum() { return mFeatureEnum; }
    
    double getMin() { return mMin; }
    double getMax() { return mMax; }
    
    std::string getName() { return xtract_features_names[mFeatureEnum]; }
    
    DataBuffer  getData()       { return mData; }
    DataBuffer  getDataRaw()    { return mDataRaw; }
    uint32_t    getDataSize()   { return mDataSize; }
    uint32_t    getBufferSize() { return mDataBufferSize; }
    
    double getValue( int k )    { return mData.get()[ k % mDataSize ]; }
    double getValueRaw( int k ) { return mDataRaw.get()[ k % mDataSize ]; }
    
    std::vector<FeatureParamRef>  getParams() { return mParams; }
    
//    void setParam( std::string name, double val )
//    {
//        mParams[name].val = val;
//    }
    
    
    float   getGain()       { return mGain; }
    float   getOffset()     { return mOffset; }
    float   getDamping()    { return mDamping; }
    bool    isLog()         { return mIsLog; }
    
    void setGain( float val )       { mGain = val; }
    void setOffset( float val )     { mOffset = val; }
    void setDamping( float val )    { mDamping = val; }
    void setLog( bool isLog )       { mIsLog = isLog; }
    
    bool isUpdated( int frameN )    { return mLastUpdateAt == frameN; }
    
protected:
    
    ciXtractFeature(    ciXtract                        *xtract,
                        xtract_features_                featureEnum,
                        uint32_t                        bufferSize,
                        xtract_features_                inputFeature        = (xtract_features_)(XTRACT_FEATURES),      // XTRACT_FEATURES is for the features that use the PCM as input data
                        std::vector<xtract_features_>   extraDependencies   = std::vector<xtract_features_>() );        // feature can have more dependencies, the input feature if != XTRACT_FEATURES, is automatically added
protected:
    
    bool checkDependencies( int frameN );
    
    void processData();
    
    bool prepareUpdate( int frameN );
    
    void doUpdate( int frameN, const double *inputData, const int inputDataSize, const void *args, double *outputData );
    void doUpdate( int frameN );
    
//    void updateWithArgdFeatures( int frameN, std::vector<xtract_features_> features );
    
    void updateWithPcm( int frameN );
    
protected:
    
    ciXtract                        *mXtract;
    
    xtract_features_                mFeatureEnum;
    xtract_features_                mInputFeatureEnum;
    std::vector<xtract_features_>   mDependencies;
    
    DataBuffer                      mDataRaw;           // raw data, no gain, damping etc. - spectrum features also include the frequency bins
    DataBuffer                      mData;              // processed data, spectrum features do NOT include frequency bins
    size_t                          mDataSize;          // results N size, DATA size only, no frequency bins
    size_t                          mDataBufferSize;    // results N size, DATA size only, no frequency bins
    
    // mResults parameters, these are used to process the mResultsRaw
    float                           mGain, mOffset, mDamping;
    bool                            mIsLog;
    
    double                          mMin;
    double                          mMax;
    
    bool                            mIsEnable;
    
    std::vector<FeatureParamRef>    mParams;
    double                          mArgd[4];

    int                             mLastUpdateAt;
    
};

#endif
