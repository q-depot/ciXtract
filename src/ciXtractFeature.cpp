/*
 *  ciXtractFeature.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#include "ciXtractFeature.h"
#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// *************************************** VECTOR FEATURES **************************************** //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //


// Spectrum                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectrum::ciXtractSpectrum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRUM, name, CI_XTRACT_VECTOR, {}, PCM_SIZE, FFT_SIZE )
{
    mData   = mXtract->getPcmData();
    mResult = std::shared_ptr<double>( new double[PCM_SIZE] );
    
    // params
    mParams["dc"]              = { false, CI_XTRACT_PARAM_BOOL  };
    mParams["norm"]            = { false, CI_XTRACT_PARAM_BOOL };
    mParams["type"]            = { XTRACT_MAGNITUDE_SPECTRUM, CI_XTRACT_PARAM_ENUM,
                                 { { "Magnitude", XTRACT_MAGNITUDE_SPECTRUM }, { "Log Magnitude", XTRACT_LOG_MAGNITUDE_SPECTRUM }, { "Power", XTRACT_POWER_SPECTRUM }, { "Log Power", XTRACT_LOG_POWER_SPECTRUM } } };
    
    xtract_init_fft( PCM_SIZE, XTRACT_SPECTRUM );
}

void ciXtractSpectrum::update()
{
    mArgd[0] = SAMPLERATE_N;
    mArgd[1] = mParams["type"].val;
    mArgd[2] = mParams["dc"].val;
    mArgd[3] = mParams["norm"].val;
    
    xtract_spectrum( mData.get(), mDataN, mArgd, mResult.get() );
}

// Autocorrelation                                                                                  //
// ------------------------------------------------------------------------------------------------ //
ciXtractAutocorrelation::ciXtractAutocorrelation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AUTOCORRELATION, name, CI_XTRACT_VECTOR, {}, PCM_SIZE, FFT_SIZE )
{
    mData   = mXtract->getPcmData();
    mResult = std::shared_ptr<double>( new double[PCM_SIZE] );
}

void ciXtractAutocorrelation::update()
{
    xtract_autocorrelation( mData.get(), mDataN, NULL, mResult.get() );
}

// AutoCorrelationFft                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractAutocorrelationFft::ciXtractAutocorrelationFft( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AUTOCORRELATION_FFT, name, CI_XTRACT_VECTOR, {}, PCM_SIZE, FFT_SIZE )
{
    mData   = mXtract->getPcmData();
    mResult = std::shared_ptr<double>( new double[PCM_SIZE] );
    
    xtract_init_fft( PCM_SIZE, XTRACT_AUTOCORRELATION_FFT );
}

void ciXtractAutocorrelationFft::update()
{
    xtract_autocorrelation_fft( mData.get(), mDataN, NULL, mResult.get() );
}

// Harmonic Spectrum                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractHarmonicSpectrum::ciXtractHarmonicSpectrum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_HARMONIC_SPECTRUM, name, CI_XTRACT_VECTOR, { XTRACT_F0 }, PCM_SIZE, FFT_SIZE )
{
    mData                       = mXtract->getPcmData();
    mResult                     = std::shared_ptr<double>( new double[PCM_SIZE] );
    mParams["threshold"]        = { 0.3f, CI_XTRACT_PARAM_DOUBLE };
}

void ciXtractHarmonicSpectrum::update()
{
    mArgd[0] = *mXtract->getFeatureResult(XTRACT_F0).get();
    mArgd[1] = mParams["threshold"].val;
    xtract_harmonic_spectrum( mData.get(), mDataN, mArgd, mResult.get() );
}

// Peak Spectrum                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractPeakSpectrum::ciXtractPeakSpectrum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_PEAK_SPECTRUM, name, CI_XTRACT_VECTOR, { XTRACT_SPECTRUM }, FFT_SIZE, FFT_SIZE )
{
    mData                       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    mResult                     = std::shared_ptr<double>( new double[PCM_SIZE] );
    mParams["threshold"]        = { 0.0f, CI_XTRACT_PARAM_DOUBLE };
}

void ciXtractPeakSpectrum::update()
{
    mArgd[0] = SAMPLERATE_N;
    mArgd[1] = mParams["threshold"].val;
    xtract_peak_spectrum( mData.get(), mDataN, mArgd, mResult.get() );
}

// Bark
// ------------------------------------------------------------------------------------------------ //
ciXtractBark::ciXtractBark( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_BARK_COEFFICIENTS, name, CI_XTRACT_VECTOR, { XTRACT_SPECTRUM }, FFT_SIZE, XTRACT_BARK_BANDS )
{
    mResult                     = std::shared_ptr<double>( new double[ XTRACT_BARK_BANDS ] );
    mBandLimits                 = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );
    mParams["threshold"]        = { 0.0f, CI_XTRACT_PARAM_DOUBLE };
    mData                       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    
    xtract_init_bark( FFT_SIZE, SAMPLERATE >> 1, mBandLimits.get() );
}

void ciXtractBark::update()
{
    mArgd[0]    = SAMPLERATE_N;
    mArgd[1]    = mParams["threshold"].val;
    xtract_bark_coefficients( mData.get(), mDataN, mBandLimits.get(), mResult.get() );
}

// Mfcc
// ------------------------------------------------------------------------------------------------ //
ciXtractMfcc::ciXtractMfcc( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_MFCC, name, CI_XTRACT_VECTOR, { XTRACT_SPECTRUM }, FFT_SIZE, MFCC_FREQ_BANDS )
{
    mData                       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    mResult                     = std::shared_ptr<double>( new double[ MFCC_FREQ_BANDS ] );
    mMelFilters.n_filters       = MFCC_FREQ_BANDS;
    mMelFilters.filters         = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
        mMelFilters.filters[n] = (double *)malloc(PCM_SIZE * sizeof(double));
    
    xtract_init_mfcc( FFT_SIZE, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mMelFilters.n_filters, mMelFilters.filters );
}

ciXtractMfcc::~ciXtractMfcc()
{
    for( auto n = 0; n < MFCC_FREQ_BANDS; ++n )
        free( mMelFilters.filters[n] );
    free( mMelFilters.filters );
}

void ciXtractMfcc::update()
{
    xtract_mfcc( mData.get(), mDataN, &mMelFilters, mResult.get() );
}

// Sub Bands
// ------------------------------------------------------------------------------------------------ //
ciXtractSubBands::ciXtractSubBands( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SUBBANDS, name, CI_XTRACT_VECTOR, { XTRACT_SPECTRUM }, FFT_SIZE, SUBBANDS_N )
{
    mResult                     = std::shared_ptr<double>( new double[ SUBBANDS_N ] );
    mData                       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    mParams["bin_offset"]       = { 0.0f, CI_XTRACT_PARAM_DOUBLE };
    mParams["function"]         = { XTRACT_SUM, CI_XTRACT_PARAM_ENUM, { { "Sum", XTRACT_SUM }, { "Mean", XTRACT_MEAN } } };
    mParams["scale"]            = { XTRACT_OCTAVE_SUBBANDS, CI_XTRACT_PARAM_ENUM, { { "Octave", XTRACT_OCTAVE_SUBBANDS }, { "Linear", XTRACT_LINEAR_SUBBANDS } } };
}

void ciXtractSubBands::update()
{
    int argd[4] = { (int)mParams["function"].val, SUBBANDS_N, (int)mParams["scale"].val, (int)mParams["bin_offset"].val };
    
    xtract_subbands( mData.get(), mDataN, argd, mResult.get() );
}


// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// *************************************** SCALAR FEATURES **************************************** //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //

// F0                                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractF0::ciXtractF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_F0, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, PCM_SIZE, 1 )
{
    mData                   = mXtract->getPcmData();
    mResult                 = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractF0::update()
{
    mArgd[0] = SAMPLERATE;
    xtract_f0( mData.get(), mDataN, mArgd, mResult.get() );
}

// Failsafe F0                                                                                      //
// ------------------------------------------------------------------------------------------------ //

ciXtractFailsafeF0::ciXtractFailsafeF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FAILSAFE_F0, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, PCM_SIZE, 1 )
{
    mData                   = mXtract->getPcmData();
    mResult                 = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractFailsafeF0::update()
{
    mArgd[0] = SAMPLERATE;
    xtract_failsafe_f0( mData.get(), mDataN, mArgd, mResult.get() );
}

// Wavelet F0                                                                                       //
// ------------------------------------------------------------------------------------------------ //
ciXtractWaveletF0::ciXtractWaveletF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_WAVELET_F0, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, PCM_SIZE, 1 )
{
    mData                   = mXtract->getPcmData();
    mResult                 = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractWaveletF0::update()
{
    mArgd[0] = SAMPLERATE;
    xtract_wavelet_f0( mData.get(), mDataN, mArgd, mResult.get() );
}

// Mean                                                                                             //
// ------------------------------------------------------------------------------------------------ //
ciXtractMean::ciXtractMean( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_MEAN, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractMean::update()
{
    xtract_mean( mData.get(), mDataN, NULL, mResult.get() );
}

// Variance                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractVariance::ciXtractVariance( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_VARIANCE, name, CI_XTRACT_SCALAR, { XTRACT_MEAN }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractVariance::update()
{
    xtract_variance( mData.get(), mDataN, mXtract->getFeatureResult(XTRACT_MEAN).get(), mResult.get() );
}

// Standard Deviation                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractStandardDeviation::ciXtractStandardDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_STANDARD_DEVIATION, name, CI_XTRACT_SCALAR, { XTRACT_VARIANCE }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractStandardDeviation::update()
{
    xtract_standard_deviation( mData.get(), mDataN, mXtract->getFeatureResult(XTRACT_VARIANCE).get(), mResult.get() );
}

// Average Deviation                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractAverageDeviation::ciXtractAverageDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AVERAGE_DEVIATION, name, CI_XTRACT_SCALAR, { XTRACT_MEAN }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractAverageDeviation::update()
{
    xtract_average_deviation( mData.get(), mDataN, mXtract->getFeatureResult(XTRACT_MEAN).get(), mResult.get() );
}

// Skewness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractSkewness::ciXtractSkewness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SKEWNESS, name, CI_XTRACT_SCALAR, { XTRACT_STANDARD_DEVIATION }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSkewness::update()
{
    mArgd[0]    = *mXtract->getFeatureResult(XTRACT_MEAN).get();
    mArgd[1]    = *mXtract->getFeatureResult(XTRACT_STANDARD_DEVIATION).get();
    xtract_skewness( mData.get(), mDataN, mArgd, mResult.get() );
}

// Kurtosis                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractKurtosis::ciXtractKurtosis( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_KURTOSIS, name, CI_XTRACT_SCALAR, { XTRACT_STANDARD_DEVIATION }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractKurtosis::update()
{
    mArgd[0]    = *mXtract->getFeatureResult(XTRACT_MEAN).get();
    mArgd[1]    = *mXtract->getFeatureResult(XTRACT_STANDARD_DEVIATION).get();
    xtract_kurtosis( mData.get(), mDataN, mArgd, mResult.get() );
}

// Spectral Mean                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralMean::ciXtractSpectralMean( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_MEAN, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralMean::update()
{
    xtract_spectral_mean( mData.get(), mDataN, NULL, mResult.get() );
}

// Spectral Variance                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralVariance::ciXtractSpectralVariance( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_VARIANCE, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRAL_MEAN }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralVariance::update()
{
    xtract_spectral_variance( mData.get(), mDataN, mXtract->getFeatureResult(XTRACT_SPECTRAL_MEAN).get(), mResult.get() );
}

// Spectral Standard Deviation                                                                      //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralStandardDeviation::ciXtractSpectralStandardDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_STANDARD_DEVIATION, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRAL_VARIANCE }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralStandardDeviation::update()
{
    xtract_spectral_standard_deviation( mData.get(), mDataN, mXtract->getFeatureResult(XTRACT_SPECTRAL_VARIANCE).get(), mResult.get() );
}


// Spectral Skewness                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralSkewness::ciXtractSpectralSkewness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_SKEWNESS, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRAL_MEAN }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralSkewness::update()
{
    xtract_spectral_skewness( mData.get(), mDataN, mXtract->getFeatureResult(XTRACT_SPECTRAL_MEAN).get(), mResult.get() );
}

// Spectral Kurtosis                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralKurtosis::ciXtractSpectralKurtosis( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_KURTOSIS, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_STANDARD_DEVIATION }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralKurtosis::update()
{
    mArgd[0]    = *mXtract->getFeatureResult(XTRACT_SPECTRAL_MEAN).get();
    mArgd[1]    = *mXtract->getFeatureResult(XTRACT_SPECTRAL_STANDARD_DEVIATION).get();
    xtract_spectral_kurtosis( mData.get(), mDataN, mArgd, mResult.get() );
}

// Spectral Centroid                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralCentroid::ciXtractSpectralCentroid( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_CENTROID, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralCentroid::update()
{  
    xtract_spectral_centroid( mData.get(), mDataN, NULL, mResult.get() );
}

// Irregularity K                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractIrregularityK::ciXtractIrregularityK( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_IRREGULARITY_K, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractIrregularityK::update()
{
    xtract_irregularity_k( mData.get(), mDataN, NULL, mResult.get() );
}

// Irregularity J                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractIrregularityJ::ciXtractIrregularityJ( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_IRREGULARITY_J, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractIrregularityJ::update()
{
    xtract_irregularity_j( mData.get(), mDataN, NULL, mResult.get() );
}

// Tristimulus 1                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractTristimulus1::ciXtractTristimulus1( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_TRISTIMULUS_1, name, CI_XTRACT_SCALAR, { XTRACT_HARMONIC_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_HARMONIC_SPECTRUM);
}

void ciXtractTristimulus1::update()
{
    xtract_tristimulus_1( mData.get(), mDataN, NULL, mResult.get() );
}

// Smoothness                                                                                       //
// ------------------------------------------------------------------------------------------------ //
ciXtractSmoothness::ciXtractSmoothness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SMOOTHNESS, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult                 = std::shared_ptr<double>( new double(0.0f) );
    mData                   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    mParams["lower_bound"]  = { 0.0f, CI_XTRACT_PARAM_DOUBLE };
    mParams["upper_bound"]  = { 10.0f, CI_XTRACT_PARAM_DOUBLE  };
    mParams["pre_scaling"]  = { 1.0f, CI_XTRACT_PARAM_DOUBLE  };
}

void ciXtractSmoothness::update()
{
    int argd[3];
    argd[0] = mParams["lower_bound"].val;
    argd[1] = mParams["upper_bound"].val;
    argd[2] = mParams["pre_scaling"].val;
    xtract_smoothness( mData.get(), mDataN, argd, mResult.get() );
}

// Spread                                                                                           //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpread::ciXtractSpread( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPREAD, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRAL_CENTROID }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpread::update()
{
    xtract_spread( mData.get(), mDataN, mXtract->getFeatureResult(XTRACT_SPECTRAL_CENTROID).get(), mResult.get() );
}

// Zcr                                                                                              //
// ------------------------------------------------------------------------------------------------ //
ciXtractZcr::ciXtractZcr( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ZCR, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractZcr::update()
{
    xtract_zcr( mData.get(), mDataN, NULL, mResult.get() );
}

// Rolloff                                                                                          //
// ------------------------------------------------------------------------------------------------ //
ciXtractRolloff::ciXtractRolloff( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ROLLOFF, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult                     = std::shared_ptr<double>( new double(0.0f) );
    mData                       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    mParams["threshold"]        = { 15.0f, CI_XTRACT_PARAM_DOUBLE };
}

void ciXtractRolloff::update()
{
    mArgd[0] = SAMPLERATE_N;
    mArgd[1] = mParams["threshold"].val;
    xtract_rolloff( mData.get(), mDataN, mArgd, mResult.get() );
}

// Loudness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractLoudness::ciXtractLoudness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_LOUDNESS, name, CI_XTRACT_SCALAR, { XTRACT_BARK_COEFFICIENTS }, XTRACT_BARK_BANDS, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_BARK_COEFFICIENTS);
}

void ciXtractLoudness::update()
{
    xtract_loudness( mData.get(), mDataN, NULL, mResult.get() );
}

// Flatness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractFlatness::ciXtractFlatness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FLATNESS, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractFlatness::update()
{
    xtract_flatness( mData.get(), mDataN, NULL, mResult.get() );
}


// Flatness Db                                                                                      //
// ------------------------------------------------------------------------------------------------ //
ciXtractFlatnessDb::ciXtractFlatnessDb( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FLATNESS_DB, name, CI_XTRACT_SCALAR, { XTRACT_FLATNESS }, 0, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractFlatnessDb::update()
{
    xtract_flatness_db( NULL, 0, mXtract->getFeatureResult(XTRACT_FLATNESS).get(), mResult.get() );
}

// Tonality                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractTonality::ciXtractTonality( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_TONALITY, name, CI_XTRACT_SCALAR, { XTRACT_FLATNESS_DB }, 0, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractTonality::update()
{
    xtract_tonality( NULL, 0, mXtract->getFeatureResult(XTRACT_FLATNESS_DB).get(), mResult.get() );
}

// RMS Amplitude                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractRmsAmplitude::ciXtractRmsAmplitude( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_RMS_AMPLITUDE, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractRmsAmplitude::update()
{
    xtract_rms_amplitude( mData.get(), mDataN, NULL, mResult.get() );
}

// Spectral Inhamornicity                                                                           //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralInharmonicity::ciXtractSpectralInharmonicity( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_INHARMONICITY, name, CI_XTRACT_SCALAR, { XTRACT_PEAK_SPECTRUM, XTRACT_F0 }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_PEAK_SPECTRUM);
}

void ciXtractSpectralInharmonicity::update()
{
    xtract_spectral_inharmonicity( mData.get(), mDataN, mXtract->getFeatureResult(XTRACT_F0).get(), mResult.get() );
}

// Crest                                                                                            //
// ------------------------------------------------------------------------------------------------ //
ciXtractCrest::ciXtractCrest( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_INHARMONICITY, name, CI_XTRACT_SCALAR, { XTRACT_PEAK_SPECTRUM, XTRACT_F0 }, NULL, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_PEAK_SPECTRUM);
}

void ciXtractCrest::update()
{
    mArgd[0] = *mXtract->getFeatureResult(XTRACT_HIGHEST_VALUE).get();
    mArgd[1] = *mXtract->getFeatureResult(XTRACT_MEAN).get();
    
    xtract_crest( NULL, NULL, mArgd, mResult.get() );
}

// Power                                                                                            //
// ------------------------------------------------------------------------------------------------ //
ciXtractPower::ciXtractPower( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_POWER, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractPower::update()
{
    xtract_power( mData.get(), mDataN, NULL, mResult.get() );
}

// Odd Even Ratio                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractOddEvenRatio::ciXtractOddEvenRatio( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ODD_EVEN_RATIO, name, CI_XTRACT_SCALAR, { XTRACT_HARMONIC_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_HARMONIC_SPECTRUM);
}

void ciXtractOddEvenRatio::update()
{
    xtract_odd_even_ratio( mData.get(), mDataN, NULL, mResult.get() );
}

// Sharpness                                                                                        //
// ------------------------------------------------------------------------------------------------ //
ciXtractSharpness::ciXtractSharpness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SHARPNESS, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSharpness::update()
{
    xtract_sharpness( mData.get(), mDataN, NULL, mResult.get() );
}

// Spectral Slope                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralSlope::ciXtractSpectralSlope( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_SLOPE, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralSlope::update()
{
    xtract_spectral_slope( mData.get(), mDataN, NULL, mResult.get() );
}

// Lowest Value                                                                                     //
// ------------------------------------------------------------------------------------------------ //
ciXtractLowestValue::ciXtractLowestValue( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_LOWEST_VALUE, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult                 = std::shared_ptr<double>( new double(0.0f) );
    mData                   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    mParams["lower_limit"]  = { 0.2f, CI_XTRACT_PARAM_DOUBLE };
}

void ciXtractLowestValue::update()
{
    xtract_lowest_value( mData.get(), mDataN, &mParams["lower_limit"], mResult.get() );
}

// Highest Value                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractHighestValue::ciXtractHighestValue( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_HIGHEST_VALUE, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractHighestValue::update()
{
    xtract_highest_value( mData.get(), mDataN, NULL, mResult.get() );
}

// Sum                                                                                              //
// ------------------------------------------------------------------------------------------------ //
ciXtractSum::ciXtractSum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SUM, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
    mData   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSum::update()
{
    xtract_sum( mData.get(), mDataN, NULL, mResult.get() );
}

// Non-Zero Count                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractNonZeroCount::ciXtractNonZeroCount( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_NONZERO_COUNT, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM }, FFT_SIZE, 1 )
{
    mResult = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractNonZeroCount::update()
{
    mData = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    xtract_nonzero_count( mData.get(), mDataN, NULL, mResult.get() );
}
