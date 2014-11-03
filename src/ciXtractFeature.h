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
                        uint32_t                        resultsN            = 1,                                        // feature has at least 1 result
                        xtract_features_                inputFeature        = (xtract_features_)(XTRACT_FEATURES),      // XTRACT_FEATURES is for the features that use the PCM as input data
                        std::vector<xtract_features_>   extraDependencies   = std::vector<xtract_features_>() );        // feature can have more dependencies, the input feature if != XTRACT_FEATURES, is automatically added
protected:
    
    bool checkDependencies( int frameN );
    
    void processData();
    
    bool isReady( int frameN ) { return !isUpdated(frameN) && checkDependencies(frameN); }
    
    void doUpdate( int frameN, const double *inputData, const int inputDataSize, const void *args, double *outputData );
    
    void updateArgs();
    
protected:
    
    ciXtract                        *mXtract;
    
    xtract_features_                mFeatureEnum;
    xtract_features_                mInputFeatureEnum;
    std::vector<xtract_features_>   mDependencies;
    
    DataBuffer                      mDataRaw;           // raw data, no gain, damping etc. - spectrum features also include the frequency bins
    DataBuffer                      mData;              // processed data, spectrum features do NOT include frequency bins
    size_t                          mDataSize;          // results N size, DATA size only, no frequency bins
    
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


// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// *************************************** VECTOR FEATURES **************************************** //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //




// Spectrum
class ciXtractSpectrum : public ciXtractFeature {
    
public:
    ciXtractSpectrum( ciXtract *xtract );
    ~ciXtractSpectrum();
    void update( int frameN  );
};

// Bark
class ciXtractBark : public ciXtractFeature {
    
public:
    ciXtractBark( ciXtract *xtract );
    ~ciXtractBark() {}
    void update( int frameN );
private:
    std::shared_ptr<int> mBandLimits;
};

// Mfcc
class ciXtractMfcc : public ciXtractFeature {
    
public:
    ciXtractMfcc( ciXtract *xtract );
    ~ciXtractMfcc();
    void update( int frameN );
private:
    xtract_mel_filter   mMelFilters;
};


// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// *************************************** SCALAR FEATURES **************************************** //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
//    xtract_mean
//    xtract_variance
//    xtract_standard_deviation
//    xtract_average_deviation
//    xtract_skewness
//    xtract_kurtosis
//    xtract_spectral_mean
//    xtract_spectral_variance
//    xtract_spectral_standard_deviation
//    xtract_spectral_skewness
//    xtract_spectral_kurtosis
//    xtract_spectral_centroid
//    xtract_irregularity_k
//    xtract_irregularity_j
//    xtract_tristimulus_1
//    xtract_tristimulus_2
//    xtract_spread
//    xtract_zcr
//    xtract_rolloff
//    xtract_loudness
//    xtract_flatness
//    xtract_flatness_db
//    xtract_tonality
//    xtract_noisiness
//    xtract_rms_amplitude
//    xtract_spectral_inharmonicity
//    xtract_crest
//    xtract_power
//    xtract_odd_even_ratio
//    xtract_sharpness
//    xtract_spectral_slope
//    xtract_lowest_value
//    xtract_highest_value
//    xtract_sum
//    xtract_hps
//    xtract_f0
//    xtract_failsafe_f0
//    xtract_wavelet_f0
//    xtract_midicent
//    xtract_nonzero_count
//    xtract_peak

// Mean
class ciXtractMean : public ciXtractFeature {

public:
    ciXtractMean( ciXtract *xtract );
    ~ciXtractMean() {}
};



//    variance
class ciXtractVariance : public ciXtractFeature {
    
public:
    ciXtractVariance( ciXtract *xtract );
    ~ciXtractVariance() {}
};

// Standard Deviation
class ciXtractStandardDeviation : public ciXtractFeature {
    
public:
    ciXtractStandardDeviation( ciXtract *xtract );
    ~ciXtractStandardDeviation() {}
};

// Average Deviation
class ciXtractAverageDeviation : public ciXtractFeature {
    
public:
    ciXtractAverageDeviation( ciXtract *xtract );
    ~ciXtractAverageDeviation() {}
};

// Skewness
class ciXtractSkewness : public ciXtractFeature {
    
public:
    ciXtractSkewness( ciXtract *xtract );
    ~ciXtractSkewness() {}
};

// Kurtosis
class ciXtractKurtosis : public ciXtractFeature {
    
public:
    ciXtractKurtosis( ciXtract *xtract );
    ~ciXtractKurtosis() {}
};

// Spectral Mean
class ciXtractSpectralMean : public ciXtractFeature {
    
public:
    ciXtractSpectralMean( ciXtract *xtract );
    ~ciXtractSpectralMean() {}
};

// Spectral Variance
class ciXtractSpectralVariance : public ciXtractFeature {
    
public:
    ciXtractSpectralVariance( ciXtract *xtract );
    ~ciXtractSpectralVariance() {}
};

// Spectral Standard Deviation
class ciXtractSpectralStandardDeviation : public ciXtractFeature {
    
public:
    ciXtractSpectralStandardDeviation( ciXtract *xtract );
    ~ciXtractSpectralStandardDeviation() {}
};

// Spectral Skewness
class ciXtractSpectralSkewness : public ciXtractFeature {
    
public:
    ciXtractSpectralSkewness( ciXtract *xtract );
    ~ciXtractSpectralSkewness() {}
};

// Spectral Kurtosis
class ciXtractSpectralKurtosis : public ciXtractFeature {
    
public:
    ciXtractSpectralKurtosis( ciXtract *xtract );
    ~ciXtractSpectralKurtosis() {}
};

// Spectral Centroid
class ciXtractSpectralCentroid : public ciXtractFeature {
    
public:
    ciXtractSpectralCentroid( ciXtract *xtract );
    ~ciXtractSpectralCentroid() {}
};

// Irregularity_k
class ciXtractIrregularityK : public ciXtractFeature {
    
public:
    ciXtractIrregularityK( ciXtract *xtract );
    ~ciXtractIrregularityK() {}
};

// Irregularity_j
class ciXtractIrregularityJ : public ciXtractFeature {
    
public:
    ciXtractIrregularityJ( ciXtract *xtract );
    ~ciXtractIrregularityJ() {}
};

// Tristimulus_1
class ciXtractTristimulus1 : public ciXtractFeature {
    
public:
    ciXtractTristimulus1( ciXtract *xtract );
    ~ciXtractTristimulus1() {}
};

// Smoothness
class ciXtractSmoothness : public ciXtractFeature {
    
public:
    ciXtractSmoothness( ciXtract *xtract );
    ~ciXtractSmoothness() {}
};

// Spread
class ciXtractSpread : public ciXtractFeature {
    
public:
    ciXtractSpread( ciXtract *xtract );
    ~ciXtractSpread() {}
};

// Zcr
class ciXtractZcr : public ciXtractFeature {
    
public:
    ciXtractZcr( ciXtract *xtract );
    ~ciXtractZcr() {}
};

// Rolloff
class ciXtractRolloff : public ciXtractFeature {
    
public:
    ciXtractRolloff( ciXtract *xtract );
    ~ciXtractRolloff() {}
};

// Loudness
class ciXtractLoudness : public ciXtractFeature {
    
public:
    ciXtractLoudness( ciXtract *xtract );
    ~ciXtractLoudness() {}
};

// Flatness
class ciXtractFlatness : public ciXtractFeature {
    
public:
    ciXtractFlatness( ciXtract *xtract );
    ~ciXtractFlatness() {}
};

// Flatness db
class ciXtractFlatnessDb : public ciXtractFeature {
    
public:
    ciXtractFlatnessDb( ciXtract *xtract );
    ~ciXtractFlatnessDb() {}
};

// Tonality
class ciXtractTonality : public ciXtractFeature {
    
public:
    ciXtractTonality( ciXtract *xtract );
    ~ciXtractTonality() {}
};

// Noisiness
class ciXtractNoisiness : public ciXtractFeature {
    
public:
    ciXtractNoisiness( ciXtract *xtract );
    ~ciXtractNoisiness() {}
};

// Rms Amplitude
class ciXtractRmsAmplitude : public ciXtractFeature {
    
public:
    ciXtractRmsAmplitude( ciXtract *xtract );
    ~ciXtractRmsAmplitude() {}
};




//    spectral_inharmonicity
//    crest
//    power
//    odd_even_ratio
//    sharpness
//    spectral_slope
//    lowest_value
//    highest_value
//    sum
//    hps
//    f0
//    failsafe_f0
//    wavelet_f0
//    midicent
//    nonzero_count
//    peak


#endif
