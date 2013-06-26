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


std::map<xtract_features_,std::vector<xtract_features_>> ciLibXtract::xtract_features_dependencies =
{
    { XTRACT_MEAN,                          { XTRACT_SPECTRUM } },
    { XTRACT_VARIANCE,                      { XTRACT_MEAN } },
    { XTRACT_STANDARD_DEVIATION,            { XTRACT_VARIANCE } },
    { XTRACT_AVERAGE_DEVIATION,             { XTRACT_MEAN } },
    { XTRACT_SKEWNESS,                      { XTRACT_STANDARD_DEVIATION } },
    { XTRACT_KURTOSIS,                      { XTRACT_STANDARD_DEVIATION } },
    { XTRACT_SPECTRAL_MEAN,                 { XTRACT_SPECTRUM } },
    { XTRACT_SPECTRAL_VARIANCE,             { XTRACT_SPECTRAL_MEAN } },
    { XTRACT_SPECTRAL_STANDARD_DEVIATION,   { XTRACT_SPECTRAL_VARIANCE } },
    
    
    { XTRACT_SPECTRUM, std::vector<xtract_features_>() }
        
    
    
};


ciLibXtract::ciLibXtract( audio::Input source )
{
    mInputSource    = source;
    
    mFontSmall      = gl::TextureFont::create( Font( "Helvetica", 12 ) );
    
    init();
}


ciLibXtract::~ciLibXtract() {}


void ciLibXtract::init()
{
    xtract_init_fft( PCM_SIZE << 1, XTRACT_SPECTRUM );
    
    
// -------------- //
// --- Vector --- //
// -------------- //
    
    mPcmData    = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    mSpectrum   = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    
    for( size_t k=0; k < PCM_SIZE; k++ )
    {
        mPcmData.get()[k]   = 0.0f;
        mSpectrum.get()[k]  = 0.0f;
    }
    
    
// -------------- //
// --- Scalar --- //
// -------------- //
    
    for( size_t k=0; k < XTRACT_FEATURES; k++ )
        mScalarValues[k] = 0.0f;
        
//    mMean               = 0.0f;
//    mVariance           = 0.0f;
//    mStandardDeviation  = 0.0f;
//    mAverageDeviation   = 0.0f;
    
// -------------- //
// --- Params --- //
// -------------- //
    
//  Spectrum
    mParams["spectrum_n"]       = SAMPLERATE / (double)PCM_SIZE;
    mParams["spectrum_type"]    = XTRACT_MAGNITUDE_SPECTRUM;    // XTRACT_MAGNITUDE_SPECTRUM, XTRACT_LOG_MAGNITUDE_SPECTRUM, XTRACT_POWER_SPECTRUM, XTRACT_LOG_POWER_SPECTRUM
    mParams["spectrum_dc"]      = 0.0f;
    mParams["spectrum_norm"]    = 0.0f;
    
    
// ----------------- //
// --- Callbacks --- //
// ----------------- //
    
    mCallbacks[XTRACT_SPECTRUM]                     = { "XTRACT_SPECTRUM", std::bind( &ciLibXtract::updateSpectrum, this ), 0 };
    mCallbacks[XTRACT_MEAN]                         = { "XTRACT_MEAN", std::bind( &ciLibXtract::updateMean, this ), 0 };                          
    mCallbacks[XTRACT_VARIANCE]                     = { "XTRACT_VARIANCE", std::bind( &ciLibXtract::updateVariance, this ), 0 };
    mCallbacks[XTRACT_STANDARD_DEVIATION]           = { "XTRACT_STANDARD_DEVIATION", std::bind( &ciLibXtract::updateStandardDeviation, this ), 0 };
    mCallbacks[XTRACT_AVERAGE_DEVIATION]            = { "XTRACT_AVERAGE_DEVIATION", std::bind( &ciLibXtract::updateAverageDeviation, this ), 0 };
    mCallbacks[XTRACT_SKEWNESS]                     = { "XTRACT_SKEWNESS", std::bind( &ciLibXtract::updateSkewness, this ), 0 };
    mCallbacks[XTRACT_KURTOSIS]                     = { "XTRACT_KURTOSIS", std::bind( &ciLibXtract::updateKurtosis, this ), 0 };
    mCallbacks[XTRACT_SPECTRAL_MEAN]                = { "XTRACT_SPECTRAL_MEAN", std::bind( &ciLibXtract::updateSpectralMean, this ), 0 };
    mCallbacks[XTRACT_SPECTRAL_VARIANCE]            = { "XTRACT_SPECTRAL_VARIANCE", std::bind( &ciLibXtract::updateSpectralVariance, this ), 0 };
    mCallbacks[XTRACT_SPECTRAL_STANDARD_DEVIATION]  = { "XTRACT_SPECTRAL_STANDARD_DEVIATION", std::bind( &ciLibXtract::updateSpectralStandardDeviation, this ), 0 };

    // TODO
    /*
    
    //        XTRACT_SPECTRAL_MEAN,
    //        XTRACT_SPECTRAL_VARIANCE,
    //        XTRACT_SPECTRAL_STANDARD_DEVIATION,

    //        XTRACT_SPECTRAL_SKEWNESS,
    //        XTRACT_SPECTRAL_KURTOSIS,
    //        XTRACT_SPECTRAL_CENTROID,
    //        XTRACT_IRREGULARITY_K,
    //        XTRACT_IRREGULARITY_J,
    //        XTRACT_TRISTIMULUS_1,
    //        XTRACT_TRISTIMULUS_2,
    //        XTRACT_TRISTIMULUS_3,
    //        XTRACT_SMOOTHNESS,
    //        XTRACT_SPREAD,
    //        XTRACT_ZCR,
    //        XTRACT_ROLLOFF,
    //        XTRACT_LOUDNESS,
    //        XTRACT_FLATNESS,
    //        XTRACT_FLATNESS_DB,
    //        XTRACT_TONALITY,
    //        XTRACT_CREST,
    //        XTRACT_NOISINESS,
    //        XTRACT_RMS_AMPLITUDE,
    //        XTRACT_SPECTRAL_INHARMONICITY,
    //        XTRACT_POWER,
    //        XTRACT_ODD_EVEN_RATIO,
    //        XTRACT_SHARPNESS,
    //        XTRACT_SPECTRAL_SLOPE,
    //        XTRACT_LOWEST_VALUE,
    //        XTRACT_HIGHEST_VALUE,
    //        XTRACT_SUM,
    //        XTRACT_NONZERO_COUNT,
    //        XTRACT_HPS,
    //        XTRACT_F0,
    //        XTRACT_FAILSAFE_F0,
    //        XTRACT_LNORM,
    //        XTRACT_FLUX,
    //        XTRACT_ATTACK_TIME,
    //        XTRACT_DECAY_TIME,
    //        XTRACT_DIFFERENCE_VECTOR,
    //        XTRACT_AUTOCORRELATION,
    //        XTRACT_AMDF,
    //        XTRACT_ASDF,
    //        XTRACT_BARK_COEFFICIENTS,
    //        XTRACT_PEAK_SPECTRUM,
    
    //        XTRACT_AUTOCORRELATION_FFT,
    //        XTRACT_MFCC,
    //        XTRACT_DCT,
    //        XTRACT_HARMONIC_SPECTRUM,
    //        XTRACT_LPC,
    //        XTRACT_LPCC,
    //        XTRACT_SUBBANDS,

    //        XTRACT_WINDOWED
    
    */
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
    
    //	audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT )  // CHANNEL_FRONT_RIGHT
}


void ciLibXtract::updateCallbacks()
{
    FeatureCallback f;
    std::map<xtract_features_,FeatureCallback>::iterator it;
    for( it = mCallbacks.begin(); it!=mCallbacks.end(); ++it )
    {
        f = it->second;
        if ( f.enable )
            f.cb();
    }
}


void ciLibXtract::debug()
{
    Vec2f offset = Vec2f( 515, 15 );
    
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
    
    vector<xtract_features_> dependencies = xtract_features_dependencies[feature];
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
    vector<xtract_features_> dependencies = xtract_features_dependencies[this_feature];
    for( auto i=0; i < dependencies.size(); i++ )
        if ( test_feature == dependencies[i] )
            return true;
    
    return false;
}


std::shared_ptr<double> ciLibXtract::getVectorFeature( xtract_features_ feature )
{
    if ( feature == XTRACT_SPECTRUM )
        return mSpectrum;
    
    return std::shared_ptr<double>();
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

void ciLibXtract::updateSpectrum()
{
    _argd[0] = mParams["spectrum_n"];
    _argd[1] = mParams["spectrum_type"];
    _argd[2] = mParams["spectrum_dc"];
    _argd[3] = mParams["spectrum_norm"];
    
    xtract_spectrum( mPcmData.get(), PCM_SIZE, _argd, mSpectrum.get() );
}



/*


shared_ptr<double> ciLibXtract::getSpectrum( xtract_spectrum_ spectrumType, bool normalised )
{
    mArgd[0] = SAMPLERATE / (double)PCM_SIZE;
    mArgd[1] = spectrumType;                        //  XTRACT_MAGNITUDE_SPECTRUM, XTRACT_LOG_MAGNITUDE_SPECTRUM, XTRACT_POWER_SPECTRUM, XTRACT_LOG_POWER_SPECTRUM
    mArgd[2] = 0.f;                                 // No DC component
    mArgd[3] = normalised;                          // No Normalisation
    
    
//    if ( dumping > 0 )
//    {
//        std::shared_ptr<double> data = std::shared_ptr<double>( new double[ PCM_SIZE ] );
//        
//        xtract[XTRACT_SPECTRUM]( mPcmData.get(), PCM_SIZE, mArgd, data.get() );
//        
//        for( size_t k=0; k < FFT_SIZE; k++ )
//            if ( data.get()[k] > mSpectrum.get()[k] )
//                mSpectrum.get()[k] = data.get()[k];
//            else
//                mSpectrum.get()[k] *= dumping;
//    }
//        else
//            xtract[XTRACT_SPECTRUM]( mPcmData.get(), PCM_SIZE, mArgd, mSpectrum.get() );
    
    xtract[XTRACT_SPECTRUM]( mPcmData.get(), PCM_SIZE, mArgd, mSpectrum.get() );
    
    return mSpectrum;
}


shared_ptr<double> ciLibXtract::getAutocorrelationFft()
{
    void *argd = NULL;
    xtract_autocorrelation_fft( mPcmData.get(), PCM_SIZE, argd, mAutocorrelationFft.get() );
    return mAutocorrelationFft;
}


shared_ptr<double> ciLibXtract::getMfcc()
{
    xtract_mfcc( mSpectrum.get(), FFT_SIZE, &mel_filters, mMfccs.get() );
    
    return mMfccs;
}



shared_ptr<double> ciLibXtract::getPeakSpectrum( double threshold )
{
    mArgd[0] = SAMPLERATE / (double)FFT_SIZE;     // samplerate / N
    mArgd[1] = threshold;                                   // peak threshold as percentage of the magnitude of the maximum peak found
    mArgd[2] = 0.f;
    mArgd[3] = 0.f;
    
    xtract_peak_spectrum( mSpectrum.get(), FFT_SIZE, mArgd, mPeakSpectrum.get() );
    
    return mPeakSpectrum;
}


std::shared_ptr<double> ciLibXtract::getBarkCoefficients()
{
    xtract_bark_coefficients( mSpectrum.get(), FFT_SIZE, mBarkBandLimits.get(), mBarks.get() );

    return mBarks;
}


std::shared_ptr<double> ciLibXtract::getHarmonicSpectrum()
{
    mArgd[0] = mF0;
    mArgd[1] = 0.3f;

    xtract_harmonic_spectrum( mPeakSpectrum.get(), FFT_SIZE, mArgd, mHarmonicSpectrum.get() );

    return mHarmonicSpectrum;
}

std::shared_ptr<double> ciLibXtract::getSubBands()
{
    mArgd[0] = XTRACT_SUM;                      // XTRACT_SUM or XTRACT_MEAN
    mArgd[1] = SUB_BANDS;                       // subbands N
    mArgd[2] = XTRACT_LINEAR_SUBBANDS;          // xtract_subband_scales_ { XTRACT_OCTAVE_SUBBANDS, XTRACT_LINEAR_SUBBANDS }
    mArgd[3] = 1;                               // start point of the subbands
    
    xtract_spectral_centroid( mSpectrum.get(), FFT_SIZE, mArgd, mSubBands.get() );
    
    return mSubBands;
}


// TODO doesn't work!

double ciLibXtract::getF0()
{
    double sr = SAMPLERATE / (double)PCM_SIZE;
    xtract_f0( mPcmData.get(), PCM_SIZE, &sr, &mF0 );
    return mF0;
}


double ciLibXtract::getFailsafeF0()
{
    double sr = SAMPLERATE;
    xtract_failsafe_f0( mPcmData.get(), PCM_SIZE, &sr, &mFailsafeF0 );
    return mFailsafeF0;
}


double ciLibXtract::getSpectralCentroid()
{
    xtract_spectral_centroid( mSpectrum.get(), FFT_SIZE, NULL, &mSpectralCentroid );
    return mSpectralCentroid;
}


double ciLibXtract::getSpread()
{
    xtract_spread( mSpectrum.get(), FFT_SIZE, &mSpectralCentroid, &mSpread );
    return mSpread;
}


double ciLibXtract::getLoudness()
{
    void *argd = NULL;
    xtract_loudness( mBarks.get(), XTRACT_BARK_BANDS, argd, &mLoudness );
    return mLoudness;
}


double ciLibXtract::getFlatness()
{
    xtract_flatness( mSpectrum.get(), FFT_SIZE / 2, NULL, &mFlatness );
    return mFlatness;
}


double ciLibXtract::getFlatnessDb()
{
    xtract_flatness( NULL, 0, &mFlatness, &mFlatnessDb );
    return mFlatnessDb;
}


double ciLibXtract::getIrregularityK()
{
    void *argd = NULL;
    xtract_irregularity_k( mSpectrum.get(), FFT_SIZE, argd, &mIrregularityK );
    return mIrregularityK;
}


double ciLibXtract::getIrregularityJ()
{
    void *argd = NULL;
    xtract_irregularity_j( mSpectrum.get(), FFT_SIZE, argd, &mIrregularityJ );
    return mIrregularityJ;
}


//double ciLibXtract::getZcr()
//{
//    void *argd = NULL;
//    xtract_irregularity_j( mSpectrum.get(), FFT_SIZE, argd, &mIrregularityJ );
//    return mIrregularityJ;
//}

double ciLibXtract::getPower()
{
    xtract_power( mSpectrum.get(), FFT_SIZE, NULL, &mPower );
    return mPower;
}


double ciLibXtract::getTonality()
{
    xtract_tonality( NULL, 0, &mFlatnessDb, &mTonality );
    return mTonality;
}



bool ciLibXtract::getOnSet( float threshold, float vel, float gain )
{
    double val = gain * ( mBarks.get()[0] + mBarks.get()[1] ) / 2.0f;
    
    if ( val < threshold * 0.1f )
    {
        mBarkOnSetAvg *= vel;
        return false;
    }
    
    if( val > mBarkOnSetAvg )
    {
        mBarkOnSetAvg = val;
        return true;
    }
    
    mBarkOnSetAvg *= vel;
    return false;
//    float val = gain * mBarks.get()[1];
//    
//    if ( val < threshold * 0.1f )
//    {
//        mBarkOnSetAvg *= vel;
//        return false;
//    }
//    
//    if( val > mBarkOnSetAvg )
//    {
//        mBarkOnSetAvg = val;
//        return true;
//    }
    
//    return false;
}

*/