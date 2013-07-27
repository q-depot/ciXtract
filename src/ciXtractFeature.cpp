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
: ciXtractFeature( xtract, XTRACT_SPECTRUM, name, CI_XTRACT_VECTOR, {}, FFT_SIZE )
{
    mEnumStr    = "XTRACT_SPECTRUM";
    mData       = mXtract->getPcmData();
    mResult     = std::shared_ptr<double>( new double[PCM_SIZE] );
    
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
    
    xtract_spectrum( mData.get(), PCM_SIZE, mArgd, mResult.get() );
}

// Autocorrelation                                                                                  //
// ------------------------------------------------------------------------------------------------ //
ciXtractAutocorrelation::ciXtractAutocorrelation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AUTOCORRELATION, name, CI_XTRACT_VECTOR, {}, FFT_SIZE )
{
    mEnumStr    = "XTRACT_AUTOCORRELATION";
    mData       = mXtract->getPcmData();
    mResult     = std::shared_ptr<double>( new double[PCM_SIZE] );
}

void ciXtractAutocorrelation::update()
{
    xtract_autocorrelation( mData.get(), PCM_SIZE, NULL, mResult.get() );
}

// AutoCorrelationFft                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractAutocorrelationFft::ciXtractAutocorrelationFft( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AUTOCORRELATION_FFT, name, CI_XTRACT_VECTOR, {}, FFT_SIZE )
{
    mEnumStr    = "XTRACT_AUTOCORRELATION_FFT";
    mData       = mXtract->getPcmData();
    mResult     = std::shared_ptr<double>( new double[PCM_SIZE] );
    
    xtract_init_fft( PCM_SIZE, XTRACT_AUTOCORRELATION_FFT );
}

void ciXtractAutocorrelationFft::update()
{
    xtract_autocorrelation_fft( mData.get(), PCM_SIZE, NULL, mResult.get() );
}

// Harmonic Spectrum                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractHarmonicSpectrum::ciXtractHarmonicSpectrum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_HARMONIC_SPECTRUM, name, CI_XTRACT_VECTOR, { XTRACT_F0 }, FFT_SIZE )
{
    mEnumStr                    = "XTRACT_HARMONIC_SPECTRUM";
    mData                       = mXtract->getPcmData();
    mResult                     = std::shared_ptr<double>( new double[PCM_SIZE] );
    mParams["threshold"]        = { 0.3f, CI_XTRACT_PARAM_DOUBLE };
}

void ciXtractHarmonicSpectrum::update()
{
    mArgd[0] = *mXtract->getFeatureResult(XTRACT_F0).get();
    mArgd[1] = mParams["threshold"].val;
    xtract_harmonic_spectrum( mData.get(), PCM_SIZE, mArgd, mResult.get() );
}

// Peak Spectrum                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractPeakSpectrum::ciXtractPeakSpectrum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_PEAK_SPECTRUM, name, CI_XTRACT_VECTOR, { XTRACT_SPECTRUM }, FFT_SIZE )
{
    mEnumStr                    = "XTRACT_PEAK_SPECTRUM";
    mData                       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    mResult                     = std::shared_ptr<double>( new double[PCM_SIZE] );
    mParams["threshold"]        = { 0.0f, CI_XTRACT_PARAM_DOUBLE };
}

void ciXtractPeakSpectrum::update()
{
    mArgd[0] = SAMPLERATE_N;
    mArgd[1] = mParams["threshold"].val;
    xtract_peak_spectrum( mData.get(), FFT_SIZE, mArgd, mResult.get() );
}

// Bark
// ------------------------------------------------------------------------------------------------ //
ciXtractBark::ciXtractBark( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_BARK_COEFFICIENTS, name, CI_XTRACT_VECTOR, { XTRACT_SPECTRUM }, XTRACT_BARK_BANDS )
{
    mEnumStr                    = "XTRACT_BARK_COEFFICIENTS";
    mData                       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    mResult                     = std::shared_ptr<double>( new double[ XTRACT_BARK_BANDS ] );
    mBandLimits                 = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );
    mParams["threshold"]        = { 0.0f, CI_XTRACT_PARAM_DOUBLE };
    
    xtract_init_bark( FFT_SIZE, SAMPLERATE >> 1, mBandLimits.get() );
}

void ciXtractBark::update()
{
    mArgd[0]    = SAMPLERATE_N;
    mArgd[1]    = mParams["threshold"].val;
    xtract_bark_coefficients( mData.get(), FFT_SIZE, mBandLimits.get(), mResult.get() );
}

// Mfcc
// ------------------------------------------------------------------------------------------------ //
ciXtractMfcc::ciXtractMfcc( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_MFCC, name, CI_XTRACT_VECTOR, { XTRACT_SPECTRUM }, MFCC_FREQ_BANDS )
{
    mEnumStr                    = "XTRACT_MFCC";
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
    xtract_mfcc( mData.get(), FFT_SIZE, &mMelFilters, mResult.get() );
}

// Sub Bands
// ------------------------------------------------------------------------------------------------ //
ciXtractSubBands::ciXtractSubBands( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SUBBANDS, name, CI_XTRACT_VECTOR, { XTRACT_SPECTRUM }, SUBBANDS_N )
{
    mEnumStr                    = "XTRACT_SUBBANDS";
    mResult                     = std::shared_ptr<double>( new double[ SUBBANDS_N ] );
    mData                       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    mParams["bin_offset"]       = { 0.0f, CI_XTRACT_PARAM_DOUBLE };
    mParams["function"]         = { XTRACT_SUM, CI_XTRACT_PARAM_ENUM, { { "Sum", XTRACT_SUM }, { "Mean", XTRACT_MEAN } } };
    mParams["scale"]            = { XTRACT_LINEAR_SUBBANDS, CI_XTRACT_PARAM_ENUM, { { "Octave", XTRACT_OCTAVE_SUBBANDS }, { "Linear", XTRACT_LINEAR_SUBBANDS } } };
}

void ciXtractSubBands::update()
{
    int argd[4] = { (int)mParams["function"].val, SUBBANDS_N, (int)mParams["scale"].val, (int)mParams["bin_offset"].val };
    
    xtract_subbands( mData.get(), FFT_SIZE, argd, mResult.get() );
}


// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// *************************************** SCALAR FEATURES **************************************** //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //

// F0                                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractF0::ciXtractF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_F0, name, CI_XTRACT_SCALAR, { } )
{
    mEnumStr                = "XTRACT_F0";
    mData                   = mXtract->getPcmData();
    mResult                 = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractF0::update()
{
    mArgd[0] = SAMPLERATE;
    xtract_f0( mData.get(), PCM_SIZE, mArgd, mResult.get() );
}

// Failsafe F0                                                                                      //
// ------------------------------------------------------------------------------------------------ //

ciXtractFailsafeF0::ciXtractFailsafeF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FAILSAFE_F0, name, CI_XTRACT_SCALAR, { } )
{
    mEnumStr                = "XTRACT_FAILSAFE_F0";
    mData                   = mXtract->getPcmData();
    mResult                 = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractFailsafeF0::update()
{
    mArgd[0] = SAMPLERATE;
    xtract_failsafe_f0( mData.get(), PCM_SIZE, mArgd, mResult.get() );
}

// Wavelet F0                                                                                       //
// ------------------------------------------------------------------------------------------------ //
ciXtractWaveletF0::ciXtractWaveletF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_WAVELET_F0, name, CI_XTRACT_SCALAR, { } )
{
    mEnumStr                = "XTRACT_WAVELET_F0";
    mData                   = mXtract->getPcmData();
    mResult                 = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractWaveletF0::update()
{
    mArgd[0] = SAMPLERATE;
    xtract_wavelet_f0( mData.get(), PCM_SIZE, mArgd, mResult.get() );
}

// Mean                                                                                             //
// ------------------------------------------------------------------------------------------------ //
ciXtractMean::ciXtractMean( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_MEAN, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_MEAN";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractMean::update()
{
    xtract_mean( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Variance                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractVariance::ciXtractVariance( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_VARIANCE, name, CI_XTRACT_SCALAR, { XTRACT_MEAN } )
{
    mEnumStr    = "XTRACT_VARIANCE";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractVariance::update()
{
    xtract_variance( mData.get(), FFT_SIZE, mXtract->getFeatureResult(XTRACT_MEAN).get(), mResult.get() );
}

// Standard Deviation                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractStandardDeviation::ciXtractStandardDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_STANDARD_DEVIATION, name, CI_XTRACT_SCALAR, { XTRACT_VARIANCE } )
{
    mEnumStr    = "XTRACT_STANDARD_DEVIATION";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractStandardDeviation::update()
{
    xtract_standard_deviation( mData.get(), FFT_SIZE, mXtract->getFeatureResult(XTRACT_VARIANCE).get(), mResult.get() );
}

// Average Deviation                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractAverageDeviation::ciXtractAverageDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AVERAGE_DEVIATION, name, CI_XTRACT_SCALAR, { XTRACT_MEAN } )
{
    mEnumStr    = "XTRACT_AVERAGE_DEVIATION";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractAverageDeviation::update()
{
    xtract_average_deviation( mData.get(), FFT_SIZE, mXtract->getFeatureResult(XTRACT_MEAN).get(), mResult.get() );
}

// Skewness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractSkewness::ciXtractSkewness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SKEWNESS, name, CI_XTRACT_SCALAR, { XTRACT_STANDARD_DEVIATION } )
{
    mEnumStr    = "XTRACT_SKEWNESS";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSkewness::update()
{
    mArgd[0]    = *mXtract->getFeatureResult(XTRACT_MEAN).get();
    mArgd[1]    = *mXtract->getFeatureResult(XTRACT_STANDARD_DEVIATION).get();
    xtract_skewness( mData.get(), FFT_SIZE, mArgd, mResult.get() );
}

// Kurtosis                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractKurtosis::ciXtractKurtosis( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_KURTOSIS, name, CI_XTRACT_SCALAR, { XTRACT_STANDARD_DEVIATION } )
{
    mEnumStr    = "XTRACT_KURTOSIS";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractKurtosis::update()
{
    mArgd[0]    = *mXtract->getFeatureResult(XTRACT_MEAN).get();
    mArgd[1]    = *mXtract->getFeatureResult(XTRACT_STANDARD_DEVIATION).get();
    xtract_kurtosis( mData.get(), FFT_SIZE, mArgd, mResult.get() );
}

// Spectral Mean                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralMean::ciXtractSpectralMean( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_MEAN, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_SPECTRAL_MEAN";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralMean::update()
{
    xtract_spectral_mean( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Spectral Variance                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralVariance::ciXtractSpectralVariance( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_VARIANCE, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRAL_MEAN } )
{
    mEnumStr    = "XTRACT_SPECTRAL_VARIANCE";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralVariance::update()
{
    xtract_spectral_variance( mData.get(), FFT_SIZE, mXtract->getFeatureResult(XTRACT_SPECTRAL_MEAN).get(), mResult.get() );
}

// Spectral Standard Deviation                                                                      //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralStandardDeviation::ciXtractSpectralStandardDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_STANDARD_DEVIATION, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRAL_VARIANCE } )
{
    mEnumStr    = "XTRACT_SPECTRAL_STANDARD_DEVIATION";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralStandardDeviation::update()
{
    xtract_spectral_standard_deviation( mData.get(), FFT_SIZE, mXtract->getFeatureResult(XTRACT_SPECTRAL_VARIANCE).get(), mResult.get() );
}

// Spectral Skewness                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralSkewness::ciXtractSpectralSkewness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_SKEWNESS, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRAL_MEAN } )
{
    mEnumStr    = "XTRACT_SPECTRAL_SKEWNESS";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralSkewness::update()
{
    xtract_spectral_skewness( mData.get(), FFT_SIZE, mXtract->getFeatureResult(XTRACT_SPECTRAL_MEAN).get(), mResult.get() );
}

// Spectral Kurtosis                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralKurtosis::ciXtractSpectralKurtosis( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_KURTOSIS, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_STANDARD_DEVIATION } )
{
    mEnumStr    = "XTRACT_SPECTRAL_KURTOSIS";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralKurtosis::update()
{
    mArgd[0]    = *mXtract->getFeatureResult(XTRACT_SPECTRAL_MEAN).get();
    mArgd[1]    = *mXtract->getFeatureResult(XTRACT_SPECTRAL_STANDARD_DEVIATION).get();
    xtract_spectral_kurtosis( mData.get(), FFT_SIZE, mArgd, mResult.get() );
}

// Spectral Centroid                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralCentroid::ciXtractSpectralCentroid( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_CENTROID, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_SPECTRAL_CENTROID";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralCentroid::update()
{  
    xtract_spectral_centroid( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Irregularity K                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractIrregularityK::ciXtractIrregularityK( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_IRREGULARITY_K, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_IRREGULARITY_K";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractIrregularityK::update()
{
    xtract_irregularity_k( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Irregularity J                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractIrregularityJ::ciXtractIrregularityJ( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_IRREGULARITY_J, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_IRREGULARITY_J";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractIrregularityJ::update()
{
    xtract_irregularity_j( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Tristimulus 1                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractTristimulus1::ciXtractTristimulus1( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_TRISTIMULUS_1, name, CI_XTRACT_SCALAR, { XTRACT_HARMONIC_SPECTRUM } )
{
    mEnumStr    = "XTRACT_TRISTIMULUS_1";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_HARMONIC_SPECTRUM);
}

void ciXtractTristimulus1::update()
{
    xtract_tristimulus_1( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Smoothness                                                                                       //
// ------------------------------------------------------------------------------------------------ //
ciXtractSmoothness::ciXtractSmoothness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SMOOTHNESS, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_SMOOTHNESS";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSmoothness::update()
{
    xtract_smoothness( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Spread                                                                                           //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpread::ciXtractSpread( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPREAD, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRAL_CENTROID } )
{
    mEnumStr    = "XTRACT_SPREAD";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpread::update()
{
    xtract_spread( mData.get(), FFT_SIZE, mXtract->getFeatureResult(XTRACT_SPECTRAL_CENTROID).get(), mResult.get() );
}

// Zcr                                                                                              //
// ------------------------------------------------------------------------------------------------ //
ciXtractZcr::ciXtractZcr( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ZCR, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_ZCR";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractZcr::update()
{
    xtract_zcr( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Rolloff                                                                                          //
// ------------------------------------------------------------------------------------------------ //
ciXtractRolloff::ciXtractRolloff( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ROLLOFF, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr                = "XTRACT_ROLLOFF";
    mResult                 = std::shared_ptr<double>( new double(0.0f) );
    mData                   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    mParams["threshold"]    = { 15.0f, CI_XTRACT_PARAM_DOUBLE };
}

void ciXtractRolloff::update()
{
    mArgd[0] = SAMPLERATE_N;
    mArgd[1] = mParams["threshold"].val;
    xtract_rolloff( mData.get(), FFT_SIZE, mArgd, mResult.get() );
}

// Loudness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractLoudness::ciXtractLoudness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_LOUDNESS, name, CI_XTRACT_SCALAR, { XTRACT_BARK_COEFFICIENTS } )
{
    mEnumStr    = "XTRACT_LOUDNESS";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_BARK_COEFFICIENTS);
}

void ciXtractLoudness::update()
{
    xtract_loudness( mData.get(), XTRACT_BARK_BANDS, NULL, mResult.get() );
}

// Flatness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractFlatness::ciXtractFlatness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FLATNESS, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_FLATNESS";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractFlatness::update()
{
    xtract_flatness( mData.get(), FFT_SIZE, NULL, mResult.get() );
}


// Flatness Db                                                                                      //
// ------------------------------------------------------------------------------------------------ //
ciXtractFlatnessDb::ciXtractFlatnessDb( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FLATNESS_DB, name, CI_XTRACT_SCALAR, { XTRACT_FLATNESS } )
{
    mEnumStr    = "XTRACT_FLATNESS_DB";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractFlatnessDb::update()
{
    xtract_flatness_db( NULL, 0, mXtract->getFeatureResult(XTRACT_FLATNESS).get(), mResult.get() );
}

// Tonality                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractTonality::ciXtractTonality( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_TONALITY, name, CI_XTRACT_SCALAR, { XTRACT_FLATNESS_DB } )
{
    mEnumStr    = "XTRACT_TONALITY";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractTonality::update()
{
    xtract_tonality( NULL, 0, mXtract->getFeatureResult(XTRACT_FLATNESS_DB).get(), mResult.get() );
}

// RMS Amplitude                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractRmsAmplitude::ciXtractRmsAmplitude( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_RMS_AMPLITUDE, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_RMS_AMPLITUDE";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractRmsAmplitude::update()
{
    xtract_rms_amplitude( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Spectral Inhamornicity                                                                           //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralInharmonicity::ciXtractSpectralInharmonicity( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_INHARMONICITY, name, CI_XTRACT_SCALAR, { XTRACT_PEAK_SPECTRUM, XTRACT_F0 } )
{
    mEnumStr    = "XTRACT_SPECTRAL_INHARMONICITY";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_PEAK_SPECTRUM);
}

void ciXtractSpectralInharmonicity::update()
{
    xtract_spectral_inharmonicity( mData.get(), FFT_SIZE, mXtract->getFeatureResult(XTRACT_F0).get(), mResult.get() );
}

// Crest                                                                                            //
// ------------------------------------------------------------------------------------------------ //
ciXtractCrest::ciXtractCrest( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_CREST, name, CI_XTRACT_SCALAR, { XTRACT_HIGHEST_VALUE, XTRACT_MEAN } )
{
    mEnumStr    = "XTRACT_CREST";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
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
: ciXtractFeature( xtract, XTRACT_POWER, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_POWER";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractPower::update()
{
    xtract_power( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Odd Even Ratio                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractOddEvenRatio::ciXtractOddEvenRatio( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ODD_EVEN_RATIO, name, CI_XTRACT_SCALAR, { XTRACT_HARMONIC_SPECTRUM } )
{
    mEnumStr    = "XTRACT_ODD_EVEN_RATIO";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_HARMONIC_SPECTRUM);
}

void ciXtractOddEvenRatio::update()
{
    xtract_odd_even_ratio( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Sharpness                                                                                        //
// ------------------------------------------------------------------------------------------------ //
ciXtractSharpness::ciXtractSharpness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SHARPNESS, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_SHARPNESS";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSharpness::update()
{
    xtract_sharpness( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Spectral Slope                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralSlope::ciXtractSpectralSlope( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_SLOPE, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_SPECTRAL_SLOPE";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSpectralSlope::update()
{
    xtract_spectral_slope( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Lowest Value                                                                                     //
// ------------------------------------------------------------------------------------------------ //
ciXtractLowestValue::ciXtractLowestValue( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_LOWEST_VALUE, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr                = "XTRACT_LOWEST_VALUE";
    mResult                 = std::shared_ptr<double>( new double(0.0f) );
    mData                   = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    mParams["lower_limit"]  = { 0.2f, CI_XTRACT_PARAM_DOUBLE };
}

void ciXtractLowestValue::update()
{
    xtract_lowest_value( mData.get(), FFT_SIZE, &mParams["lower_limit"], mResult.get() );
}

// Highest Value                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractHighestValue::ciXtractHighestValue( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_HIGHEST_VALUE, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_HIGHEST_VALUE";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractHighestValue::update()
{
    xtract_highest_value( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Sum                                                                                              //
// ------------------------------------------------------------------------------------------------ //
ciXtractSum::ciXtractSum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SUM, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_SUM";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
    mData       = mXtract->getFeatureResult(XTRACT_SPECTRUM);
}

void ciXtractSum::update()
{
    xtract_sum( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

// Non-Zero Count                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractNonZeroCount::ciXtractNonZeroCount( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_NONZERO_COUNT, name, CI_XTRACT_SCALAR, { XTRACT_SPECTRUM } )
{
    mEnumStr    = "XTRACT_NONZERO_COUNT";
    mResult     = std::shared_ptr<double>( new double(0.0f) );
}

void ciXtractNonZeroCount::update()
{
    mData = mXtract->getFeatureResult(XTRACT_SPECTRUM);
    xtract_nonzero_count( mData.get(), FFT_SIZE, NULL, mResult.get() );
}

