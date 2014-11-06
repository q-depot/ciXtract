/*
 *  ciXtractFeature.h
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2014 . All rights reserved.
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
#include "cinder/app/AppNative.h"

class ciXtractFeature;
typedef std::shared_ptr<ciXtractFeature>       ciXtractFeatureRef;

class ciXtractSpectrum;

class ciXtract;

enum ciXtractParamType {
    CI_XTRACT_PARAM_ENUM,
    CI_XTRACT_PARAM_BOOL,
    CI_XTRACT_PARAM_DOUBLE
};

struct ciXtractFeatureParam {
    double                          val;
    ciXtractParamType               type;
    std::map<std::string,double>    options;
};


#define CIXTRACT_PCM_SIZE           512//1024
#define CIXTRACT_FFT_SIZE           256
#define CIXTRACT_SAMPLERATE         44100
#define CIXTRACT_SAMPLERATE_N       CIXTRACT_SAMPLERATE / (double)CIXTRACT_PCM_SIZE

#define CIXTRACT_PERIOD             100
#define CIXTRACT_MFCC_FREQ_BANDS    13
#define CIXTRACT_MFCC_FREQ_MIN      20
#define CIXTRACT_MFCC_FREQ_MAX      20000
#define CIXTRACT_SUBBANDS_N         32

#define CIXTRACT_PCM_FEATURE        XTRACT_FEATURES
#define CIXTRACT_NO_FEATURE         ( XTRACT_FEATURES + 1 )

#ifdef _MSC_VER
    #ifndef isnan
        #define isnan(x) ((x)!=(x)) 
    #endif
    #ifndef isinf
        #define isinf(x) ((x)!=(x)) 
    #endif
#endif

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
    "XTRACT_MIDICENT",
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
    "XTRACT_WINDOWED",
    "XTRACT_SMOOTHE"
};

struct InputDataBuffer {
    
    std::shared_ptr<double>     data;
    size_t                      dataSize;
};

class ciXtractFeature {

public:

    template <class T>
    static ciXtractFeatureRef create( ciXtract *xtract, std::string enumStr )
    {
        return ciXtractFeatureRef( new T( xtract, enumStr ) );
    }
    
    virtual void update( int frameN );
    
    virtual void processData( int frameN );

    virtual void doUpdate( int frameN ) { ci::app::console() << "NEVER CALL THIS!" << std::endl; }
    
    virtual void enable( bool isEnable );
    
    bool isEnable() { return mIsEnable; }
    
    xtract_features_ getEnum() { return mFeature; }
    
    double getMin() { return mMin; }
    double getMax() { return mMax; }
    
    std::string getName() { return mName; }
    
    std::shared_ptr<double>         getData()       { return mData; }
    std::shared_ptr<double>         getDataRaw()    { return mDataRaw; }
    uint32_t                        getDataSize()   { return mDataSize; }
    uint32_t                        getBufferSize() { return mDataBufferSize; }
    
    double getDataValue( int k ) { return mData.get()[ k % mDataSize ]; }
    double getDataValueRaw( int k ) { return mDataRaw.get()[ k % mDataSize ]; }
    
    std::map<std::string,ciXtractFeatureParam>  getParams() { return mParams; }
    
    void setParam( std::string name, double val )
    {
        mParams[name].val = val;
    }
    
    float   getGain()       { return mGain; }
    float   getOffset()     { return mOffset; }
    float   getDamping()    { return mDamping; }
    bool    isLog()         { return mIsLog; }
    
    void setGain( float val )       { mGain = val; }
    void setOffset( float val )     { mOffset = val; }
    void setDamping( float val )    { mDamping = val; }
    void setLog( bool isLog )       { mIsLog = isLog; }
    
    std::vector<xtract_features_> getDependencies() { return mDependencies; }
    
protected:
    
	// in VS you can create a struct with {..} but not assign {..} to an existing one, so we use this method until VS will become a decent platform
	static ciXtractFeatureParam createFeatureParam( double val, ciXtractParamType type, std::map<std::string,double> options )
	{
		ciXtractFeatureParam param = { val, type, options };
		return param;
	}

    
    bool checkDependencies( int frameN );
    
    bool isUpdated( int frameN )    { return mLastUpdateAt == frameN; }
    
protected:
    
    ciXtractFeature( ciXtract *xtract, xtract_features_ feature, std::string name, xtract_features_ inputFeature, uint32_t resultsN = 1, int resultArraySize = -1 );

protected:
    
    ciXtract                        *mXtract;
    xtract_features_                mFeature;
    std::string                     mName;
    std::vector<xtract_features_>   mDependencies;

    xtract_features_                mInputFeature;
    InputDataBuffer                 mInputBuffer;
    
    std::shared_ptr<double>         mDataRaw;        // raw data, no gain, damping etc. - spectrum features also include the frequency bins
    std::shared_ptr<double>         mData;           // processed data, spectrum features do NOT include frequency bins
    uint32_t                        mDataSize;          // results N size, DATA size only, no frequency bins
    int                             mDataBufferSize;
    
    // mResults parameters, these are used to process the mResultsRaw
    float                           mGain, mOffset, mDamping;
    bool                            mIsLog;
    
    double                          mMin;
    double                          mMax;
    
    bool                            mIsEnable;
    bool                            mIsInit;
    double                          mArgd[4];

    std::map<std::string,ciXtractFeatureParam>    mParams;
    
    int                             mLastUpdateAt;
};

#endif
