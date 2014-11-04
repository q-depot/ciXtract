/*
 *  ciXtractScalarFeature.h
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2014 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef CI_XTRACT_SCALAR_FEATURE
#define CI_XTRACT_SCALAR_FEATURE

#pragma once

#include "ciXtractFeature.h"


// F0
class ciXtractF0 : public ciXtractFeature {
    
public:
    ciXtractF0( ciXtract *xtract, std::string name );
    ~ciXtractF0() {}
    void doUpdate( int frameN );
    
private:
    
};

// Failsafe F0
class ciXtractFailsafeF0 : public ciXtractFeature {
    
public:
    ciXtractFailsafeF0( ciXtract *xtract, std::string name );
    ~ciXtractFailsafeF0() {}
    void doUpdate( int frameN );
    
private:
    
};


// Wavelet F0
class ciXtractWaveletF0 : public ciXtractFeature {
    
public:
    ciXtractWaveletF0( ciXtract *xtract, std::string name );
    ~ciXtractWaveletF0() {}
    void doUpdate( int frameN );
    
private:
};

// Mean
class ciXtractMean : public ciXtractFeature {
    
public:
    ciXtractMean( ciXtract *xtract, std::string name );
    ~ciXtractMean() {}
    void doUpdate( int frameN );
    
private:
    
};

// Variance
class ciXtractVariance : public ciXtractFeature {
    
public:
    ciXtractVariance( ciXtract *xtract, std::string name );
    ~ciXtractVariance() {}
    void doUpdate( int frameN );
    
private:
    
};

// Standard Deviation
class ciXtractStandardDeviation : public ciXtractFeature {
    
public:
    ciXtractStandardDeviation( ciXtract *xtract, std::string name );
    ~ciXtractStandardDeviation() {}
    void doUpdate( int frameN );
    
private:
    
};

// Average Deviation
class ciXtractAverageDeviation : public ciXtractFeature {
    
public:
    ciXtractAverageDeviation( ciXtract *xtract, std::string name );
    ~ciXtractAverageDeviation() {}
    void doUpdate( int frameN );
    
private:
};

// Skewness
class ciXtractSkewness : public ciXtractFeature {
    
public:
    ciXtractSkewness( ciXtract *xtract, std::string name );
    ~ciXtractSkewness() {}
    void doUpdate( int frameN );
    
private:
    
};

// Kurtosis
class ciXtractKurtosis : public ciXtractFeature {
    
public:
    ciXtractKurtosis( ciXtract *xtract, std::string name );
    ~ciXtractKurtosis() {}
    void doUpdate( int frameN );
    
private:
    
};

// Spectral Mean
class ciXtractSpectralMean : public ciXtractFeature {
    
public:
    ciXtractSpectralMean( ciXtract *xtract, std::string name );
    ~ciXtractSpectralMean() {}
    void doUpdate( int frameN );
    
private:
    
};

// Spectral Variance
class ciXtractSpectralVariance : public ciXtractFeature {
    
public:
    ciXtractSpectralVariance( ciXtract *xtract, std::string name );
    ~ciXtractSpectralVariance() {}
    void doUpdate( int frameN );
    
private:
    
};

// Spectral Standard Deviation
class ciXtractSpectralStandardDeviation : public ciXtractFeature {
    
public:
    ciXtractSpectralStandardDeviation( ciXtract *xtract, std::string name );
    ~ciXtractSpectralStandardDeviation() {}
    void doUpdate( int frameN );
    
private:
    
};

// Spectral Skewness
class ciXtractSpectralSkewness : public ciXtractFeature {
    
public:
    ciXtractSpectralSkewness( ciXtract *xtract, std::string name );
    ~ciXtractSpectralSkewness() {}
    void doUpdate( int frameN );
    
private:
    
};

// Spectral Kurtosis
class ciXtractSpectralKurtosis : public ciXtractFeature {
    
public:
    ciXtractSpectralKurtosis( ciXtract *xtract, std::string name );
    ~ciXtractSpectralKurtosis() {}
    void doUpdate( int frameN );
    
private:
    
};

// Spectral Centroid
class ciXtractSpectralCentroid : public ciXtractFeature {
    
public:
    ciXtractSpectralCentroid( ciXtract *xtract, std::string name );
    ~ciXtractSpectralCentroid() {}
    void doUpdate( int frameN );
    
private:
    
};

// Irregularity K
class ciXtractIrregularityK : public ciXtractFeature {
    
public:
    ciXtractIrregularityK( ciXtract *xtract, std::string name );
    ~ciXtractIrregularityK() {}
    void doUpdate( int frameN );
    
private:
    
};

// Irregularity J
class ciXtractIrregularityJ : public ciXtractFeature {
    
public:
    ciXtractIrregularityJ( ciXtract *xtract, std::string name );
    ~ciXtractIrregularityJ() {}
    void doUpdate( int frameN );
    
private:
    
};

// Tristimulus 1
class ciXtractTristimulus1 : public ciXtractFeature {
    
public:
    ciXtractTristimulus1( ciXtract *xtract, std::string name );
    ~ciXtractTristimulus1() {}
    void doUpdate( int frameN );
    
private:
    
};

// Smoothness
class ciXtractSmoothness : public ciXtractFeature {
    
public:
    ciXtractSmoothness( ciXtract *xtract, std::string name );
    ~ciXtractSmoothness() {}
    void doUpdate( int frameN );
    
private:
    
};

// Spread
class ciXtractSpread : public ciXtractFeature {
    
public:
    ciXtractSpread( ciXtract *xtract, std::string name );
    ~ciXtractSpread() {}
    void doUpdate( int frameN );
    
private:
    
};

// Zcr
class ciXtractZcr : public ciXtractFeature {
    
public:
    ciXtractZcr( ciXtract *xtract, std::string name );
    ~ciXtractZcr() {}
    void doUpdate( int frameN );
    
private:
    
};

/*
// Rolloff
class ciXtractRolloff : public ciXtractFeature {
    
public:
    ciXtractRolloff( ciXtract *xtract, std::string name );
    ~ciXtractRolloff() {}
    void doUpdate( int frameN );
    
private:
    
};
*/

// Loudness
class ciXtractLoudness : public ciXtractFeature {
    
public:
    ciXtractLoudness( ciXtract *xtract, std::string name );
    ~ciXtractLoudness() {}
    void doUpdate( int frameN );
    
private:
    
};

// Flatness
class ciXtractFlatness : public ciXtractFeature {
    
public:
    ciXtractFlatness( ciXtract *xtract, std::string name );
    ~ciXtractFlatness() {}
    void doUpdate( int frameN );
    
private:
    
};

// Flatness Db
class ciXtractFlatnessDb : public ciXtractFeature {
    
public:
    ciXtractFlatnessDb( ciXtract *xtract, std::string name );
    ~ciXtractFlatnessDb() {}
    void doUpdate( int frameN );
    
private:
    
};

// Tonality
class ciXtractTonality : public ciXtractFeature {
    
public:
    ciXtractTonality( ciXtract *xtract, std::string name );
    ~ciXtractTonality() {}
    void doUpdate( int frameN );
    
private:
    
};

// RMS Amplitude
class ciXtractRmsAmplitude : public ciXtractFeature {
    
public:
    ciXtractRmsAmplitude( ciXtract *xtract, std::string name );
    ~ciXtractRmsAmplitude() {}
    void doUpdate( int frameN );
    
private:
    
};

// Spectral Inharmonicity Amplitude
class ciXtractSpectralInharmonicity : public ciXtractFeature {
    
public:
    ciXtractSpectralInharmonicity( ciXtract *xtract, std::string name );
    ~ciXtractSpectralInharmonicity() {}
    void doUpdate( int frameN );
    
private:
    
};

// Crest
class ciXtractCrest : public ciXtractFeature {
    
public:
    ciXtractCrest( ciXtract *xtract, std::string name );
    ~ciXtractCrest() {}
    void doUpdate( int frameN );
    
private:
    
};

// Power
class ciXtractPower : public ciXtractFeature {
    
public:
    ciXtractPower( ciXtract *xtract, std::string name );
    ~ciXtractPower() {}
    void doUpdate( int frameN );
    
private:
    
};

// Odd Even Ratio
class ciXtractOddEvenRatio : public ciXtractFeature {
    
public:
    ciXtractOddEvenRatio( ciXtract *xtract, std::string name );
    ~ciXtractOddEvenRatio() {}
    void doUpdate( int frameN );
    
private:
    
};

// Sharpness
class ciXtractSharpness : public ciXtractFeature {
    
public:
    ciXtractSharpness( ciXtract *xtract, std::string name );
    ~ciXtractSharpness() {}
    void doUpdate( int frameN );
    
private:
    
};

// Spectral Slope
class ciXtractSpectralSlope : public ciXtractFeature {
    
public:
    ciXtractSpectralSlope( ciXtract *xtract, std::string name );
    ~ciXtractSpectralSlope() {}
    void doUpdate( int frameN );
    
private:
    
};

// Lowest Value
class ciXtractLowestValue : public ciXtractFeature {
    
public:
    ciXtractLowestValue( ciXtract *xtract, std::string name );
    ~ciXtractLowestValue() {}
    void doUpdate( int frameN );
    
private:
    
};

// Highest Value
class ciXtractHighestValue : public ciXtractFeature {
    
public:
    ciXtractHighestValue( ciXtract *xtract, std::string name );
    ~ciXtractHighestValue() {}
    void doUpdate( int frameN );
    
private:
    
};

// Sum
class ciXtractSum : public ciXtractFeature {
    
public:
    ciXtractSum( ciXtract *xtract, std::string name );
    ~ciXtractSum() {}
    void doUpdate( int frameN );
    
private:
    
};

// Non-Zero Count
class ciXtractNonZeroCount : public ciXtractFeature {
    
public:
    ciXtractNonZeroCount( ciXtract *xtract, std::string name );
    ~ciXtractNonZeroCount() {}
    void doUpdate( int frameN );
    
private:
    
};


#endif
