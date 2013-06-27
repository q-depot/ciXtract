/*
 *  ciLibXtract.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#include "ciLibXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


ciLibXtract::ciLibXtract( audio::Input source )
{
    mInputSource    = source;
    
    mFontSmall      = gl::TextureFont::create( Font( "Helvetica", 12 ) );
    
    init();
}


ciLibXtract::~ciLibXtract() {}


void ciLibXtract::init()
{
    
// -------------- //
// ----- FFT ---- //
// -------------- //
    
    mPcmData            = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    mSpectrum           = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    mPeakSpectrum       = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    mHarmonicSpectrum   = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    
    for( size_t k=0; k < PCM_SIZE; k++ )
    {
        mPcmData.get()[k]           = 0.0f;
        mSpectrum.get()[k]          = 0.0f;
        mPeakSpectrum.get()[k]      = 0.0f;
        mHarmonicSpectrum.get()[k]  = 0.0f;
    }
    
    xtract_init_fft( PCM_SIZE << 1, XTRACT_SPECTRUM );
//    xtract_init_fft( PCM_SIZE, XTRACT_SPECTRUM );
    
    
// -------------- //
// ---- Mfcc ---- //
// -------------- //
    
    mMfccs              = std::shared_ptr<double>( new double[ MFCC_FREQ_BANDS ] );
    
    for( size_t k=0; k < MFCC_FREQ_BANDS; k++ )
        mMfccs.get()[k] = 0.0f;
    
    mMelFilters.n_filters = MFCC_FREQ_BANDS;
    mMelFilters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
        mMelFilters.filters[n] = (double *)malloc(PCM_SIZE * sizeof(double));
    
    xtract_init_mfcc( PCM_SIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mMelFilters.n_filters, mMelFilters.filters );
    
    
// -------------- //
// --- Barks ---- //
// -------------- //
    
    mBarks              = std::shared_ptr<double>( new double[ XTRACT_BARK_BANDS ] );
    mBarkBandLimits     = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );
    
    for( size_t k=0; k < XTRACT_BARK_BANDS; k++ )
        mBarks.get()[k] = 0.0f;
    
    xtract_init_bark( PCM_SIZE >> 1, SAMPLERATE >> 1, mBarkBandLimits.get() );
    
    
// -------------- //
// --- Scalar --- //
// -------------- //
    
    for( size_t k=0; k < XTRACT_FEATURES; k++ )
        mScalarValues[k] = 0.0f;

    
// -------------- //
// --- Params --- //
// -------------- //
    
//  Spectrum
    mParams["spectrum_sample_rate_N"]   = SAMPLERATE / (double)( PCM_SIZE >> 1 );
    mParams["spectrum_type"]            = XTRACT_MAGNITUDE_SPECTRUM;    // XTRACT_MAGNITUDE_SPECTRUM, XTRACT_LOG_MAGNITUDE_SPECTRUM, XTRACT_POWER_SPECTRUM, XTRACT_LOG_POWER_SPECTRUM
    mParams["spectrum_dc"]              = 0.0f;
    mParams["spectrum_norm"]            = 0.0f;
    mParams["peak_spectrum_threshold"]  = 0.3f;
    
// ----------------- //
// --- Callbacks --- //
// ----------------- //
    
    // TODO
    // FIX THE ORDER!!!!!!!  std::map doesn't sort the callbacks as below!
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // .....................................................
    
    mCallbacks[XTRACT_SPECTRUM]                 = { "XTRACT_SPECTRUM", std::bind( &ciLibXtract::updateSpectrum, this ), false, VECTOR_FEATURE, { } };
    
    mCallbacks[XTRACT_PEAK_SPECTRUM]            = { "XTRACT_PEAK_SPECTRUM", std::bind( &ciLibXtract::updatePeakSpectrum, this ), false, VECTOR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_F0]                       = { "XTRACT_F0", std::bind( &ciLibXtract::updateF0, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_BARK_COEFFICIENTS]        = { "XTRACT_BARK_COEFFICIENTS", std::bind( &ciLibXtract::updateBarkCoefficients, this ), false, VECTOR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_HARMONIC_SPECTRUM]        = { "XTRACT_HARMONIC_SPECTRUM", std::bind( &ciLibXtract::updateHarmonicSpectrum, this ), false, VECTOR_FEATURE,
                                                { XTRACT_PEAK_SPECTRUM, XTRACT_F0 } };
    
    mCallbacks[XTRACT_MEAN]                     = { "XTRACT_MEAN", std::bind( &ciLibXtract::updateMean, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_VARIANCE]                 = { "XTRACT_VARIANCE", std::bind( &ciLibXtract::updateVariance, this ), false, SCALAR_FEATURE,
                                                { XTRACT_MEAN } };
    
    mCallbacks[XTRACT_STANDARD_DEVIATION]       = { "XTRACT_STANDARD_DEVIATION", std::bind( &ciLibXtract::updateStandardDeviation, this ), false, SCALAR_FEATURE,
                                                { XTRACT_VARIANCE } };
    
    mCallbacks[XTRACT_AVERAGE_DEVIATION]        = { "XTRACT_AVERAGE_DEVIATION", std::bind( &ciLibXtract::updateAverageDeviation, this ), false, SCALAR_FEATURE ,
                                                { XTRACT_MEAN } };
    
    mCallbacks[XTRACT_SKEWNESS]                 = { "XTRACT_SKEWNESS", std::bind( &ciLibXtract::updateSkewness, this ), false, SCALAR_FEATURE,
                                                { XTRACT_STANDARD_DEVIATION } };
    
    mCallbacks[XTRACT_KURTOSIS]                 = { "XTRACT_KURTOSIS", std::bind( &ciLibXtract::updateKurtosis, this ), false, SCALAR_FEATURE,
                                                { XTRACT_STANDARD_DEVIATION } };
    
    mCallbacks[XTRACT_SPECTRAL_MEAN]            = { "XTRACT_SPECTRAL_MEAN", std::bind( &ciLibXtract::updateSpectralMean, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_SPECTRAL_VARIANCE]        = { "XTRACT_SPECTRAL_VARIANCE", std::bind( &ciLibXtract::updateSpectralVariance, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRAL_MEAN } };
    
    mCallbacks[XTRACT_SPECTRAL_STANDARD_DEVIATION]  = { "XTRACT_SPECTRAL_STANDARD_DEVIATION", std::bind( &ciLibXtract::updateSpectralStandardDeviation, this ), false, SCALAR_FEATURE,
                                                    { XTRACT_SPECTRAL_VARIANCE } };
    
    mCallbacks[XTRACT_SPECTRAL_SKEWNESS]        = { "XTRACT_SPECTRAL_SKEWNESS", std::bind( &ciLibXtract::updateSpectralSkewness, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRAL_MEAN } };
    
    mCallbacks[XTRACT_SPECTRAL_KURTOSIS]        = { "XTRACT_SPECTRAL_KURTOSIS", std::bind( &ciLibXtract::updateSpectralKurtosis, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_STANDARD_DEVIATION } };
    
    mCallbacks[XTRACT_SPECTRAL_CENTROID]        = { "XTRACT_SPECTRAL_CENTROID", std::bind( &ciLibXtract::updateSpectralCentroid, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_IRREGULARITY_K]           = { "XTRACT_IRREGULARITY_K", std::bind( &ciLibXtract::updateIrregularityK, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_IRREGULARITY_J]           = { "XTRACT_IRREGULARITY_J", std::bind( &ciLibXtract::updateIrregularityJ, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_TRISTIMULUS_1]            = { "XTRACT_TRISTIMULUS_1", std::bind( &ciLibXtract::updateTristimulus1, this ), false, SCALAR_FEATURE,
                                                { XTRACT_HARMONIC_SPECTRUM } };
    
    mCallbacks[XTRACT_SMOOTHNESS]               = { "XTRACT_SMOOTHNESS", std::bind( &ciLibXtract::updateSmoothness, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_SPREAD]                   = { "XTRACT_SPREAD", std::bind( &ciLibXtract::updateSpread, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRAL_CENTROID } };
    
    mCallbacks[XTRACT_ZCR]                      = { "XTRACT_ZCR", std::bind( &ciLibXtract::updateZcr, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_ROLLOFF]                  = { "XTRACT_ROLLOFF", std::bind( &ciLibXtract::updateRollOff, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_LOUDNESS]                 = { "XTRACT_LOUDNESS", std::bind( &ciLibXtract::updateLoudness, this ), false, SCALAR_FEATURE,
                                                { XTRACT_BARK_COEFFICIENTS } };
    
    mCallbacks[XTRACT_FLATNESS]                 = { "XTRACT_FLATNESS", std::bind( &ciLibXtract::updateFlatness, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_FLATNESS_DB]              = { "XTRACT_FLATNESS_DB", std::bind( &ciLibXtract::updateFlatnessDb, this ), false, SCALAR_FEATURE,
                                                { XTRACT_FLATNESS } };
    
    mCallbacks[XTRACT_TONALITY]                 = { "XTRACT_TONALITY", std::bind( &ciLibXtract::updateTonality, this ), false, SCALAR_FEATURE,
                                                { XTRACT_FLATNESS_DB } };
    
    mCallbacks[XTRACT_RMS_AMPLITUDE]            = { "XTRACT_RMS_AMPLITUDE", std::bind( &ciLibXtract::updateRmsAmplitude, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_SPECTRAL_INHARMONICITY]   = { "XTRACT_SPECTRAL_INHARMONICITY", std::bind( &ciLibXtract::updateSpectralInharmonicity, this ), false, SCALAR_FEATURE,
                                                { XTRACT_PEAK_SPECTRUM, XTRACT_F0 } };
    
    mCallbacks[XTRACT_POWER]                    = { "XTRACT_POWER", std::bind( &ciLibXtract::updatePower, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_ODD_EVEN_RATIO]           = { "XTRACT_ODD_EVEN_RATIO", std::bind( &ciLibXtract::updateOddEvenRatio, this ), false, SCALAR_FEATURE,
                                                { XTRACT_HARMONIC_SPECTRUM } };
    
    mCallbacks[XTRACT_SHARPNESS]                = { "XTRACT_SHARPNESS", std::bind( &ciLibXtract::updateSharpness, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_SPECTRAL_SLOPE]           = { "XTRACT_SPECTRAL_SLOPE", std::bind( &ciLibXtract::updateSpectralSlope, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_LOWEST_VALUE]             = { "XTRACT_LOWEST_VALUE", std::bind( &ciLibXtract::updateLowestValue, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_HIGHEST_VALUE]            = { "XTRACT_HIGHEST_VALUE", std::bind( &ciLibXtract::updateHighestValue, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_SUM]                      = { "XTRACT_SUM", std::bind( &ciLibXtract::updateSum, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_NONZERO_COUNT]            = { "XTRACT_NONZERO_COUNT", std::bind( &ciLibXtract::updateNonZeroCount, this ), false, SCALAR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    mCallbacks[XTRACT_MFCC]                     = { "XTRACT_MFCC", std::bind( &ciLibXtract::updateMfcc, this ), false,  VECTOR_FEATURE,
                                                { XTRACT_SPECTRUM } };
    
    
    //    mCallbacks[XTRACT_CREST]                        = { "XTRACT_CREST", std::bind( &ciLibXtract::updateCrest, this ), false };
    //    mCallbacks[XTRACT_NOISINESS]                    = { "XTRACT_NOISINESS", std::bind( &ciLibXtract::updateNoisiness, this ), false };
//    mCallbacks[XTRACT_HPS]                          = { "XTRACT_HPS", std::bind( &ciLibXtract::updateHps, this ), false };
//    mCallbacks[XTRACT_FAILSAFE_F0]                  = { "XTRACT_FAILSAFE_F0", std::bind( &ciLibXtract::updateFailsafeF0, this ), false };
//    mCallbacks[XTRACT_LNORM]                        = { "XTRACT_LNORM", std::bind( &ciLibXtract::updateLnorm, this ), false };
//    mCallbacks[XTRACT_FLUX]                         = { "XTRACT_FLUX", std::bind( &ciLibXtract::updateFlux, this ), false };
//    mCallbacks[XTRACT_ATTACK_TIME]                  = { "XTRACT_ATTACK_TIME", std::bind( &ciLibXtract::updateAttackTime, this ), false };
//    mCallbacks[XTRACT_DECAY_TIME]                   = { "XTRACT_DECAY_TIME", std::bind( &ciLibXtract::updateDecayTime, this ), false };
//    mCallbacks[XTRACT_DIFFERENCE_VECTOR]            = { "XTRACT_DIFFERENCE_VECTOR", std::bind( &ciLibXtract::updateDifferenceVector, this ), false };
//    mCallbacks[XTRACT_AUTOCORRELATION]              = { "XTRACT_AUTOCORRELATION", std::bind( &ciLibXtract::updateAutocorrelation, this ), false };
//    mCallbacks[XTRACT_AMDF]                         = { "XTRACT_AMDF", std::bind( &ciLibXtract::updateAmdf, this ), false };
//    mCallbacks[XTRACT_ASDF]                         = { "XTRACT_ASDF", std::bind( &ciLibXtract::updateAsdf, this ), false };
//    mCallbacks[XTRACT_LPC]                          = { "XTRACT_LPC", std::bind( &ciLibXtract::updateLpc, this ), false };
//    mCallbacks[XTRACT_LPCC]                         = { "XTRACT_LPCC", std::bind( &ciLibXtract::updateLpcc, this ), false };
//    mCallbacks[XTRACT_AUTOCORRELATION_FFT]          = { "XTRACT_AUTOCORRELATION_FFT", std::bind( &ciLibXtract::updateAutoCorrelationFft, this ), false };
//    mCallbacks[XTRACT_DCT]                          = { "XTRACT_DCT", std::bind( &ciLibXtract::updateDct, this ), false };
//    mCallbacks[XTRACT_SUBBANDS]                     = { "XTRACT_SUBBANDS", std::bind( &ciLibXtract::updateSubbands, this ), false };
//    mCallbacks[XTRACT_WINDOWED]                     = { "XTRACT_WINDOWED", std::bind( &ciLibXtract::updateWindowed, this ), false };

}

void ciLibXtract::update()
{
    if ( !mInputSource )
        return;
    
    mPcmBuffer = mInputSource.getPcmBuffer();

	if( !mPcmBuffer )
		return;
    
    audio::Buffer32fRef buff = mPcmBuffer->getInterleavedData();
    
    for( size_t k=0; k < PCM_SIZE; k++ )
        mPcmData.get()[k] = buff->mData[k*2];
    
    updateCallbacks();
}


void ciLibXtract::updateCallbacks()
{
    std::map<xtract_features_,FeatureCallback>::iterator it;
    for( it = mCallbacks.begin(); it!=mCallbacks.end(); ++it )
        if ( it->second.enable )
            it->second.cb();
}


void ciLibXtract::debug()
{
    Vec2f offset = Vec2f( 515, 25 );
    
    std::map<xtract_features_,FeatureCallback>::iterator it;
    for( it = mCallbacks.begin(); it!=mCallbacks.end(); ++it )
    {
        mFontSmall->drawString( to_string( it->second.enable ) + "\t" + it->second.name, offset );
        offset += Vec2f( 0, 15 );
    }
}


void ciLibXtract::enableFeature( xtract_features_ feature )
{
    mCallbacks[feature].enable = true;
    
    vector<xtract_features_> dependencies = mCallbacks[feature].dependencies;
    for( auto k=0; k < dependencies.size(); k++ )
        enableFeature( dependencies[k] );
}


void ciLibXtract::disableFeature( xtract_features_ feature )
{
    mCallbacks[feature].enable = false;
    
    // disable all features that depends on this one
    std::map<xtract_features_,FeatureCallback>::iterator it;
    for( it = mCallbacks.begin(); it != mCallbacks.end(); ++it )
    {
        if ( featureDependsOn( it->first, feature ) )
            disableFeature( it->first );
    }
}


bool ciLibXtract::featureDependsOn( xtract_features_ this_feature, xtract_features_ test_feature )
{
    vector<xtract_features_> dependencies = mCallbacks[this_feature].dependencies;
    for( auto i=0; i < dependencies.size(); i++ )
        if ( test_feature == dependencies[i] )
            return true;
    
    return false;
}


std::shared_ptr<double> ciLibXtract::getVectorFeature( xtract_features_ feature )
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
    
    else
    {
        console() << "getVectorFeature() feature not found! " << feature << endl;
        exit(-1);
    }
//    return std::shared_ptr<double>();
}


// ------------------------------------ //
//              Callbacks               //
// ------------------------------------ //

void ciLibXtract::updateMean()
{
    double *argd = NULL;
    xtract_mean( mSpectrum.get(), PCM_SIZE >> 1, argd, &mScalarValues[XTRACT_MEAN] );
}

void ciLibXtract::updateVariance()
{
    xtract_variance( mSpectrum.get(), PCM_SIZE >> 1, &mScalarValues[XTRACT_MEAN], &mScalarValues[XTRACT_VARIANCE] );
}

void ciLibXtract::updateStandardDeviation()
{
    xtract_standard_deviation( mSpectrum.get(), PCM_SIZE >> 1, &mScalarValues[XTRACT_VARIANCE], &mScalarValues[XTRACT_STANDARD_DEVIATION] );
}

void ciLibXtract::updateAverageDeviation()
{
    xtract_average_deviation( mSpectrum.get(), PCM_SIZE >> 1, &mScalarValues[XTRACT_MEAN], &mScalarValues[XTRACT_AVERAGE_DEVIATION] );
}

void ciLibXtract::updateSkewness()
{
    double data[2] = { mScalarValues[XTRACT_MEAN], mScalarValues[XTRACT_STANDARD_DEVIATION] };
    xtract_skewness( mSpectrum.get(), PCM_SIZE >> 1, data, &mScalarValues[XTRACT_SKEWNESS] );
}

void ciLibXtract::updateKurtosis()
{
    double data[2] = { mScalarValues[XTRACT_MEAN], mScalarValues[XTRACT_STANDARD_DEVIATION] };
    xtract_kurtosis( mSpectrum.get(), PCM_SIZE >> 1, data, &mScalarValues[XTRACT_KURTOSIS] );
}

void ciLibXtract::updateSpectralMean()
{
    xtract_spectral_mean( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_SPECTRAL_MEAN] );
}

void ciLibXtract::updateSpectralVariance()
{
    xtract_spectral_variance( mSpectrum.get(), PCM_SIZE >> 1, &mScalarValues[XTRACT_SPECTRAL_MEAN], &mScalarValues[XTRACT_SPECTRAL_VARIANCE] );
}

void ciLibXtract::updateSpectralStandardDeviation()
{
    xtract_spectral_standard_deviation( mSpectrum.get(), PCM_SIZE >> 1, &mScalarValues[XTRACT_SPECTRAL_VARIANCE], &mScalarValues[XTRACT_SPECTRAL_STANDARD_DEVIATION] );
}

void ciLibXtract::updateSpectralSkewness()
{
    xtract_spectral_skewness( mSpectrum.get(), PCM_SIZE >> 1, &mScalarValues[XTRACT_SPECTRAL_MEAN], &mScalarValues[XTRACT_SPECTRAL_SKEWNESS] );
}

void ciLibXtract::updateSpectralKurtosis()
{
    double data[2] = { mScalarValues[XTRACT_SPECTRAL_MEAN], mScalarValues[XTRACT_SPECTRAL_STANDARD_DEVIATION] };
    xtract_spectral_kurtosis( mSpectrum.get(), PCM_SIZE >> 1, data, &mScalarValues[XTRACT_SPECTRAL_KURTOSIS] );
}

void ciLibXtract::updateSpectralCentroid()
{
    xtract_spectral_centroid( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_SPECTRAL_CENTROID] );
}

void ciLibXtract::updateIrregularityK()
{
    xtract_irregularity_k( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_IRREGULARITY_K] );
}

void ciLibXtract::updateIrregularityJ()
{
    xtract_irregularity_j( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_IRREGULARITY_J] );
}

void ciLibXtract::updateTristimulus1()
{
    xtract_tristimulus_1( mHarmonicSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_TRISTIMULUS_1] );
}


// TODO: add params!
void ciLibXtract::updateSmoothness()
{
    int data[3] = { 0, 10, 10 };    // lower bound, upper bound, and pre-scaling factor, whereby array data in the range lower < n < upper will be pre-scaled by p before processing.
    xtract_smoothness( mSpectrum.get(), PCM_SIZE >> 1, data, &mScalarValues[XTRACT_SMOOTHNESS] );
}

void ciLibXtract::updateSpread()
{
    xtract_spread( mSpectrum.get(), PCM_SIZE >> 1, &mScalarValues[XTRACT_SPECTRAL_CENTROID], &mScalarValues[XTRACT_SPREAD] );
}

void ciLibXtract::updateZcr()
{
    xtract_zcr( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_ZCR] );
}

// TODO: add params!
void ciLibXtract::updateRollOff()
{
    //    spectral rolloff in Hz of N values from the array pointed to by *data. This is the point in the spectrum below which argv[0] of the energy is distributed.
    double data[3] = { (double)SAMPLERATE / (double)( PCM_SIZE >> 1 ), 10 }; //  (samplerate / N ) and a double representing the threshold for rolloff
    xtract_rolloff( mSpectrum.get(), PCM_SIZE >> 1, data, &mScalarValues[XTRACT_ROLLOFF] );
}

void ciLibXtract::updateLoudness()
{
    xtract_loudness( mBarks.get(), XTRACT_BARK_BANDS, NULL, &mScalarValues[XTRACT_LOUDNESS] );
}

void ciLibXtract::updateFlatness()
{
    xtract_flatness( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_FLATNESS] );
}

void ciLibXtract::updateFlatnessDb()
{
    xtract_flatness( NULL, 0, &mScalarValues[XTRACT_FLATNESS], &mScalarValues[XTRACT_FLATNESS_DB] );
}

void ciLibXtract::updateTonality()
{
    xtract_tonality( NULL, 0, &mScalarValues[XTRACT_FLATNESS_DB], &mScalarValues[XTRACT_TONALITY] );
}

// TODO requires: maximum value in a spectrum, and a double representing the mean value of a spectrum
//void ciLibXtract::updateCrest()
//{
//    xtract_crest( NULL, 0, &mScalarValues[XTRACT_SPECTRAL_FLATNESS_DB], &mScalarValues[XTRACT_CREST] );
//}

//void ciLibXtract::updateNoisiness() {}

void ciLibXtract::updateRmsAmplitude()
{
    xtract_rms_amplitude( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_RMS_AMPLITUDE] );
}

void ciLibXtract::updateSpectralInharmonicity()
{
    xtract_spectral_inharmonicity( mPeakSpectrum.get(), PCM_SIZE >> 1, &mScalarValues[XTRACT_F0], &mScalarValues[XTRACT_SPECTRAL_INHARMONICITY] );
}

void ciLibXtract::updatePower()
{
    xtract_power( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_POWER] );
}

void ciLibXtract::updateOddEvenRatio()
{
    xtract_odd_even_ratio( mHarmonicSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_ODD_EVEN_RATIO] );
}

void ciLibXtract::updateSharpness ()
{
    xtract_sharpness( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_SHARPNESS] );
}


void ciLibXtract::updateF0()
{
    double sample_rate = SAMPLERATE;
    xtract_f0( mSpectrum.get(), PCM_SIZE >> 1, &sample_rate, &mScalarValues[XTRACT_F0] );
}

void ciLibXtract::updateSpectralSlope()
{
    xtract_spectral_slope( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_SPECTRAL_SLOPE] );
}

// TODO: add params!
void ciLibXtract::updateLowestValue()
{
    double lowerLimit = 0.2f;
    xtract_lowest_value( mSpectrum.get(), PCM_SIZE >> 1, &lowerLimit, &mScalarValues[XTRACT_LOWEST_VALUE] );
}

void ciLibXtract::updateHighestValue()
{
    xtract_highest_value( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_HIGHEST_VALUE] );
}

void ciLibXtract::updateSum()
{
    xtract_sum( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_SUM] );
}

void ciLibXtract::updateNonZeroCount()
{
    xtract_nonzero_count( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_NONZERO_COUNT] );
}


// THIS FUNCTION DOESN'T WORK PROPERLY ast stated in the LibXtract doc!!!
//void ciLibXtract::updateHps()
//{
//    xtract_hps( mSpectrum.get(), PCM_SIZE >> 1, NULL, &mScalarValues[XTRACT_HPS] );
//}


/////////////////////////////////

// Vector ///////////////////////////////////////////////////////////////////////////////

void ciLibXtract::updateSpectrum()
{
    _argd[0] = mParams["spectrum_sample_rate_N"];
    _argd[1] = mParams["spectrum_type"];
    _argd[2] = mParams["spectrum_dc"];
    _argd[3] = mParams["spectrum_norm"];
    
    xtract_spectrum( mPcmData.get(), PCM_SIZE, _argd, mSpectrum.get() );
}

void ciLibXtract::updatePeakSpectrum()
{
    double data[2] = { mParams["spectrum_sample_rate_N"], mParams["peak_spectrum_threshold"] };
    xtract_peak_spectrum( mSpectrum.get(), PCM_SIZE >> 1, data, mPeakSpectrum.get() );
}

void ciLibXtract::updateBarkCoefficients()
{
    xtract_bark_coefficients( mSpectrum.get(), PCM_SIZE >> 1, mBarkBandLimits.get(), mBarks.get() );
}

// TODO add param
void ciLibXtract::updateHarmonicSpectrum()
{
    //    F0 and a threshold (t) where 0<=t<=1.0, and t determines the distance from the nearest harmonic number within which a partial can be considered harmonic
    double data[2] = { mScalarValues[XTRACT_F0], 0.3f };
    xtract_harmonic_spectrum( mPeakSpectrum.get(), PCM_SIZE >> 1, data, mHarmonicSpectrum.get() );
}

void ciLibXtract::updateMfcc()
{
    xtract_mfcc( mSpectrum.get(), PCM_SIZE >> 1, &mMelFilters, mMfccs.get() );
}

