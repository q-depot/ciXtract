/*
 *  ciXtractScalarFeature.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2014 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */

#include "ciXtractScalarFeature.h"
#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// TODO: mean, varianc ... kurtosis, check whether they process the spectrum or pcm!!!


// Mean ------------------------------------------------------------------------------------------- //
//ciXtractMean::ciXtractMean( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_MEAN, 1, XTRACT_SPECTRUM ) {}
ciXtractMean::ciXtractMean( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_MEAN, 1 ) {}

void ciXtractMean::update( int frameN )
{
    updateWithPcm( frameN );
}


// variance --------------------------------------------------------------------------------------- //
ciXtractVariance::ciXtractVariance( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_VARIANCE, 1, XTRACT_SPECTRUM, vector<xtract_features_>( XTRACT_MEAN ) ) {}

void ciXtractVariance::update( int frameN )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_MEAN )->getValue(0);
    
    doUpdate( frameN, mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
}


// Standard Deviation ----------------------------------------------------------------------------- //
ciXtractStandardDeviation::ciXtractStandardDeviation( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_STANDARD_DEVIATION, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_VARIANCE) ) {}

void ciXtractStandardDeviation::update( int frameN )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_VARIANCE )->getValue(0);
    
    doUpdate( frameN, mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
    
//    updateWithArgdFeatures( frameN, mDependencies );
}


// Average Deviation ------------------------------------------------------------------------------ //
ciXtractAverageDeviation::ciXtractAverageDeviation( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_AVERAGE_DEVIATION, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_MEAN) ) {}

void ciXtractAverageDeviation::update( int frameN )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_MEAN )->getValue(0);
    
    doUpdate( frameN, mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
//    updateWithArgdFeatures( frameN, mDependencies );
}


// Skewness --------------------------------------------------------------------------------------- //
ciXtractSkewness::ciXtractSkewness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SKEWNESS, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_MEAN , XTRACT_STANDARD_DEVIATION) ) {}

void ciXtractSkewness::update( int frameN  )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_MEAN )->getValue(0);
    mArgd[1] = mXtract->getActiveFeature( XTRACT_STANDARD_DEVIATION )->getValue(0);
    
    doUpdate( frameN, mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
}


// Kurtosis --------------------------------------------------------------------------------------- //
ciXtractKurtosis::ciXtractKurtosis( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_KURTOSIS, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_MEAN , XTRACT_STANDARD_DEVIATION) ) {}

void ciXtractKurtosis::update( int frameN  )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_MEAN )->getValue(0);
    mArgd[1] = mXtract->getActiveFeature( XTRACT_STANDARD_DEVIATION )->getValue(0);
    
    doUpdate( frameN, mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
}


// Spectral Mean ---------------------------------------------------------------------------------- //
ciXtractSpectralMean::ciXtractSpectralMean( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_MEAN, 1, XTRACT_SPECTRUM ) {}


// Spectral Variance ------------------------------------------------------------------------------ //
ciXtractSpectralVariance::ciXtractSpectralVariance( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_VARIANCE, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_SPECTRAL_MEAN) ) {}

void ciXtractSpectralVariance::update( int frameN  )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_SPECTRAL_MEAN )->getValue(0);
    
    doUpdate( frameN );
}


// Spectral Standard Deviation -------------------------------------------------------------------- //
ciXtractSpectralStandardDeviation::ciXtractSpectralStandardDeviation( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_STANDARD_DEVIATION, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_SPECTRAL_VARIANCE) ) {}

void ciXtractSpectralStandardDeviation::update( int frameN  )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_SPECTRAL_VARIANCE )->getValue(0);
    
    doUpdate( frameN );
}


// Spectral Skewness ------------------------------------------------------------------------------ //
ciXtractSpectralSkewness::ciXtractSpectralSkewness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_SKEWNESS, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_STANDARD_DEVIATION) ) {}

void ciXtractSpectralSkewness::update( int frameN  )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_SPECTRAL_MEAN )->getValue(0);
    mArgd[1] = mXtract->getActiveFeature( XTRACT_SPECTRAL_STANDARD_DEVIATION )->getValue(0);
    
    doUpdate( frameN );
}


// Spectral Kurtosis ------------------------------------------------------------------------------ //
ciXtractSpectralKurtosis::ciXtractSpectralKurtosis( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_KURTOSIS, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_STANDARD_DEVIATION) ) {}

void ciXtractSpectralKurtosis::update( int frameN  )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_SPECTRAL_MEAN )->getValue(0);
    mArgd[1] = mXtract->getActiveFeature( XTRACT_SPECTRAL_STANDARD_DEVIATION )->getValue(0);
    
    doUpdate( frameN );
}


// Spectral Centroid ------------------------------------------------------------------------------ //
ciXtractSpectralCentroid::ciXtractSpectralCentroid( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_CENTROID, 1, XTRACT_SPECTRUM ) {}


// Irregularity_k --------------------------------------------------------------------------------- //
ciXtractIrregularityK::ciXtractIrregularityK( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_IRREGULARITY_K, 1, XTRACT_SPECTRUM ) {}


// Irregularity_j --------------------------------------------------------------------------------- //
ciXtractIrregularityJ::ciXtractIrregularityJ( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_IRREGULARITY_J, 1, XTRACT_SPECTRUM ) {}


// Tristimulus_1 ---------------------------------------------------------------------------------- //
ciXtractTristimulus1::ciXtractTristimulus1( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_TRISTIMULUS_1, 1, XTRACT_HARMONIC_SPECTRUM, vector<xtract_features_>(XTRACT_F0) ) {}

void ciXtractTristimulus1::update( int frameN  )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_F0 )->getValue(0);
    
    doUpdate( frameN );
}


// Smoothness ------------------------------------------------------------------------------------- //
ciXtractSmoothness::ciXtractSmoothness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SMOOTHNESS, 1, XTRACT_SPECTRUM ) {}


// Spread ----------------------------------------------------------------------------------------- //
ciXtractSpread::ciXtractSpread( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPREAD, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_SPECTRAL_CENTROID) ) {}

void ciXtractSpread::update( int frameN  )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_SPECTRAL_CENTROID )->getValue(0);
    
    doUpdate( frameN );
}


// Zcr -------------------------------------------------------------------------------------------- //
ciXtractZcr::ciXtractZcr( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_ZCR, 1, XTRACT_SPECTRUM ) {}


// Loudness --------------------------------------------------------------------------------------- //
ciXtractLoudness::ciXtractLoudness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_LOUDNESS, 1, XTRACT_BARK_COEFFICIENTS ) {}


// Flatness --------------------------------------------------------------------------------------- //
ciXtractFlatness::ciXtractFlatness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_FLATNESS, 1, XTRACT_SPECTRUM ) {}


// Flatness db ------------------------------------------------------------------------------------ //
ciXtractFlatnessDb::ciXtractFlatnessDb( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_FLATNESS_DB, 1, XTRACT_FLATNESS ) {}


// Tonality --------------------------------------------------------------------------------------- //
ciXtractTonality::ciXtractTonality( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_TONALITY, 1, XTRACT_FLATNESS_DB ) {}

void ciXtractTonality::update( int frameN )
{
    if ( !prepareUpdate(frameN) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_FLATNESS_DB )->getValue(0);
    
    doUpdate( frameN, NULL, 0, mArgd, mDataRaw.get() );
}


// Rms Amplitude ---------------------------------------------------------------------------------- //
ciXtractRmsAmplitude::ciXtractRmsAmplitude( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_RMS_AMPLITUDE, 1, XTRACT_SPECTRUM ) {}


// Spectral Inharmonicity ------------------------------------------------------------------------- //
ciXtractSpectralInharmonicity::ciXtractSpectralInharmonicity( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_INHARMONICITY, 1, XTRACT_PEAK_SPECTRUM, vector<xtract_features_>(XTRACT_F0) ) {}

void ciXtractSpectralInharmonicity::update( int frameN  )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_F0 )->getValue(0);
    
    doUpdate( frameN );
}


// Crest ------------------------------------------------------------------------------------------ //
ciXtractCrest::ciXtractCrest( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_CREST, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_HIGHEST_VALUE, XTRACT_SPECTRAL_MEAN)  ) {}

void ciXtractCrest::update( int frameN  )
{
    if ( !prepareUpdate(frameN) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_HIGHEST_VALUE )->getValue(0);
    mArgd[1] = mXtract->getActiveFeature( XTRACT_SPECTRAL_MEAN )->getValue(0);
    
    doUpdate( frameN, NULL, 0, mArgd, mDataRaw.get() );
}


// Power ------------------------------------------------------------------------------------------ //
ciXtractPower::ciXtractPower( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_POWER, 1, XTRACT_SPECTRUM ) {}


// Odd Even Ratio --------------------------------------------------------------------------------- //
ciXtractOddEvenRatio::ciXtractOddEvenRatio( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_ODD_EVEN_RATIO, 1, XTRACT_HARMONIC_SPECTRUM, vector<xtract_features_>(XTRACT_F0) ) {}

void ciXtractOddEvenRatio::ciXtractOddEvenRatio::update( int frameN  )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    mArgd[0] = mXtract->getActiveFeature( XTRACT_F0 )->getValue(0);
    
    doUpdate( frameN );
}


// Sharpness -------------------------------------------------------------------------------------- //
ciXtractSharpness::ciXtractSharpness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SHARPNESS, 1, XTRACT_SPECTRUM ) {}


// Spectral Slope --------------------------------------------------------------------------------- //
ciXtractSpectralSlope::ciXtractSpectralSlope( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_SLOPE, 1, XTRACT_SPECTRUM ) {}


// Lowest Value ----------------------------------------------------------------------------------- //
ciXtractLowestValue::ciXtractLowestValue( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_LOWEST_VALUE, 1, XTRACT_SPECTRUM )
{
    mParams.push_back( FeatureParam::create( "lower_limit", 0.0 ) );
}


// Highest Value ---------------------------------------------------------------------------------- //
ciXtractHighestValue::ciXtractHighestValue( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_HIGHEST_VALUE, 1, XTRACT_SPECTRUM ) {}


// Sum -------------------------------------------------------------------------------------------- //
ciXtractSum::ciXtractSum( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SUM, 1, XTRACT_SPECTRUM ) {}


// Hps -------------------------------------------------------------------------------------------- //
ciXtractHps::ciXtractHps( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_HPS, 1, XTRACT_SPECTRUM ) {}


// F0 --------------------------------------------------------------------------------------------- //
ciXtractF0::ciXtractF0( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_F0, 1, XTRACT_SPECTRUM )
{
    mParams.push_back( FeatureParam::create( "samplerate", CIXTRACT_SAMPLERATE ) );
}


// Failsafe F0 ------------------------------------------------------------------------------------ //
ciXtractFailsafeF0::ciXtractFailsafeF0( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_FAILSAFE_F0, 1, XTRACT_SPECTRUM )
{
    mParams.push_back( FeatureParam::create( "samplerate", CIXTRACT_SAMPLERATE ) );
}

// Wavelet F0 ------------------------------------------------------------------------------------- //
ciXtractWaveletF0::ciXtractWaveletF0( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_WAVELET_F0, 1, XTRACT_SPECTRUM )
{
    mParams.push_back( FeatureParam::create( "samplerate", CIXTRACT_SAMPLERATE ) );
}

// Non Zero Count --------------------------------------------------------------------------------- //
ciXtractNonZeroCount::ciXtractNonZeroCount( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_NONZERO_COUNT, 1, XTRACT_SPECTRUM ) {}


