/*
 *  ciXtractFeature.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */

#include "cinder/audio/Utilities.h"
#include "ciXtractFeature.h"
#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


ciXtractFeature::ciXtractFeature( ciXtract *xtract, xtract_features_ featureEnum, uint32_t resultsN, xtract_features_ inputFeature, std::vector<xtract_features_> extraDependencies )
: mXtract(xtract), mFeatureEnum(featureEnum), mDataSize(resultsN), mInputFeatureEnum(inputFeature)
{
    mDependencies       = extraDependencies;
    
    if ( inputFeature < XTRACT_FEATURES )
        mDependencies.insert( mDependencies.begin(), inputFeature );
    
    mIsEnable           = false;
    mLastUpdateAt       = -1;
    mMin                = 0.0f;
    mMax                = 1.0f;
    mGain               = 1.0f;
    mOffset             = 0.0f;
    mDamping            = 0.96f;
    mIsLog              = false;
    mData               = DataBuffer( new double[mDataSize] );
    mDataRaw            = DataBuffer( new double[mDataSize] );
    
    for( uint32_t k=0; k < mDataSize; k++ )
    {
        mData.get()[k]      = 0.0;
        mDataRaw.get()[k]   = 0.0;
    }
    
    console() << "ciXtractFeature() " << getName() << " | ";
    for( auto k=0; k < mDependencies.size(); k++ )
        console() << mDependencies[k] << " ";
    console() << endl;
}


bool ciXtractFeature::prepareUpdate( int frameN )
{
    if ( isUpdated(frameN) || !checkDependencies(frameN) )
        return false;
    
    for( size_t k=0; k < mParams.size(); k++ )
        mArgd[k] = mParams[k]->getValue();
    
    return true;
}


void ciXtractFeature::update( int frameN )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    ciXtractFeatureRef inputFeature = mXtract->getActiveFeature( mInputFeatureEnum );
    
    doUpdate( frameN, inputFeature->getDataRaw().get(), inputFeature->getDataSize(), mArgd, mDataRaw.get() );
}


void ciXtractFeature::doUpdate( int frameN )
{
    ciXtractFeatureRef inputFeature = mXtract->getActiveFeature( mInputFeatureEnum );
    
    doUpdate( frameN, inputFeature->getDataRaw().get(), inputFeature->getDataSize(), &mArgd[0], mDataRaw.get() );
}


void ciXtractFeature::doUpdate( int frameN, const double *inputData, const int inputDataSize, const void *args, double *outputData )
{
    xtract[mFeatureEnum]( inputData, inputDataSize, args, outputData );
    
    processData();
    
    mLastUpdateAt = frameN;
}

void ciXtractFeature::updateWithArgdFeatures( int frameN, vector<xtract_features_> features )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    for( size_t k=0; k < features.size(); k++ )
    {
        if ( k >= 4 )
            break;
        
        mArgd[k] = mXtract->getActiveFeature( features[k] )->getValue(0);
    }
    
    doUpdate( frameN );
}

void ciXtractFeature::updateWithPcm( int frameN )
{
    if ( !prepareUpdate(frameN) )
        return;

    doUpdate( frameN, mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
}

bool ciXtractFeature::checkDependencies( int frameN )
{
//    if( !mInputData || mInputDataSize == 0 )
//        return false;
    
    ciXtractFeatureRef dep;
    for( size_t k=0; k < mDependencies.size(); k++ )
    {
        dep = mXtract->getActiveFeature( mDependencies[k] );
        if ( !dep )
            return false;
        
        if ( !dep->isUpdated( frameN ) )
            dep->update( frameN );
    }
    return true;
}


void ciXtractFeature::processData()
{
    float val;
    
    for( size_t i=0; i < mDataSize; i++ )
    {
        // clamp min-max range
        val = ( mDataRaw.get()[i] - mMin ) / ( mMax - mMin );
        
        // this function doesn't work properly.
        // val = min( (float)(i + 25) / (float)mResultsN, 1.0f ) * 100 * log10( 1.0f + val );
        
        if ( mIsLog )
            val = 0.01f * audio::linearToDecibel( val );
        
        val = mOffset + mGain * val;
        
        val = math<float>::clamp( val, 0.0f, 1.0f );
        
        if ( mDamping > 0.0f )
        {
            if (  val >= mData.get()[i] )
                mData.get()[i] = val;
            else
                mData.get()[i] *= mDamping;
        }
        else
            mData.get()[i] = val;
    }
}



// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// ---------------------------------------------- FEATURES ---------------------------------------- //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //

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
//    updateWithArgdFeatures( frameN, mDependencies );
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
//    updateWithArgdFeatures( frameN, mDependencies );
}

// Spectral Mean ---------------------------------------------------------------------------------- //
ciXtractSpectralMean::ciXtractSpectralMean( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_MEAN, 1, XTRACT_SPECTRUM ) {}

// Spectral Variance ------------------------------------------------------------------------------ //
ciXtractSpectralVariance::ciXtractSpectralVariance( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_VARIANCE, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_SPECTRAL_MEAN) ) {}

void ciXtractSpectralVariance::update( int frameN  )
{
    updateWithArgdFeatures( frameN, mDependencies );
}

// Spectral Standard Deviation -------------------------------------------------------------------- //
ciXtractSpectralStandardDeviation::ciXtractSpectralStandardDeviation( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_STANDARD_DEVIATION, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_SPECTRAL_VARIANCE) ) {}

void ciXtractSpectralStandardDeviation::update( int frameN  )
{
    updateWithArgdFeatures( frameN, mDependencies );
}

// Spectral Skewness ------------------------------------------------------------------------------ //
ciXtractSpectralSkewness::ciXtractSpectralSkewness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_SKEWNESS, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_STANDARD_DEVIATION) ) {}

void ciXtractSpectralSkewness::update( int frameN  )
{
    updateWithArgdFeatures( frameN, mDependencies );
}

// Spectral Kurtosis ------------------------------------------------------------------------------ //
ciXtractSpectralKurtosis::ciXtractSpectralKurtosis( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_KURTOSIS, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_STANDARD_DEVIATION) ) {}

void ciXtractSpectralKurtosis::update( int frameN  )
{
    updateWithArgdFeatures( frameN, mDependencies );
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
    updateWithArgdFeatures( frameN, mDependencies );
}

// Smoothness ------------------------------------------------------------------------------------- //
ciXtractSmoothness::ciXtractSmoothness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SMOOTHNESS, 1, XTRACT_SPECTRUM ) {}

// Spread ----------------------------------------------------------------------------------------- //
ciXtractSpread::ciXtractSpread( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPREAD, 1, XTRACT_SPECTRUM, vector<xtract_features_>(XTRACT_SPECTRAL_CENTROID) ) {}

void ciXtractSpread::update( int frameN  )
{
    updateWithArgdFeatures( frameN, mDependencies );
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
    updateWithArgdFeatures( frameN, mDependencies );
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
    updateWithArgdFeatures( frameN, mDependencies );
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




// VECTOR FEATURES





// Spectrum --------------------------------------------------------------------------------------- //
ciXtractSpectrum::ciXtractSpectrum( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRUM, CIXTRACT_FFT_SIZE * 2 )
{
    mParams.resize(4);
    
    mParams[0] = FeatureParam::create( "samplerate", CIXTRACT_SAMPLERATE, FeatureParam::PARAM_READONLY );
    
    mParams[1] = FeatureParam::create( "type", XTRACT_MAGNITUDE_SPECTRUM );
    mParams[1]->addOption( "Magnitude",     XTRACT_MAGNITUDE_SPECTRUM );
    mParams[1]->addOption( "Log Magnitude", XTRACT_LOG_MAGNITUDE_SPECTRUM );
    mParams[1]->addOption( "Power",         XTRACT_POWER_SPECTRUM );
    mParams[1]->addOption( "Log Power",     XTRACT_LOG_POWER_SPECTRUM );
    
    mParams[2] = FeatureParam::create( "dc", 0.0 );
    mParams[2]->addOption( "yes",           1.0 );
    mParams[2]->addOption( "no",            0.0 );
    
    mParams[3] = FeatureParam::create( "norm", 0.0 );
    mParams[3]->addOption( "yes",           1.0 );
    mParams[3]->addOption( "no",            0.0 );
    
	xtract_init_fft( CIXTRACT_PCM_SIZE, XTRACT_SPECTRUM );
}

ciXtractSpectrum::~ciXtractSpectrum()
{
    xtract_free_fft();
}

void ciXtractSpectrum::update( int frameN )
{
    updateWithPcm( frameN );
}


// Autocorrelation Fft ---------------------------------------------------------------------------- //
ciXtractAutocorrelationFft::ciXtractAutocorrelationFft( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_AUTOCORRELATION_FFT, CIXTRACT_PCM_SIZE )
{
    xtract_init_fft( CIXTRACT_PCM_SIZE, XTRACT_AUTOCORRELATION_FFT );
}

// I need to free the fft but other features might have done it already, move it to ciXtract?!?!?!
//ciXtractAutocorrelationFft::~ciXtractSpectrum()
//{
//    xtract_free_fft();
//}

void ciXtractAutocorrelationFft::update( int frameN )
{
    updateWithPcm( frameN );
}


// Mfcc ------------------------------------------------------------------------------------------- //
ciXtractMfcc::ciXtractMfcc( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_MFCC, CIXTRACT_MFCC_FREQ_BANDS, XTRACT_SPECTRUM )
{
    mMelFilters.n_filters       = CIXTRACT_MFCC_FREQ_BANDS;
    mMelFilters.filters         = (double **)malloc(CIXTRACT_MFCC_FREQ_BANDS * sizeof(double *));
    for( int n = 0; n < CIXTRACT_MFCC_FREQ_BANDS; ++n )
        mMelFilters.filters[n] = (double *)malloc(CIXTRACT_PCM_SIZE * sizeof(double));
    
    // XTRACT_EQUAL_GAIN, XTRACT_EQUAL_AREA
    xtract_init_mfcc( CIXTRACT_FFT_SIZE, CIXTRACT_SAMPLERATE >> 1, XTRACT_EQUAL_AREA, CIXTRACT_MFCC_FREQ_MIN, CIXTRACT_MFCC_FREQ_MAX, mMelFilters.n_filters, mMelFilters.filters );
}

ciXtractMfcc::~ciXtractMfcc()
{
    for( auto n = 0; n < CIXTRACT_MFCC_FREQ_BANDS; ++n )
        free( mMelFilters.filters[n] );
    free( mMelFilters.filters );
}

void ciXtractMfcc::update( int frameN )
{
    if ( !prepareUpdate(frameN) )
        return;
    
    ciXtractFeatureRef inputFeature = mXtract->getActiveFeature( mInputFeatureEnum );
    
    doUpdate( frameN, inputFeature->getDataRaw().get(), inputFeature->getDataSize(), &mMelFilters, mDataRaw.get() );
}

// Autocorrelation -------------------------------------------------------------------------------- //
ciXtractAutocorrelation::ciXtractAutocorrelation( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_AUTOCORRELATION, CIXTRACT_PCM_SIZE ) {}

void ciXtractAutocorrelation::update( int frameN )
{
    updateWithPcm( frameN );
}


// Bark ------------------------------------------------------------------------------------------- //
ciXtractBark::ciXtractBark( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_BARK_COEFFICIENTS, XTRACT_BARK_BANDS, XTRACT_SPECTRUM )
{
    mBandLimits = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );
    
    xtract_init_bark( CIXTRACT_FFT_SIZE, CIXTRACT_SAMPLERATE, mBandLimits.get() );
}

void ciXtractBark::update( int frameN )
{
    if ( !prepareUpdate(frameN) )
        return;
    
    ciXtractFeatureRef inputFeature = mXtract->getActiveFeature( mInputFeatureEnum );
    
    doUpdate( frameN, inputFeature->getDataRaw().get(), inputFeature->getDataSize(), mBandLimits.get(), mDataRaw.get() );
}

// Peak Spectrum
ciXtractPeakSpectrum::ciXtractPeakSpectrum( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_PEAK_SPECTRUM, CIXTRACT_FFT_SIZE * 2, XTRACT_SPECTRUM )
{
    mParams.push_back( FeatureParam::create( "samplerate_n", CIXTRACT_SAMPLERATE_N ) );
    mParams.push_back( FeatureParam::create( "threshold", 0.1 ) );
}

// Harmonic Spectrum
ciXtractHarmonicSpectrum::ciXtractHarmonicSpectrum( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_HARMONIC_SPECTRUM, CIXTRACT_FFT_SIZE * 2, XTRACT_PEAK_SPECTRUM, vector<xtract_features_>(XTRACT_F0) )
{
    mParams.push_back( FeatureParam::create( "threshold", 0.1 ) );
}

void ciXtractHarmonicSpectrum::update( int frameN )
{
    if ( !prepareUpdate(frameN) )
        return;

    mArgd[0] = mXtract->getActiveFeature( XTRACT_F0 )->getValue(0);
    mArgd[1] = mParams[0]->getValue();
    
    doUpdate( frameN );
}


/*
 // Sub Bands
// ------------------------------------------------------------------------------------------------ //
ciXtractSubBands::ciXtractSubBands( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SUBBANDS, name, CI_XTRACT_VECTOR, CIXTRACT_SUBBANDS_N )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr				= "XTRACT_SUBBANDS";
    mDataInput              = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
	
	// params
	std::map<std::string,double> funcOpts;
	funcOpts["Sum"]			= XTRACT_SUM;
	funcOpts["Mean"]		= XTRACT_MEAN;

	std::map<std::string,double> scaleOps;
	scaleOps["Octave"]		= XTRACT_OCTAVE_SUBBANDS;
	scaleOps["Linear"]		= XTRACT_LINEAR_SUBBANDS;

    mParams["bin_offset"]	= ciXtractFeature::createFeatureParam( 0.0f,					CI_XTRACT_PARAM_DOUBLE, std::map<std::string,double>() );
    mParams["function"]     = ciXtractFeature::createFeatureParam( XTRACT_SUM,				CI_XTRACT_PARAM_ENUM,   funcOpts );
    mParams["scale"]        = ciXtractFeature::createFeatureParam( XTRACT_LINEAR_SUBBANDS,	CI_XTRACT_PARAM_ENUM,   scaleOps );
}

void ciXtractSubBands::update()
{
    int argd[4] = { (int)mParams["function"].val, CIXTRACT_SUBBANDS_N, (int)mParams["scale"].val, (int)mParams["bin_offset"].val };
    
    xtract_subbands( mDataInput.get(), CIXTRACT_FFT_SIZE, argd, mResultsRaw.get() );
    
    ciXtractFeature::update();
}
 
*/
