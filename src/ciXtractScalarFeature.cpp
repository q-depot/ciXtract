/*
 *  ciXtractScalarFeature.cpp
 *
 *  Created by Andrea Cuius
 *  The MIT License (MIT)
 *  Copyright (c) 2014 Nocte Studio Ltd.
 *
 *  www.nocte.co.uk
 *
 */

#include "ciXtractScalarFeature.h"
#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


// Mean                                                                                             //
// ------------------------------------------------------------------------------------------------ //
ciXtractMean::ciXtractMean( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_MEAN, name, (xtract_features_)CIXTRACT_PCM_FEATURE ) {}

void ciXtractMean::doUpdate( int frameN )
{
    xtract_mean( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Variance                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractVariance::ciXtractVariance( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_VARIANCE, name, (xtract_features_)CIXTRACT_PCM_FEATURE )
{
	mDependencies.push_back( XTRACT_MEAN );
}

void ciXtractVariance::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_MEAN).get()[0];
    xtract_variance( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Standard Deviation                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractStandardDeviation::ciXtractStandardDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_STANDARD_DEVIATION, name, (xtract_features_)CIXTRACT_PCM_FEATURE )
{
	mDependencies.push_back( XTRACT_VARIANCE );
}

void ciXtractStandardDeviation::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_VARIANCE).get()[0];
    xtract_standard_deviation( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Average Deviation                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractAverageDeviation::ciXtractAverageDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AVERAGE_DEVIATION, name, (xtract_features_)CIXTRACT_PCM_FEATURE )
{
	mDependencies.push_back( XTRACT_MEAN );
}

void ciXtractAverageDeviation::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_MEAN).get()[0];
    xtract_average_deviation( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Skewness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractSkewness::ciXtractSkewness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SKEWNESS, name, (xtract_features_)CIXTRACT_PCM_FEATURE )
{
	mDependencies.push_back( XTRACT_MEAN );
	mDependencies.push_back( XTRACT_STANDARD_DEVIATION );
}

void ciXtractSkewness::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_MEAN).get()[0];
    mArgd[1] = mXtract->getFeatureDataRaw(XTRACT_STANDARD_DEVIATION).get()[0];
    
    xtract_skewness( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Kurtosis                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractKurtosis::ciXtractKurtosis( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_KURTOSIS, name, (xtract_features_)CIXTRACT_PCM_FEATURE )
{
	mDependencies.push_back( XTRACT_MEAN );
	mDependencies.push_back( XTRACT_STANDARD_DEVIATION );
}

void ciXtractKurtosis::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_MEAN).get()[0];
    mArgd[1] = mXtract->getFeatureDataRaw(XTRACT_STANDARD_DEVIATION).get()[0];
    
    xtract_kurtosis( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Spectral Mean                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralMean::ciXtractSpectralMean( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_MEAN, name, XTRACT_SPECTRUM ) {}

void ciXtractSpectralMean::doUpdate( int frameN )
{
    xtract_spectral_mean( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Spectral Variance                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralVariance::ciXtractSpectralVariance( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_VARIANCE, name, XTRACT_SPECTRUM )
{
	mDependencies.push_back( XTRACT_SPECTRAL_MEAN );
}

void ciXtractSpectralVariance::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_SPECTRAL_MEAN).get()[0];
    
    xtract_spectral_variance( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Spectral Standard Deviation                                                                      //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralStandardDeviation::ciXtractSpectralStandardDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_STANDARD_DEVIATION, name, XTRACT_SPECTRUM )
{
	mDependencies.push_back( XTRACT_SPECTRAL_VARIANCE );
}

void ciXtractSpectralStandardDeviation::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_SPECTRAL_VARIANCE).get()[0];
    
    xtract_spectral_standard_deviation( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Spectral Skewness                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralSkewness::ciXtractSpectralSkewness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_SKEWNESS, name, XTRACT_SPECTRUM )
{
	mDependencies.push_back( XTRACT_SPECTRAL_MEAN );
	mDependencies.push_back( XTRACT_SPECTRAL_STANDARD_DEVIATION );
}

void ciXtractSpectralSkewness::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_SPECTRAL_MEAN).get()[0];
    mArgd[1] = mXtract->getFeatureDataRaw(XTRACT_SPECTRAL_STANDARD_DEVIATION).get()[0];
    
    xtract_spectral_skewness( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Spectral Kurtosis                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralKurtosis::ciXtractSpectralKurtosis( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_KURTOSIS, name, XTRACT_SPECTRUM )
{
	mDependencies.push_back( XTRACT_SPECTRAL_MEAN );
	mDependencies.push_back( XTRACT_SPECTRAL_STANDARD_DEVIATION );
}

void ciXtractSpectralKurtosis::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_SPECTRAL_MEAN).get()[0];
    mArgd[1] = mXtract->getFeatureDataRaw(XTRACT_SPECTRAL_STANDARD_DEVIATION).get()[0];
    
    xtract_spectral_kurtosis( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Spectral Centroid                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralCentroid::ciXtractSpectralCentroid( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_CENTROID, name, XTRACT_SPECTRUM ) {}

void ciXtractSpectralCentroid::doUpdate( int frameN )
{
    xtract_spectral_centroid( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Irregularity K                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractIrregularityK::ciXtractIrregularityK( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_IRREGULARITY_K, name, XTRACT_SPECTRUM ) {}

void ciXtractIrregularityK::doUpdate( int frameN )
{
    xtract_irregularity_k( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Irregularity J                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractIrregularityJ::ciXtractIrregularityJ( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_IRREGULARITY_J, name, XTRACT_SPECTRUM ) {}

void ciXtractIrregularityJ::doUpdate( int frameN )
{
    xtract_irregularity_j( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Tristimulus 1                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractTristimulus1::ciXtractTristimulus1( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_TRISTIMULUS_1, name, XTRACT_HARMONIC_SPECTRUM )
{
    mDependencies.push_back( XTRACT_F0 );
}

void ciXtractTristimulus1::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw( XTRACT_F0 ).get()[0];
    xtract_tristimulus_1( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Smoothness                                                                                       //
// ------------------------------------------------------------------------------------------------ //
ciXtractSmoothness::ciXtractSmoothness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SMOOTHNESS, name, XTRACT_SPECTRUM ) {}

void ciXtractSmoothness::doUpdate( int frameN )
{
    xtract_smoothness( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
    
}


// Spread                                                                                           //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpread::ciXtractSpread( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPREAD, name, XTRACT_SPECTRUM )
{
	mDependencies.push_back( XTRACT_SPECTRAL_CENTROID );
}

void ciXtractSpread::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw( XTRACT_SPECTRAL_CENTROID ).get()[0];
    xtract_spread( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Zcr                                                                                              //
// ------------------------------------------------------------------------------------------------ //
ciXtractZcr::ciXtractZcr( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ZCR, name, XTRACT_SPECTRUM ) {}

void ciXtractZcr::doUpdate( int frameN )
{
    xtract_zcr( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


/*
// Rolloff                                                                                          //
// ------------------------------------------------------------------------------------------------ //
ciXtractRolloff::ciXtractRolloff( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ROLLOFF, name, XTRACT_SPECTRUM )
{
    mParams["threshold"]    = ciXtractFeature::createFeatureParam( 15.0f, CI_XTRACT_PARAM_DOUBLE, std::map<std::string,double>() );
}

void ciXtractRolloff::doUpdate( int frameN )
{
    mArgd[0] = CIXTRACT_SAMPLERATE_N;
    mArgd[1] = mParams["threshold"].val;

    xtract_rolloff( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
    
}
*/


// Loudness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractLoudness::ciXtractLoudness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_LOUDNESS, name, XTRACT_BARK_COEFFICIENTS ) {}

void ciXtractLoudness::doUpdate( int frameN )
{
    xtract_loudness( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Flatness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractFlatness::ciXtractFlatness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FLATNESS, name, XTRACT_SPECTRUM ) {}                              // can also be computed with BARKS

void ciXtractFlatness::doUpdate( int frameN )
{
    xtract_flatness( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Flatness Db                                                                                      //
// ------------------------------------------------------------------------------------------------ //
ciXtractFlatnessDb::ciXtractFlatnessDb( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FLATNESS_DB, name, (xtract_features_)CIXTRACT_NO_FEATURE )
{
    mDependencies.push_back( XTRACT_FLATNESS );
}

void ciXtractFlatnessDb::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_FLATNESS).get()[0];
    xtract_flatness_db( NULL, 0, mArgd, mDataRaw.get() );
}


// Tonality                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractTonality::ciXtractTonality( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_TONALITY, name, (xtract_features_)CIXTRACT_NO_FEATURE )
{
	mDependencies.push_back( XTRACT_FLATNESS_DB );
}

void ciXtractTonality::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw( XTRACT_FLATNESS_DB ).get()[0];

    xtract_tonality( NULL, 0, mArgd, mDataRaw.get() );
}


// Noisiness                                                                                        //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //


// RMS Amplitude                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractRmsAmplitude::ciXtractRmsAmplitude( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_RMS_AMPLITUDE, name, XTRACT_SPECTRUM ) {}

void ciXtractRmsAmplitude::doUpdate( int frameN )
{
    xtract_rms_amplitude( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Spectral Inhamornicity                                                                           //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralInharmonicity::ciXtractSpectralInharmonicity( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_INHARMONICITY, name, XTRACT_PEAK_SPECTRUM )
{
	mDependencies.push_back( XTRACT_F0 );
}

void ciXtractSpectralInharmonicity::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw( XTRACT_F0 ).get()[0];

    xtract_spectral_inharmonicity( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Crest                                                                                            //
// ------------------------------------------------------------------------------------------------ //
ciXtractCrest::ciXtractCrest( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_CREST, name, (xtract_features_)CIXTRACT_NO_FEATURE )
{
	mDependencies.push_back( XTRACT_HIGHEST_VALUE );
	mDependencies.push_back( XTRACT_MEAN );
}

void ciXtractCrest::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_HIGHEST_VALUE).get()[0];
    mArgd[1] = mXtract->getFeatureDataRaw(XTRACT_MEAN).get()[0];
    
    xtract_crest( NULL, 0, mArgd, mDataRaw.get() );
}


// Power                                                                                            //
// ------------------------------------------------------------------------------------------------ //
ciXtractPower::ciXtractPower( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_POWER, name, XTRACT_SPECTRUM ) {}

void ciXtractPower::doUpdate( int frameN )
{
    xtract_power( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Odd Even Ratio                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractOddEvenRatio::ciXtractOddEvenRatio( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ODD_EVEN_RATIO, name, XTRACT_HARMONIC_SPECTRUM )
{
    mDependencies.push_back( XTRACT_F0 );
}


void ciXtractOddEvenRatio::doUpdate( int frameN )
{
    mArgd[0] = mXtract->getFeatureDataRaw(XTRACT_F0).get()[0];
    xtract_odd_even_ratio( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Sharpness                                                                                        //
// ------------------------------------------------------------------------------------------------ //
ciXtractSharpness::ciXtractSharpness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SHARPNESS, name, XTRACT_SPECTRUM ) {}

void ciXtractSharpness::doUpdate( int frameN )
{
    xtract_sharpness( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Spectral Slope                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralSlope::ciXtractSpectralSlope( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_SLOPE, name, XTRACT_SPECTRUM ) {}

void ciXtractSpectralSlope::doUpdate( int frameN )
{
    xtract_spectral_slope( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Lowest Value                                                                                     //
// ------------------------------------------------------------------------------------------------ //
ciXtractLowestValue::ciXtractLowestValue( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_LOWEST_VALUE, name, XTRACT_SPECTRUM )
{
    mParams["lower_limit"]  = ciXtractFeature::createFeatureParam( 0.2f, CI_XTRACT_PARAM_DOUBLE, std::map<std::string,double>() );
}

void ciXtractLowestValue::doUpdate( int frameN )
{
    mArgd[0] = mParams["lower_limit"].val;
    
    xtract_lowest_value( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Highest Value                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractHighestValue::ciXtractHighestValue( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_HIGHEST_VALUE, name, XTRACT_SPECTRUM ) {}

void ciXtractHighestValue::doUpdate( int frameN )
{
    xtract_highest_value( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Sum                                                                                              //
// ------------------------------------------------------------------------------------------------ //
ciXtractSum::ciXtractSum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SUM, name, XTRACT_SPECTRUM ) {}

void ciXtractSum::doUpdate( int frameN )
{
    xtract_sum( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Hps                                                                                              //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //


// F0                                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractF0::ciXtractF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_F0, name, (xtract_features_)CIXTRACT_PCM_FEATURE ) {}

void ciXtractF0::doUpdate( int frameN )
{
    mArgd[0] = CIXTRACT_SAMPLERATE;
    xtract_f0( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Failsafe F0                                                                                      //
// ------------------------------------------------------------------------------------------------ //

ciXtractFailsafeF0::ciXtractFailsafeF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FAILSAFE_F0, name, (xtract_features_)CIXTRACT_PCM_FEATURE ) {}

void ciXtractFailsafeF0::doUpdate( int frameN )
{
    mArgd[0] = CIXTRACT_SAMPLERATE;
    xtract_failsafe_f0( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Wavelet F0                                                                                       //
// ------------------------------------------------------------------------------------------------ //
ciXtractWaveletF0::ciXtractWaveletF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_WAVELET_F0, name, (xtract_features_)CIXTRACT_PCM_FEATURE ) {}

void ciXtractWaveletF0::enable( bool isEnable )
{
    if ( !mIsInit )
        xtract_init_wavelet_f0_state();
    
    ciXtractFeature::enable(isEnable);
}

void ciXtractWaveletF0::doUpdate( int frameN )
{
    mArgd[0] = CIXTRACT_SAMPLERATE;
    xtract_wavelet_f0( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Non-Zero Count                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractNonZeroCount::ciXtractNonZeroCount( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_NONZERO_COUNT, name, XTRACT_SPECTRUM ) {}

void ciXtractNonZeroCount::doUpdate( int frameN )
{
    xtract_nonzero_count( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}

