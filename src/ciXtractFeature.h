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
    
    void updateArgs()
    {
        for( size_t k=0; k < mParams.size(); k++ )
            mArgd[k] = mParams[k] ? mParams[k]->getValue() : NULL;
    }
    
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

// Mean
class ciXtractMean : public ciXtractFeature {
    
public:
    //    static ciXtractFeatureRef create( ciXtract *xtract ) { return ciXtractFeatureRef( new ciXtractMean( xtract ) ); }
    ciXtractMean( ciXtract *xtract );
    ~ciXtractMean() {}
};


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


/*
// Autocorrelation
class ciXtractAutocorrelation : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractAutocorrelation( xtract, name ) ); }
    ~ciXtractAutocorrelation() {}
    void update();
    
private:
    ciXtractAutocorrelation( ciXtract *xtract, std::string name );
    
};

// Autocorrelation Fft
class ciXtractAutocorrelationFft : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractAutocorrelationFft( xtract, name ) ); }
    ~ciXtractAutocorrelationFft() {}
    void update();
    
private:
    ciXtractAutocorrelationFft( ciXtract *xtract, std::string name );
    
};

// Harmonic Spectrum
class ciXtractHarmonicSpectrum : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractHarmonicSpectrum( xtract, name ) ); }
    ~ciXtractHarmonicSpectrum() {}
    void update();
    
private:
    ciXtractHarmonicSpectrum( ciXtract *xtract, std::string name );
    
};

// Peak Spectrum
class ciXtractPeakSpectrum : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractPeakSpectrum( xtract, name ) ); }
    ~ciXtractPeakSpectrum() {}
    void update();
    
private:
    ciXtractPeakSpectrum( ciXtract *xtract, std::string name );
    
};

// SubBands
class ciXtractSubBands : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSubBands( xtract, name ) ); }
    ~ciXtractSubBands() {}
    void update();
    
private:
    ciXtractSubBands( ciXtract *xtract, std::string name );
    
};

// Mffc
class ciXtractMfcc : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractMfcc( xtract, name ) ); }
    ~ciXtractMfcc();
    void update();
    
private:
    ciXtractMfcc( ciXtract *xtract, std::string name );
    
    xtract_mel_filter   mMelFilters;
};

// Bark
class ciXtractBark : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractBark( xtract, name ) ); }
    ~ciXtractBark() {}
    void update();
    
private:
    ciXtractBark( ciXtract *xtract, std::string name );
    
    std::shared_ptr<int>    mBandLimits;
    
};


// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// *************************************** SCALAR FEATURES **************************************** //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //

// F0
class ciXtractF0 : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractF0( xtract, name ) ); }
    ~ciXtractF0() {}
    void update();
    
private:
    ciXtractF0( ciXtract *xtract, std::string name );
    
};

// Failsafe F0
class ciXtractFailsafeF0 : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractFailsafeF0( xtract, name ) ); }
    ~ciXtractFailsafeF0() {}
    void update();
    
private:
    ciXtractFailsafeF0( ciXtract *xtract, std::string name );
    
};


// Wavelet F0
class ciXtractWaveletF0 : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractWaveletF0( xtract, name ) ); }
    ~ciXtractWaveletF0() {}
    void update();
    
private:
    ciXtractWaveletF0( ciXtract *xtract, std::string name );
    
};

// Mean
class ciXtractMean : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractMean( xtract, name ) ); }
    ~ciXtractMean() {}
    void update();
    
private:
    ciXtractMean( ciXtract *xtract, std::string name );
    
};

// Variance
class ciXtractVariance : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractVariance( xtract, name ) ); }
    ~ciXtractVariance() {}
    void update();
    
private:
    ciXtractVariance( ciXtract *xtract, std::string name );
    
};

// Standard Deviation
class ciXtractStandardDeviation : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractStandardDeviation( xtract, name ) ); }
    ~ciXtractStandardDeviation() {}
    void update();
    
private:
    ciXtractStandardDeviation( ciXtract *xtract, std::string name );
    
};

// Average Deviation
class ciXtractAverageDeviation : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractAverageDeviation( xtract, name ) ); }
    ~ciXtractAverageDeviation() {}
    void update();
    
private:
    ciXtractAverageDeviation( ciXtract *xtract, std::string name );
    
};

// Skewness
class ciXtractSkewness : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSkewness( xtract, name ) ); }
    ~ciXtractSkewness() {}
    void update();
    
private:
    ciXtractSkewness( ciXtract *xtract, std::string name );
    
};

// Kurtosis
class ciXtractKurtosis : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractKurtosis( xtract, name ) ); }
    ~ciXtractKurtosis() {}
    void update();
    
private:
    ciXtractKurtosis( ciXtract *xtract, std::string name );
    
};

// Spectral Mean
class ciXtractSpectralMean : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSpectralMean( xtract, name ) ); }
    ~ciXtractSpectralMean() {}
    void update();
    
private:
    ciXtractSpectralMean( ciXtract *xtract, std::string name );
    
};

// Spectral Variance
class ciXtractSpectralVariance : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSpectralVariance( xtract, name ) ); }
    ~ciXtractSpectralVariance() {}
    void update();
    
private:
    ciXtractSpectralVariance( ciXtract *xtract, std::string name );
    
};

// Spectral Standard Deviation
class ciXtractSpectralStandardDeviation : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSpectralStandardDeviation( xtract, name ) ); }
    ~ciXtractSpectralStandardDeviation() {}
    void update();
    
private:
    ciXtractSpectralStandardDeviation( ciXtract *xtract, std::string name );
    
};

// Spectral Skewness
class ciXtractSpectralSkewness : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSpectralSkewness( xtract, name ) ); }
    ~ciXtractSpectralSkewness() {}
    void update();
    
private:
    ciXtractSpectralSkewness( ciXtract *xtract, std::string name );
    
};

// Spectral Kurtosis
class ciXtractSpectralKurtosis : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSpectralKurtosis( xtract, name ) ); }
    ~ciXtractSpectralKurtosis() {}
    void update();
    
private:
    ciXtractSpectralKurtosis( ciXtract *xtract, std::string name );
    
};

// Spectral Centroid
class ciXtractSpectralCentroid : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSpectralCentroid( xtract, name ) ); }
    ~ciXtractSpectralCentroid() {}
    void update();
    
private:
    ciXtractSpectralCentroid( ciXtract *xtract, std::string name );
    
};

// Irregularity K
class ciXtractIrregularityK : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractIrregularityK( xtract, name ) ); }
    ~ciXtractIrregularityK() {}
    void update();
    
private:
    ciXtractIrregularityK( ciXtract *xtract, std::string name );
    
};

// Irregularity J
class ciXtractIrregularityJ : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractIrregularityJ( xtract, name ) ); }
    ~ciXtractIrregularityJ() {}
    void update();
    
private:
    ciXtractIrregularityJ( ciXtract *xtract, std::string name );
    
};

// Tristimulus 1
class ciXtractTristimulus1 : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractTristimulus1( xtract, name ) ); }
    ~ciXtractTristimulus1() {}
    void update();
    
private:
    ciXtractTristimulus1( ciXtract *xtract, std::string name );
    
};

// Smoothness
class ciXtractSmoothness : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSmoothness( xtract, name ) ); }
    ~ciXtractSmoothness() {}
    void update();
    
private:
    ciXtractSmoothness( ciXtract *xtract, std::string name );
    
};

// Spread
class ciXtractSpread : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSpread( xtract, name ) ); }
    ~ciXtractSpread() {}
    void update();
    
private:
    ciXtractSpread( ciXtract *xtract, std::string name );
    
};

// Zcr
class ciXtractZcr : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractZcr( xtract, name ) ); }
    ~ciXtractZcr() {}
    void update();
    
private:
    ciXtractZcr( ciXtract *xtract, std::string name );
    
};

// Rolloff
class ciXtractRolloff : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractRolloff( xtract, name ) ); }
    ~ciXtractRolloff() {}
    void update();
    
private:
    ciXtractRolloff( ciXtract *xtract, std::string name );
    
};

// Loudness
class ciXtractLoudness : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractLoudness( xtract, name ) ); }
    ~ciXtractLoudness() {}
    void update();
    
private:
    ciXtractLoudness( ciXtract *xtract, std::string name );
    
};

// Flatness
class ciXtractFlatness : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractFlatness( xtract, name ) ); }
    ~ciXtractFlatness() {}
    void update();
    
private:
    ciXtractFlatness( ciXtract *xtract, std::string name );
    
};

// Flatness Db
class ciXtractFlatnessDb : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractFlatnessDb( xtract, name ) ); }
    ~ciXtractFlatnessDb() {}
    void update();
    
private:
    ciXtractFlatnessDb( ciXtract *xtract, std::string name );
    
};

// Tonality
class ciXtractTonality : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractTonality( xtract, name ) ); }
    ~ciXtractTonality() {}
    void update();
    
private:
    ciXtractTonality( ciXtract *xtract, std::string name );
    
};

// RMS Amplitude
class ciXtractRmsAmplitude : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractRmsAmplitude( xtract, name ) ); }
    ~ciXtractRmsAmplitude() {}
    void update();
    
private:
    ciXtractRmsAmplitude( ciXtract *xtract, std::string name );
    
};

// Spectral Inharmonicity Amplitude
class ciXtractSpectralInharmonicity : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSpectralInharmonicity( xtract, name ) ); }
    ~ciXtractSpectralInharmonicity() {}
    void update();
    
private:
    ciXtractSpectralInharmonicity( ciXtract *xtract, std::string name );
    
};

// Crest
class ciXtractCrest : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractCrest( xtract, name ) ); }
    ~ciXtractCrest() {}
    void update();
    
private:
    ciXtractCrest( ciXtract *xtract, std::string name );
    
};

// Power
class ciXtractPower : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractPower( xtract, name ) ); }
    ~ciXtractPower() {}
    void update();
    
private:
    ciXtractPower( ciXtract *xtract, std::string name );
    
};

// Odd Even Ratio
class ciXtractOddEvenRatio : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractOddEvenRatio( xtract, name ) ); }
    ~ciXtractOddEvenRatio() {}
    void update();
    
private:
    ciXtractOddEvenRatio( ciXtract *xtract, std::string name );
    
};

// Sharpness
class ciXtractSharpness : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSharpness( xtract, name ) ); }
    ~ciXtractSharpness() {}
    void update();
    
private:
    ciXtractSharpness( ciXtract *xtract, std::string name );
    
};

// Spectral Slope
class ciXtractSpectralSlope : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSpectralSlope( xtract, name ) ); }
    ~ciXtractSpectralSlope() {}
    void update();
    
private:
    ciXtractSpectralSlope( ciXtract *xtract, std::string name );
    
};

// Lowest Value
class ciXtractLowestValue : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractLowestValue( xtract, name ) ); }
    ~ciXtractLowestValue() {}
    void update();
    
private:
    ciXtractLowestValue( ciXtract *xtract, std::string name );
    
};

// Highest Value
class ciXtractHighestValue : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractHighestValue( xtract, name ) ); }
    ~ciXtractHighestValue() {}
    void update();
    
private:
    ciXtractHighestValue( ciXtract *xtract, std::string name );
    
};

// Sum
class ciXtractSum : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSum( xtract, name ) ); }
    ~ciXtractSum() {}
    void update();
    
private:
    ciXtractSum( ciXtract *xtract, std::string name );
    
};

// Non-Zero Count
class ciXtractNonZeroCount : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractNonZeroCount( xtract, name ) ); }
    ~ciXtractNonZeroCount() {}
    void update();
    
private:
    ciXtractNonZeroCount( ciXtract *xtract, std::string name );
    
};

*/


#endif
