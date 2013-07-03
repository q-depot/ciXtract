/*
 *  ciXtract.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


ciXtract::ciXtract( audio::Input source )
{
    mInputSource    = source;
    mFontSmall      = gl::TextureFont::create( Font( "Helvetica", 12 ) );
    
    init();
}


ciXtract::~ciXtract()
{
    // TODO clean up
}


void ciXtract::init()
{
    mRunCalibration     = false;
    
    mPcmData            = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    mSpectrum           = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    mPeakSpectrum       = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    mHarmonicSpectrum   = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    mAutocorrelation    = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    mAutocorrelationFft = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    mSubBands           = std::shared_ptr<double>( new double[ SUBBANDS_N ] );
    
    for( size_t k=0; k < PCM_SIZE; k++ )
    {
        mPcmData.get()[k]               = 0.0f;
        mSpectrum.get()[k]              = 0.0f;
        mPeakSpectrum.get()[k]          = 0.0f;
        mHarmonicSpectrum.get()[k]      = 0.0f;
        mAutocorrelation.get()[k]       = 0.0f;
        mAutocorrelationFft.get()[k]    = 0.0f;
    }
    
    for( size_t k=0; k < SUBBANDS_N; k++ )
        mSubBands.get()[k] = 0.0f;
    
    for( size_t k=0; k < XTRACT_FEATURES; k++ )
        mScalarValues[k] = 0.0f;
    
    initFft();
    
    initMfccs();
    
    initBarks();
    
    initCallbacks();
}


void ciXtract::initFft()
{
    xtract_init_fft( PCM_SIZE << 1, XTRACT_SPECTRUM );
    xtract_init_fft( PCM_SIZE, XTRACT_AUTOCORRELATION_FFT );
}

void ciXtract::initMfccs()
{
    mMfccs              = std::shared_ptr<double>( new double[ MFCC_FREQ_BANDS ] );
    
    for( size_t k=0; k < MFCC_FREQ_BANDS; k++ )
        mMfccs.get()[k] = 0.0f;
    
    mMelFilters.n_filters = MFCC_FREQ_BANDS;
    mMelFilters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
        mMelFilters.filters[n] = (double *)malloc(PCM_SIZE * sizeof(double));
    
    xtract_init_mfcc( FFT_SIZE, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mMelFilters.n_filters, mMelFilters.filters );
}

void ciXtract::initBarks()
{
    mBarks              = std::shared_ptr<double>( new double[ XTRACT_BARK_BANDS ] );
    mBarkBandLimits     = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );
    
    for( size_t k=0; k < XTRACT_BARK_BANDS; k++ )
        mBarks.get()[k] = 0.0f;
    
    xtract_init_bark( FFT_SIZE, SAMPLERATE >> 1, mBarkBandLimits.get() );
    
}

void ciXtract::initParams()
{
    mParams["spectrum_sample_rate_N"]   = SAMPLERATE / (double)PCM_SIZE;
    mParams["spectrum_type"]            = XTRACT_MAGNITUDE_SPECTRUM;    // XTRACT_MAGNITUDE_SPECTRUM, XTRACT_LOG_MAGNITUDE_SPECTRUM, XTRACT_POWER_SPECTRUM, XTRACT_LOG_POWER_SPECTRUM
    mParams["spectrum_dc"]              = 0.0f;
    mParams["spectrum_norm"]            = 0.0f;
    mParams["peak_spectrum_threshold"]  = 0.3f;
}

void ciXtract::initCallbacks()
{
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SPECTRUM,             "SPECTRUM",             CI_XTRACT_VECTOR, std::bind( &ciXtract::updateSpectrum, this ), {}, FFT_SIZE ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_AUTOCORRELATION,      "AUTOCORRELATION",      CI_XTRACT_VECTOR, std::bind( &ciXtract::updateAutoCorrelation, this ), {}, PCM_SIZE ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_AUTOCORRELATION_FFT,  "AUTOCORRELATION_FFT",  CI_XTRACT_VECTOR, std::bind( &ciXtract::updateAutoCorrelationFft, this ), {}, PCM_SIZE ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_PEAK_SPECTRUM,        "PEAK_SPECTRUM",        CI_XTRACT_VECTOR, std::bind( &ciXtract::updatePeakSpectrum, this ), { XTRACT_SPECTRUM }, FFT_SIZE ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SUBBANDS,             "SUBBANDS",             CI_XTRACT_VECTOR, std::bind( &ciXtract::updateSubBands, this ), { XTRACT_SPECTRUM }, SUBBANDS_N ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_MFCC,                 "MFCC",                 CI_XTRACT_VECTOR, std::bind( &ciXtract::updateMfcc, this ), { XTRACT_SPECTRUM }, MFCC_FREQ_BANDS ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_BARK_COEFFICIENTS,    "BARK_COEFFICIENTS",    CI_XTRACT_VECTOR, std::bind( &ciXtract::updateBarkCoefficients, this ), { XTRACT_SPECTRUM }, XTRACT_BARK_BANDS ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_F0,                   "F0",                   CI_XTRACT_SCALAR, std::bind( &ciXtract::updateF0, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_HARMONIC_SPECTRUM,    "HARMONIC_SPECTRUM",    CI_XTRACT_VECTOR, std::bind( &ciXtract::updateHarmonicSpectrum, this ), { XTRACT_PEAK_SPECTRUM, XTRACT_F0 }, FFT_SIZE ) );
    
    
    mFeatures.push_back( ciXtractFeature::create( XTRACT_MEAN,                 "MEAN",                 CI_XTRACT_SCALAR, std::bind( &ciXtract::updateMean, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_VARIANCE,             "VARIANCE",             CI_XTRACT_SCALAR, std::bind( &ciXtract::updateVariance, this ), { XTRACT_MEAN } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_STANDARD_DEVIATION,   "STANDARD_DEVIATION",   CI_XTRACT_SCALAR, std::bind( &ciXtract::updateStandardDeviation, this ), { XTRACT_VARIANCE } ) );
    
    mFeatures.push_back( ciXtractFeature::create( XTRACT_AVERAGE_DEVIATION,    "AVERAGE_DEVIATION",    CI_XTRACT_SCALAR, std::bind( &ciXtract::updateAverageDeviation, this ), { XTRACT_MEAN } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SKEWNESS,             "SKEWNESS",             CI_XTRACT_SCALAR, std::bind( &ciXtract::updateSkewness, this ), { XTRACT_STANDARD_DEVIATION } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_KURTOSIS,             "KURTOSIS",             CI_XTRACT_SCALAR, std::bind( &ciXtract::updateKurtosis, this ), { XTRACT_STANDARD_DEVIATION } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SPECTRAL_MEAN,        "SPECTRAL_MEAN",        CI_XTRACT_SCALAR, std::bind( &ciXtract::updateSpectralMean, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SPECTRAL_VARIANCE,    "SPECTRAL_VARIANCE",    CI_XTRACT_SCALAR, std::bind( &ciXtract::updateSpectralVariance, this ), { XTRACT_SPECTRAL_MEAN } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SPECTRAL_STANDARD_DEVIATION, "SPECTRAL_STANDARD_DEVIATION", CI_XTRACT_SCALAR, std::bind( &ciXtract::updateSpectralStandardDeviation, this ), { XTRACT_SPECTRAL_VARIANCE } ) );
    
    
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SPECTRAL_SKEWNESS,    "SPECTRAL_SKEWNESS",    CI_XTRACT_SCALAR, std::bind( &ciXtract::updateSpectralSkewness, this ), { XTRACT_SPECTRAL_MEAN } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SPECTRAL_KURTOSIS,    "SPECTRAL_KURTOSIS",    CI_XTRACT_SCALAR, std::bind( &ciXtract::updateSpectralKurtosis, this ), { XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_STANDARD_DEVIATION, XTRACT_SPECTRAL_STANDARD_DEVIATION } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SPECTRAL_CENTROID,    "SPECTRAL_CENTROID",    CI_XTRACT_SCALAR, std::bind( &ciXtract::updateSpectralCentroid, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_IRREGULARITY_K,       "IRREGULARITY_K",       CI_XTRACT_SCALAR, std::bind( &ciXtract::updateIrregularityK, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_IRREGULARITY_J,       "IRREGULARITY_J",       CI_XTRACT_SCALAR, std::bind( &ciXtract::updateIrregularityJ, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_TRISTIMULUS_1,        "TRISTIMULUS_1",        CI_XTRACT_SCALAR, std::bind( &ciXtract::updateTristimulus1, this ), { XTRACT_HARMONIC_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SMOOTHNESS,           "SMOOTHNESS",           CI_XTRACT_SCALAR, std::bind( &ciXtract::updateSmoothness, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SPREAD,               "SPREAD",               CI_XTRACT_SCALAR, std::bind( &ciXtract::updateSpread, this ), { XTRACT_SPECTRAL_CENTROID } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_ZCR,                  "ZCR",                  CI_XTRACT_SCALAR, std::bind( &ciXtract::updateZcr, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_ROLLOFF,              "ROLLOFF",              CI_XTRACT_SCALAR, std::bind( &ciXtract::updateRollOff, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_LOUDNESS,             "LOUDNESS",             CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_BARK_COEFFICIENTS } ) );
    
    
    mFeatures.push_back( ciXtractFeature::create( XTRACT_FLATNESS,             "FLATNESS",             CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_FLATNESS_DB,          "FLATNESS_DB",          CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_FLATNESS } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_TONALITY,             "TONALITY",             CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_FLATNESS_DB } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_RMS_AMPLITUDE,        "RMS_AMPLITUDE",        CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SPECTRAL_INHARMONICITY, "SPECTRAL_INHARMONICITY", CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_PEAK_SPECTRUM, XTRACT_F0 } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_POWER,                "POWER",                CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_ODD_EVEN_RATIO,       "ODD_EVEN_RATIO",       CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_HARMONIC_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SHARPNESS,            "SHARPNESS",            CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SPECTRAL_SLOPE,       "SPECTRAL_SLOPE",       CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_LOWEST_VALUE,         "LOWEST_VALUE",         CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_HIGHEST_VALUE,        "HIGHEST_VALUE",        CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_SUM,                  "SUM",                  CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_SPECTRUM } ) );
    mFeatures.push_back( ciXtractFeature::create( XTRACT_NONZERO_COUNT,        "XTRACT_NONZERO_COUNT", CI_XTRACT_SCALAR, std::bind( &ciXtract::updateLoudness, this ), { XTRACT_SPECTRUM } ) );
    /*
    mFeatures.push_back( 	{ XTRACT_SPECTRUM, "SPECTRUM", std::bind( &ciXtract::updateSpectrum, this ), false, VECTOR_FEATURE,
                            { }, 0.0f, 0.01f, FFT_SIZE } );
    
    mFeatures.push_back( 	{ XTRACT_AUTOCORRELATION, "AUTOCORRELATION", std::bind( &ciXtract::updateAutoCorrelation, this ), false, VECTOR_FEATURE,
                            { }, 0.0f, 0.001f, PCM_SIZE } );
    
    mFeatures.push_back( 	{ XTRACT_AUTOCORRELATION_FFT, "AUTOCORRELATION_FFT", std::bind( &ciXtract::updateAutoCorrelationFft, this ), false, VECTOR_FEATURE,
                            { }, 0.0f, 1.0f, PCM_SIZE } );
    
    mFeatures.push_back( 	{ XTRACT_PEAK_SPECTRUM, "PEAK_SPECTRUM", std::bind( &ciXtract::updatePeakSpectrum, this ), false, VECTOR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 0.01f, FFT_SIZE } );
    
    mFeatures.push_back( 	{ XTRACT_SUBBANDS, "SUBBANDS", std::bind( &ciXtract::updateSubBands, this ), false, VECTOR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 0.01f, SUBBANDS_N } );
    
    mFeatures.push_back( 	{ XTRACT_MFCC, "MFCC", std::bind( &ciXtract::updateMfcc, this ), false,  VECTOR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 10.0f, MFCC_FREQ_BANDS } );
    
    mFeatures.push_back( 	{ XTRACT_F0, "F0", std::bind( &ciXtract::updateF0, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 1.0f } );
    
    mFeatures.push_back( 	{ XTRACT_BARK_COEFFICIENTS, "BARK_COEFFICIENTS", std::bind( &ciXtract::updateBarkCoefficients, this ), false, VECTOR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 0.1f, XTRACT_BARK_BANDS } );
    
    mFeatures.push_back( 	{ XTRACT_HARMONIC_SPECTRUM, "HARMONIC_SPECTRUM", std::bind( &ciXtract::updateHarmonicSpectrum, this ), false, VECTOR_FEATURE,
                            { XTRACT_PEAK_SPECTRUM, XTRACT_F0 }, 0.0f, 0.01f, FFT_SIZE } );

    mFeatures.push_back( 	{ XTRACT_MEAN, "MEAN", std::bind( &ciXtract::updateMean, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 0.01f } );
    
    mFeatures.push_back( 	{ XTRACT_VARIANCE, "VARIANCE", std::bind( &ciXtract::updateVariance, this ), false, SCALAR_FEATURE,
                            { XTRACT_MEAN }, 0.0f, 0.0001f } );
    
    mFeatures.push_back( 	{ XTRACT_STANDARD_DEVIATION, "STANDARD_DEVIATION", std::bind( &ciXtract::updateStandardDeviation, this ), false, SCALAR_FEATURE,
                            { XTRACT_VARIANCE }, 0.0f, 0.1f } );
    
    mFeatures.push_back( 	{ XTRACT_AVERAGE_DEVIATION, "AVERAGE_DEVIATION", std::bind( &ciXtract::updateAverageDeviation, this ), false, SCALAR_FEATURE ,
                            { XTRACT_MEAN }, 0.0f, 0.01f } );
    
    mFeatures.push_back( 	{ XTRACT_SKEWNESS, "SKEWNESS", std::bind( &ciXtract::updateSkewness, this ), false, SCALAR_FEATURE,
                            { XTRACT_STANDARD_DEVIATION }, 0.0f, 1000000000000.0f } );
    
    mFeatures.push_back( 	{ XTRACT_KURTOSIS, "KURTOSIS", std::bind( &ciXtract::updateKurtosis, this ), false, SCALAR_FEATURE,
                            { XTRACT_STANDARD_DEVIATION }, 0.0f, 1000.0f } );
    
    mFeatures.push_back( 	{ XTRACT_SPECTRAL_MEAN, "SPECTRAL_MEAN", std::bind( &ciXtract::updateSpectralMean, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 0.001f } );
    
    mFeatures.push_back( 	{ XTRACT_SPECTRAL_VARIANCE, "SPECTRAL_VARIANCE", std::bind( &ciXtract::updateSpectralVariance, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRAL_MEAN }, 0.0f, 0.00001f } );
    
    mFeatures.push_back( 	{ XTRACT_SPECTRAL_STANDARD_DEVIATION, "SPECTRAL_STANDARD_DEVIATION", std::bind( &ciXtract::updateSpectralStandardDeviation, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRAL_VARIANCE }, 0.0f, 0.01f } );

    mFeatures.push_back( 	{ XTRACT_SPECTRAL_SKEWNESS, "SPECTRAL_SKEWNESS", std::bind( &ciXtract::updateSpectralSkewness, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRAL_MEAN }, 0.0f, 1.0f } );
    
    mFeatures.push_back( 	{ XTRACT_SPECTRAL_KURTOSIS, "SPECTRAL_KURTOSIS", std::bind( &ciXtract::updateSpectralKurtosis, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRAL_MEAN,XTRACT_SPECTRAL_STANDARD_DEVIATION,XTRACT_SPECTRAL_STANDARD_DEVIATION }, 0.0f, 10.0f } );
    
    mFeatures.push_back( 	{ XTRACT_SPECTRAL_CENTROID, "SPECTRAL_CENTROID", std::bind( &ciXtract::updateSpectralCentroid, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 0.001f } );
    
    mFeatures.push_back( 	{ XTRACT_IRREGULARITY_K, "IRREGULARITY_K", std::bind( &ciXtract::updateIrregularityK, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 1.0f } );
    
    mFeatures.push_back( 	{ XTRACT_IRREGULARITY_J, "IRREGULARITY_J", std::bind( &ciXtract::updateIrregularityJ, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 1000000.0f } );
    
    mFeatures.push_back( 	{ XTRACT_TRISTIMULUS_1, "TRISTIMULUS_1", std::bind( &ciXtract::updateTristimulus1, this ), false, SCALAR_FEATURE,
                            { XTRACT_HARMONIC_SPECTRUM }, 0.0f, 1.0f } );
    
    mFeatures.push_back( 	{ XTRACT_SMOOTHNESS, "SMOOTHNESS", std::bind( &ciXtract::updateSmoothness, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 1.0f } );
    
    mFeatures.push_back( 	{ XTRACT_SPREAD, "SPREAD", std::bind( &ciXtract::updateSpread, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRAL_CENTROID }, 0.0f, 0.000001f } );
    
    mFeatures.push_back( 	{ XTRACT_ZCR, "ZCR", std::bind( &ciXtract::updateZcr, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, -1.0f, 1.0f } );
    
    mFeatures.push_back( 	{ XTRACT_ROLLOFF, "ROLLOFF", std::bind( &ciXtract::updateRollOff, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 1000.0f } );
    
    mFeatures.push_back( 	{ XTRACT_LOUDNESS, "LOUDNESS", std::bind( &ciXtract::updateLoudness, this ), false, SCALAR_FEATURE,
                            { XTRACT_BARK_COEFFICIENTS }, -1.0f, 0.001f } );
    
    mFeatures.push_back( 	{ XTRACT_FLATNESS, "FLATNESS", std::bind( &ciXtract::updateFlatness, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 10000.0f } );
    
    mFeatures.push_back( 	{ XTRACT_FLATNESS_DB, "FLATNESS_DB", std::bind( &ciXtract::updateFlatnessDb, this ), false, SCALAR_FEATURE,
                            { XTRACT_FLATNESS }, 0.0f, 100.0f } );
    
    mFeatures.push_back( 	{ XTRACT_TONALITY, "TONALITY", std::bind( &ciXtract::updateTonality, this ), false, SCALAR_FEATURE,
                            { XTRACT_FLATNESS_DB }, 0.0f, 1.0f } );
    
    mFeatures.push_back( 	{ XTRACT_RMS_AMPLITUDE, "RMS_AMPLITUDE", std::bind( &ciXtract::updateRmsAmplitude, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 0.001f } );
    
    mFeatures.push_back( 	{ XTRACT_SPECTRAL_INHARMONICITY, "SPECTRAL_INHARMONICITY", std::bind( &ciXtract::updateSpectralInharmonicity, this ), false, SCALAR_FEATURE,
                            { XTRACT_PEAK_SPECTRUM, XTRACT_F0 }, 0.0f, 100.0f } );
    
    mFeatures.push_back( 	{ XTRACT_POWER, "POWER", std::bind( &ciXtract::updatePower, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 1.0f } );
    
    mFeatures.push_back( 	{ XTRACT_ODD_EVEN_RATIO, "ODD_EVEN_RATIO", std::bind( &ciXtract::updateOddEvenRatio, this ), false, SCALAR_FEATURE,
                            { XTRACT_HARMONIC_SPECTRUM }, 0.0f, 10.0f } );
    
    mFeatures.push_back( 	{ XTRACT_SHARPNESS, "SHARPNESS", std::bind( &ciXtract::updateSharpness, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 1.0f } );
    
    mFeatures.push_back( 	{ XTRACT_SPECTRAL_SLOPE, "SPECTRAL_SLOPE", std::bind( &ciXtract::updateSpectralSlope, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 100.0f } );
    
    mFeatures.push_back( 	{ XTRACT_LOWEST_VALUE, "LOWEST_VALUE", std::bind( &ciXtract::updateLowestValue, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 0.001f } );
    
    mFeatures.push_back( 	{ XTRACT_HIGHEST_VALUE, "HIGHEST_VALUE", std::bind( &ciXtract::updateHighestValue, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 1.0f } );
    
    mFeatures.push_back( 	{ XTRACT_SUM, "SUM", std::bind( &ciXtract::updateSum, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 1.0f } );
    
    mFeatures.push_back( 	{ XTRACT_NONZERO_COUNT, "NONZERO_COUNT", std::bind( &ciXtract::updateNonZeroCount, this ), false, SCALAR_FEATURE,
                            { XTRACT_SPECTRUM }, 0.0f, 1000.0f } );
    
    */
    //    mFeatures[XTRACT_CREST]                        = { "CREST", std::bind( &ciXtract::updateCrest, this ), false };
    //    mFeatures[XTRACT_NOISINESS]                    = { "NOISINESS", std::bind( &ciXtract::updateNoisiness, this ), false };
    //    mFeatures[XTRACT_HPS]                          = { "HPS", std::bind( &ciXtract::updateHps, this ), false };
    //    mFeatures[XTRACT_FAILSAFE_F0]                  = { "FAILSAFE_F0", std::bind( &ciXtract::updateFailsafeF0, this ), false };
    //    mFeatures[XTRACT_LNORM]                        = { "LNORM", std::bind( &ciXtract::updateLnorm, this ), false };
    //    mFeatures[XTRACT_FLUX]                         = { "FLUX", std::bind( &ciXtract::updateFlux, this ), false };
    //    mFeatures[XTRACT_ATTACK_TIME]                  = { "ATTACK_TIME", std::bind( &ciXtract::updateAttackTime, this ), false };
    //    mFeatures[XTRACT_DECAY_TIME]                   = { "DECAY_TIME", std::bind( &ciXtract::updateDecayTime, this ), false };
    //    mFeatures[XTRACT_DIFFERENCE_VECTOR]            = { "DIFFERENCE_VECTOR", std::bind( &ciXtract::updateDifferenceVector, this ), false };
    
    //    mFeatures[XTRACT_AMDF]                         = { "AMDF", std::bind( &ciXtract::updateAmdf, this ), false };
    //    mFeatures[XTRACT_ASDF]                         = { "ASDF", std::bind( &ciXtract::updateAsdf, this ), false };
    //    mFeatures[XTRACT_LPC]                          = { "LPC", std::bind( &ciXtract::updateLpc, this ), false };
    //    mFeatures[XTRACT_LPCC]                         = { "LPCC", std::bind( &ciXtract::updateLpcc, this ), false };
    //    mFeatures[XTRACT_DCT]                          = { "DCT", std::bind( &ciXtract::updateDct, this ), false };
    //    mFeatures[XTRACT_SUBBANDS]                     = { "SUBBANDS", std::bind( &ciXtract::updateSubbands, this ), false };
    //    mFeatures[XTRACT_WINDOWED]                     = { "WINDOWED", std::bind( &ciXtract::updateWindowed, this ), false };
}

void ciXtract::update()
{
    if ( !mInputSource )
        return;
    
    mPcmBuffer = mInputSource.getPcmBuffer();

	if( !mPcmBuffer )
		return;
    
//    audio::Buffer32fRef buff        = mPcmBuffer->getInterleavedData();
//	audio::Buffer32fRef leftBuffer  = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
  	audio::Buffer32fRef buff  = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
    for( size_t k=0; k < PCM_SIZE; k++ )
        mPcmData.get()[k] = buff->mData[k];
    
//    console() << mPcmBuffer->isInterleaved() << " " << mPcmBuffer->getMaxSampleCount() << " " << mPcmBuffer->getSampleCount() << " " << mPcmBuffer->getChannelCount();
//    console() << " --- " << buff->mSampleCount << " " << buff->mNumberChannels << " " << PCM_SIZE << endl;
//    console() << leftBuffer->mNumberChannels << " " << leftBuffer->mSampleCount << " --- ";
//    console() << buff->mNumberChannels << " " << buff->mSampleCount << endl;
    
//    for( size_t k=0; k < PCM_SIZE; k++ )
//        mPcmData.get()[k] = buff->mData[k*2];
    
    vector<ciXtractFeatureRef>::iterator it;
    for( it = mFeatures.begin(); it!=mFeatures.end(); ++it )
        (*it)->update();
    
    if ( mRunCalibration )
        updateCalibration();
}


void ciXtract::enableFeature( xtract_features_ feature )
{
    ciXtractFeatureRef f = getFeature( feature );

    if ( !f )
        return;

    f->mIsEnable = true;

    vector<xtract_features_> dependencies = f->mDependencies;
    for( auto k=0; k < dependencies.size(); k++ )
        enableFeature( dependencies[k] );
}


void ciXtract::disableFeature( xtract_features_ feature )
{
    ciXtractFeatureRef f = getFeature( feature );
    if ( !f )
        return;
    
    f->mIsEnable = false;
    
    // disable all features that depends on this one
    std::vector<ciXtractFeatureRef>::iterator it;
    for( it = mFeatures.begin(); it != mFeatures.end(); ++it )
        if ( featureDependsOn( (*it)->mFeature, feature ) )
            disableFeature( (*it)->mFeature );
}


void ciXtract::toggleFeature( xtract_features_ feature )
{
    ciXtractFeatureRef f = getFeature( feature );
    if ( !f )
        return;

    if ( f->isEnable() )
        disableFeature( feature );
    else
        enableFeature( feature );
}


bool ciXtract::featureDependsOn( xtract_features_ this_feature, xtract_features_ test_feature )
{
    ciXtractFeatureRef f = getFeature( this_feature );
    if ( !f )
        return false;
    
    vector<xtract_features_> dependencies = f->mDependencies;
    for( auto i=0; i < dependencies.size(); i++ )
        if ( test_feature == dependencies[i] )
            return true;
    
    return false;
}


ciXtractFeatureRef ciXtract::getFeature( xtract_features_ feature )
{
    std::vector<ciXtractFeatureRef>::iterator it;
    for( it = mFeatures.begin(); it != mFeatures.end(); ++it )
        if ( (*it)->getEnum() == feature )
        {
            return (*it);
        }
    
    return ciXtractFeatureRef();
}


std::shared_ptr<double> ciXtract::getVectorFeature( xtract_features_ feature )
{
    if ( feature == XTRACT_SPECTRUM )
        return mSpectrum;

    else if ( feature == XTRACT_PEAK_SPECTRUM )
        return mPeakSpectrum;
    
    else if ( feature == XTRACT_BARK_COEFFICIENTS )
        return mBarks;
    
    else if ( feature == XTRACT_HARMONIC_SPECTRUM )
        return mHarmonicSpectrum;
    
    else if ( feature == XTRACT_MFCC )
        return mMfccs;
    
    else if ( feature == XTRACT_AUTOCORRELATION )
        return mAutocorrelation;
    
    else if ( feature == XTRACT_AUTOCORRELATION_FFT )
        return mAutocorrelationFft;
    
    else if ( feature == XTRACT_SUBBANDS )
        return mSubBands;
    
    else
    {
        console() << "getVectorFeature() feature not found! " << feature << endl;
        exit(-1);
    }
}


// ------------------------------------ //
//              Callbacks               //
// ------------------------------------ //

void ciXtract::updateMean()
{
    double *argd = NULL;
    xtract_mean( mSpectrum.get(), FFT_SIZE, argd, &mScalarValues[XTRACT_MEAN] );
}

void ciXtract::updateVariance()
{
    xtract_variance( mSpectrum.get(), FFT_SIZE, &mScalarValues[XTRACT_MEAN], &mScalarValues[XTRACT_VARIANCE] );
}

void ciXtract::updateStandardDeviation()
{
    xtract_standard_deviation( mSpectrum.get(), FFT_SIZE, &mScalarValues[XTRACT_VARIANCE], &mScalarValues[XTRACT_STANDARD_DEVIATION] );
}

void ciXtract::updateAverageDeviation()
{
    xtract_average_deviation( mSpectrum.get(), FFT_SIZE, &mScalarValues[XTRACT_MEAN], &mScalarValues[XTRACT_AVERAGE_DEVIATION] );
}

void ciXtract::updateSkewness()
{
    double data[2] = { mScalarValues[XTRACT_MEAN], mScalarValues[XTRACT_STANDARD_DEVIATION] };
    xtract_skewness( mSpectrum.get(), FFT_SIZE, data, &mScalarValues[XTRACT_SKEWNESS] );
}

void ciXtract::updateKurtosis()
{
    double data[2] = { mScalarValues[XTRACT_MEAN], mScalarValues[XTRACT_STANDARD_DEVIATION] };
    xtract_kurtosis( mSpectrum.get(), FFT_SIZE, data, &mScalarValues[XTRACT_KURTOSIS] );
}

void ciXtract::updateSpectralMean()
{
    xtract_spectral_mean( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_SPECTRAL_MEAN] );
}

void ciXtract::updateSpectralVariance()
{
    xtract_spectral_variance( mSpectrum.get(), FFT_SIZE, &mScalarValues[XTRACT_SPECTRAL_MEAN], &mScalarValues[XTRACT_SPECTRAL_VARIANCE] );
}

void ciXtract::updateSpectralStandardDeviation()
{
    xtract_spectral_standard_deviation( mSpectrum.get(), FFT_SIZE, &mScalarValues[XTRACT_SPECTRAL_VARIANCE], &mScalarValues[XTRACT_SPECTRAL_STANDARD_DEVIATION] );
}

void ciXtract::updateSpectralSkewness()
{
    xtract_spectral_skewness( mSpectrum.get(), FFT_SIZE, &mScalarValues[XTRACT_SPECTRAL_MEAN], &mScalarValues[XTRACT_SPECTRAL_SKEWNESS] );
}

void ciXtract::updateSpectralKurtosis()
{
    double data[2] = { mScalarValues[XTRACT_SPECTRAL_MEAN], mScalarValues[XTRACT_SPECTRAL_STANDARD_DEVIATION] };
    xtract_spectral_kurtosis( mSpectrum.get(), FFT_SIZE, data, &mScalarValues[XTRACT_SPECTRAL_KURTOSIS] );
}

void ciXtract::updateSpectralCentroid()
{
    xtract_spectral_centroid( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_SPECTRAL_CENTROID] );
}

void ciXtract::updateIrregularityK()
{
    xtract_irregularity_k( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_IRREGULARITY_K] );
}

void ciXtract::updateIrregularityJ()
{
    xtract_irregularity_j( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_IRREGULARITY_J] );
}

void ciXtract::updateTristimulus1()
{
    xtract_tristimulus_1( mHarmonicSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_TRISTIMULUS_1] );
}


// TODO: add params!
void ciXtract::updateSmoothness()
{
    int data[3] = { 0, 10, 10 };    // lower bound, upper bound, and pre-scaling factor, whereby array data in the range lower < n < upper will be pre-scaled by p before processing.
    xtract_smoothness( mSpectrum.get(), FFT_SIZE, data, &mScalarValues[XTRACT_SMOOTHNESS] );
}

void ciXtract::updateSpread()
{
    xtract_spread( mSpectrum.get(), FFT_SIZE, &mScalarValues[XTRACT_SPECTRAL_CENTROID], &mScalarValues[XTRACT_SPREAD] );
}

void ciXtract::updateZcr()
{
    xtract_zcr( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_ZCR] );
}

// TODO: add params!
void ciXtract::updateRollOff()
{
    //    spectral rolloff in Hz of N values from the array pointed to by *data. This is the point in the spectrum below which argv[0] of the energy is distributed.
    double data[3] = { (double)SAMPLERATE / (double)( FFT_SIZE ), 10 }; //  (samplerate / N ) and a double representing the threshold for rolloff
    xtract_rolloff( mSpectrum.get(), FFT_SIZE, data, &mScalarValues[XTRACT_ROLLOFF] );
}

void ciXtract::updateLoudness()
{
    xtract_loudness( mBarks.get(), XTRACT_BARK_BANDS, NULL, &mScalarValues[XTRACT_LOUDNESS] );
}

void ciXtract::updateFlatness()
{
    xtract_flatness( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_FLATNESS] );
}

void ciXtract::updateFlatnessDb()
{
    xtract_flatness_db( NULL, 0, &mScalarValues[XTRACT_FLATNESS], &mScalarValues[XTRACT_FLATNESS_DB] );
}

void ciXtract::updateTonality()
{
    xtract_tonality( NULL, 0, &mScalarValues[XTRACT_FLATNESS_DB], &mScalarValues[XTRACT_TONALITY] );
}

// TODO requires: maximum value in a spectrum, and a double representing the mean value of a spectrum
//void ciXtract::updateCrest()
//{
//    xtract_crest( NULL, 0, &mScalarValues[XTRACT_SPECTRAL_FLATNESS_DB], &mScalarValues[XTRACT_CREST] );
//}

//void ciXtract::updateNoisiness() {}

void ciXtract::updateRmsAmplitude()
{
    xtract_rms_amplitude( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_RMS_AMPLITUDE] );
}

void ciXtract::updateSpectralInharmonicity()
{
    xtract_spectral_inharmonicity( mPeakSpectrum.get(), FFT_SIZE, &mScalarValues[XTRACT_F0], &mScalarValues[XTRACT_SPECTRAL_INHARMONICITY] );
}

void ciXtract::updatePower()
{
    xtract_power( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_POWER] );
}

void ciXtract::updateOddEvenRatio()
{
    xtract_odd_even_ratio( mHarmonicSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_ODD_EVEN_RATIO] );
}

void ciXtract::updateSharpness ()
{
    xtract_sharpness( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_SHARPNESS] );
}


void ciXtract::updateF0()
{
    double sample_rate = SAMPLERATE;
    xtract_f0( mSpectrum.get(), FFT_SIZE, &sample_rate, &mScalarValues[XTRACT_F0] );
}

void ciXtract::updateSpectralSlope()
{
    xtract_spectral_slope( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_SPECTRAL_SLOPE] );
}

// TODO: add params!
void ciXtract::updateLowestValue()
{
    double lowerLimit = 0.2f;
    xtract_lowest_value( mSpectrum.get(), FFT_SIZE, &lowerLimit, &mScalarValues[XTRACT_LOWEST_VALUE] );
}

void ciXtract::updateHighestValue()
{
    xtract_highest_value( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_HIGHEST_VALUE] );
}

void ciXtract::updateSum()
{
    xtract_sum( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_SUM] );
}

void ciXtract::updateNonZeroCount()
{
    xtract_nonzero_count( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_NONZERO_COUNT] );
}


// THIS FUNCTION DOESN'T WORK PROPERLY as stated in the LibXtract doc!!!
//void ciXtract::updateHps()
//{
//    xtract_hps( mSpectrum.get(), FFT_SIZE, NULL, &mScalarValues[XTRACT_HPS] );
//}


// Vector /////////////////////////////////

void ciXtract::updateSpectrum()
{
    _argd[0] = mParams["spectrum_sample_rate_N"];
    _argd[1] = mParams["spectrum_type"];
    _argd[2] = mParams["spectrum_dc"];
    _argd[3] = mParams["spectrum_norm"];
    
    xtract_spectrum( mPcmData.get(), PCM_SIZE, _argd, mSpectrum.get() );
}

void ciXtract::updatePeakSpectrum()
{
    double data[2] = { mParams["spectrum_sample_rate_N"], mParams["peak_spectrum_threshold"] };
    xtract_peak_spectrum( mSpectrum.get(), FFT_SIZE, data, mPeakSpectrum.get() );
}

void ciXtract::updateBarkCoefficients()
{
    xtract_bark_coefficients( mSpectrum.get(), FFT_SIZE, mBarkBandLimits.get(), mBarks.get() );
}

// TODO add param
void ciXtract::updateHarmonicSpectrum()
{
    //    F0 and a threshold (t) where 0<=t<=1.0, and t determines the distance from the nearest harmonic number within which a partial can be considered harmonic
    double data[2] = { mScalarValues[XTRACT_F0], 0.3f };
    xtract_harmonic_spectrum( mPeakSpectrum.get(), FFT_SIZE, data, mHarmonicSpectrum.get() );
}

void ciXtract::updateMfcc()
{
    xtract_mfcc( mSpectrum.get(), FFT_SIZE, &mMelFilters, mMfccs.get() );
}

void ciXtract::updateAutoCorrelation()
{
    xtract_autocorrelation( mPcmData.get(), PCM_SIZE, NULL, mAutocorrelation.get() );
}

void ciXtract::updateAutoCorrelationFft()
{
    xtract_autocorrelation_fft( mPcmData.get(), PCM_SIZE, NULL, mAutocorrelationFft.get() );
}

void ciXtract::updateSubBands()
{
    int argd[4] = { XTRACT_MEAN, SUBBANDS_N, XTRACT_LINEAR_SUBBANDS, 5 };       // { XTRACT_SUM, ...  XTRACT_OCTAVE_SUBBANDS,    XTRACT_LINEAR_SUBBANDS
    xtract_subbands( mSpectrum.get(), FFT_SIZE, argd, mSubBands.get() );
}


// Auto Calibration

void ciXtract::autoCalibration()
{
    vector<ciXtractFeatureRef>::iterator it;
    for( it = mFeatures.begin(); it!=mFeatures.end(); ++it )
    {
        (*it)->mResultMin = std::numeric_limits<double>::max();
        (*it)->mResultMax = std::numeric_limits<double>::min();
    }
    
    mRunCalibration = getElapsedSeconds();    
}


void ciXtract::updateCalibration()
{
    if ( getElapsedSeconds() - mRunCalibration > 2.0f )
    {
        mRunCalibration = -1;
        return;
    }
    
    double val;
    
    vector<ciXtractFeatureRef>::iterator it;
    for( it = mFeatures.begin(); it!=mFeatures.end(); ++it )
    {
        if ( ! (*it)->isEnable() )
            continue;
        
        if ( (*it)->mType == CI_XTRACT_SCALAR )
        {
            val = mScalarValues[ (*it)->getEnum() ];
            
            if ( isnan(val) )
                continue;
            
            if ( val > (*it)->mResultMax )
                (*it)->mResultMax = val;
            
            if ( val < (*it)->mResultMin )
                (*it)->mResultMin = val;
        }

        else if ( (*it)->mType == CI_XTRACT_VECTOR )
        {
            
        }
    }
}