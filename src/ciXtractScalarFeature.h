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
    void update( int frameN );
};

// Variance
class ciXtractVariance : public ciXtractFeature {
    
public:
    ciXtractVariance( ciXtract *xtract );
    ~ciXtractVariance() {}
    void update( int frameN );
};

// Standard Deviation
class ciXtractStandardDeviation : public ciXtractFeature {
    
public:
    ciXtractStandardDeviation( ciXtract *xtract );
    ~ciXtractStandardDeviation() {}
    void update( int frameN );
};

// Average Deviation
class ciXtractAverageDeviation : public ciXtractFeature {
    
public:
    ciXtractAverageDeviation( ciXtract *xtract );
    ~ciXtractAverageDeviation() {}
    void update( int frameN );
};

// Skewness
class ciXtractSkewness : public ciXtractFeature {
    
public:
    ciXtractSkewness( ciXtract *xtract );
    ~ciXtractSkewness() {}
    void update( int frameN  );
};

// Kurtosis
class ciXtractKurtosis : public ciXtractFeature {
    
public:
    ciXtractKurtosis( ciXtract *xtract );
    ~ciXtractKurtosis() {}
    void update( int frameN  );
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
    void update( int frameN  );
};

// Spectral Standard Deviation
class ciXtractSpectralStandardDeviation : public ciXtractFeature {
    
public:
    ciXtractSpectralStandardDeviation( ciXtract *xtract );
    ~ciXtractSpectralStandardDeviation() {}
    void update( int frameN  );
};

// Spectral Skewness
class ciXtractSpectralSkewness : public ciXtractFeature {
    
public:
    ciXtractSpectralSkewness( ciXtract *xtract );
    ~ciXtractSpectralSkewness() {}
    void update( int frameN  );
};

// Spectral Kurtosis
class ciXtractSpectralKurtosis : public ciXtractFeature {
    
public:
    ciXtractSpectralKurtosis( ciXtract *xtract );
    ~ciXtractSpectralKurtosis() {}
    void update( int frameN  );

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
    void update( int frameN  );
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
    void update( int frameN  );
};

// Zcr
class ciXtractZcr : public ciXtractFeature {
    
public:
    ciXtractZcr( ciXtract *xtract );
    ~ciXtractZcr() {}
};

// Rolloff
//class ciXtractRolloff : public ciXtractFeature {
//    
//public:
//    ciXtractRolloff( ciXtract *xtract );
//    ~ciXtractRolloff() {}
//    void update( int frameN  );
//};

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
    void update( int frameN  );
};

// Rms Amplitude
class ciXtractRmsAmplitude : public ciXtractFeature {
    
public:
    ciXtractRmsAmplitude( ciXtract *xtract );
    ~ciXtractRmsAmplitude() {}
};

// Spectral Inharmonicity
class ciXtractSpectralInharmonicity : public ciXtractFeature {
    
public:
    ciXtractSpectralInharmonicity( ciXtract *xtract );
    ~ciXtractSpectralInharmonicity() {}
    void update( int frameN  );
};

// Crest
class ciXtractCrest : public ciXtractFeature {
    
public:
    ciXtractCrest( ciXtract *xtract );
    ~ciXtractCrest() {}
    void update( int frameN  );
};

// Power
class ciXtractPower : public ciXtractFeature {
    
public:
    ciXtractPower( ciXtract *xtract );
    ~ciXtractPower() {}
};

// Odd Even Ratio
class ciXtractOddEvenRatio : public ciXtractFeature {
    
public:
    ciXtractOddEvenRatio( ciXtract *xtract );
    ~ciXtractOddEvenRatio() {}
    void update( int frameN  );
};

// Sharpness
class ciXtractSharpness : public ciXtractFeature {
    
public:
    ciXtractSharpness( ciXtract *xtract );
    ~ciXtractSharpness() {}
};

// Spectral Slope
class ciXtractSpectralSlope : public ciXtractFeature {
    
public:
    ciXtractSpectralSlope( ciXtract *xtract );
    ~ciXtractSpectralSlope() {}
};

// Lowest Value
class ciXtractLowestValue : public ciXtractFeature {
    
public:
    ciXtractLowestValue( ciXtract *xtract );
    ~ciXtractLowestValue() {}
};

// Highest Value
class ciXtractHighestValue : public ciXtractFeature {
    
public:
    ciXtractHighestValue( ciXtract *xtract );
    ~ciXtractHighestValue() {}
};

// Sum
class ciXtractSum : public ciXtractFeature {
    
public:
    ciXtractSum( ciXtract *xtract );
    ~ciXtractSum() {}
};

// Hps
class ciXtractHps : public ciXtractFeature {
    
public:
    ciXtractHps( ciXtract *xtract );
    ~ciXtractHps() {}
};

// F0
class ciXtractF0 : public ciXtractFeature {
    
public:
    ciXtractF0( ciXtract *xtract );
    ~ciXtractF0() {}
};

// Failsafe F0
class ciXtractFailsafeF0 : public ciXtractFeature {
    
public:
    ciXtractFailsafeF0( ciXtract *xtract );
    ~ciXtractFailsafeF0() {}
};

// Wavelet F0
class ciXtractWaveletF0 : public ciXtractFeature {
    
public:
    ciXtractWaveletF0( ciXtract *xtract );
    ~ciXtractWaveletF0() {}
};

// Midicent     UTILITY <<<<<<<<<<<<<<<<<<
//class ciXtractMidicent : public ciXtractFeature {
//    
//public:
//    ciXtractMidicent( ciXtract *xtract );
//    ~ciXtractMidicent() {}
//};

// Non Zero Count
class ciXtractNonZeroCount : public ciXtractFeature {
    
public:
    ciXtractNonZeroCount( ciXtract *xtract );
    ~ciXtractNonZeroCount() {}
};

// Peak
//class ciXtractPeak : public ciXtractFeature {
//    
//public:
//    ciXtractPeak( ciXtract *xtract );
//    ~ciXtractPeak() {}
//};


#endif
