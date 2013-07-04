/*
 *  ciXtract.h
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

#include "libxtract.h"

class ciXtractFeature;
typedef std::shared_ptr<ciXtractFeature>       ciXtractFeatureRef;

class ciXtractSpectrum;

class ciXtract;


enum ciXtractFeatureType {
    CI_XTRACT_VECTOR,
    CI_XTRACT_SCALAR
};


#define PCM_SIZE            2048
#define FFT_SIZE            512
#define SAMPLERATE          44100

#define PERIOD              100
#define MFCC_FREQ_BANDS     13
#define MFCC_FREQ_MIN       20
#define MFCC_FREQ_MAX       20000
#define SUBBANDS_N          32



class ciXtractFeature {

public:
    
    friend class ciXtract;
    
    virtual void update() {}
    
    bool isEnable() { return mIsEnable; }
    
    xtract_features_ getEnum() { return mFeature; }
    
    double getResultMin() { return mResultMin; }
    double getResultMax() { return mResultMax; }
    
    ciXtractFeatureType getType() { return mType; }
    
    std::string getName() { return mName; }
    
    std::shared_ptr<double>         getResult() { return mResult; }
    uint32_t                        getResultN() { return mResultN; }
    
    void resetCalibration()
    {    
        mResultMin = std::numeric_limits<double>::max();
        mResultMax = std::numeric_limits<double>::min();
    }
    
    void calibrate()
    {
        if ( !mIsEnable )
            return;

        double val;
        
            
        for( auto k=0; k < mResultN; k++ )
        {
            val = mResult.get()[k];

            /// I should also check for infinite values, they should be ignored <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
            if ( isnan(val) )
                continue;
            
//            if ( mFeature == XTRACT_SPECTRUM )
//                ci::app::console() << val << " " << mMaxResult << " " <<  std::endl;
            
            if ( val > mResultMax )
                mResultMax = val;
            
            if ( val < mResultMin )
                mResultMin = val;
        }
        
        if ( mResultMin == mResultMax )
            mResultMax += 0.001f;
    }
    
    
protected:
    
    ciXtractFeature( ciXtract *xtract, xtract_features_ feature, std::string name, ciXtractFeatureType type, std::vector<xtract_features_> dependencies, uint32_t dataN, uint32_t resultN )
    {
        mXtract         = xtract;
        mFeature        = feature;
        mName           = name;
        mType           = type;
        mDependencies   = dependencies;
        mResultN        = resultN;
        mDataN          = dataN;
        // each feature implementation define its own mData mResult and mParams
        mResultMin      = 0.0f;
        mResultMax      = 1.0f;
        mIsEnable       = true;
    }

    
protected:
    
    ciXtract                        *mXtract;
    xtract_features_                mFeature;
    std::string                     mName;
    ciXtractFeatureType             mType;
    std::vector<xtract_features_>   mDependencies;
    
    std::shared_ptr<double>         mData;
    uint32_t                        mDataN;
    
    std::shared_ptr<double>         mResult;
    uint32_t                        mResultN;
    
    double                          mResultMin;
    double                          mResultMax;
    
    bool                            mIsEnable;
    std::map<std::string,double>    mParams;
    double                          mArgd[4];
};


// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// *************************************** VECTOR FEATURES **************************************** //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //

// Spectrum
class ciXtractSpectrum : public ciXtractFeature {
    
public:
    static ciXtractFeatureRef create( ciXtract *xtract, std::string name ) { return ciXtractFeatureRef( new ciXtractSpectrum( xtract, name ) ); }
    ~ciXtractSpectrum() {}
    void update();
    
private:
    ciXtractSpectrum( ciXtract *xtract, std::string name );
    
};

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
    ~ciXtractMfcc() {}
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


#endif
