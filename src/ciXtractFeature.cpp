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
    
    mParams.resize(4);      // features have at the most 4 args
    
    console() << "ciXtractFeature() " << getName() << " | ";
    for( auto k=0; k < mDependencies.size(); k++ )
        console() << mDependencies[k] << " ";
    console() << endl;
}


void ciXtractFeature::update( int frameN )
{    
    ciXtractFeatureRef inputFeature = mXtract->getActiveFeature( mInputFeatureEnum );
    
    if ( !inputFeature )
        return;
    
    updateArgs();
    
    doUpdate( frameN, inputFeature->getDataRaw().get(), inputFeature->getDataSize(), mArgd, mDataRaw.get() );
}


void ciXtractFeature::doUpdate( int frameN, const double *inputData, const int inputDataSize, const void *args, double *outputData )
{
    if ( !isReady( frameN ) )
        return;
    
    xtract[mFeatureEnum]( inputData, inputDataSize, args, outputData );
    
    processData();
    
    mLastUpdateAt = frameN;
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


void ciXtractFeature::updateArgs()
{
    for( size_t k=0; k < mParams.size(); k++ )
        mArgd[k] = mParams[k] ? mParams[k]->getValue() : NULL;
}

// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// ---------------------------------------------- FEATURES ---------------------------------------- //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //

// TODO: mean, varianc ... kurtosis, check whether they process the spectrum or pcm!!!

// Mean ------------------------------------------------------------------------------------------- //
ciXtractMean::ciXtractMean( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_MEAN, 1, XTRACT_SPECTRUM ) {}

// variance --------------------------------------------------------------------------------------- //
ciXtractVariance::ciXtractVariance( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_VARIANCE, 1, XTRACT_SPECTRUM, std::vector<xtract_features_>( XTRACT_MEAN ) ) {}

void ciXtractVariance::update( int frameN )
{
    ciXtractFeatureRef meanFeature = mXtract->getActiveFeature( XTRACT_MEAN );
    
    if ( !meanFeature )
        return;
    
    mArgd[0] = meanFeature->getValue(0);

    update(frameN);
}

// Standard Deviation ----------------------------------------------------------------------------- //
ciXtractStandardDeviation::ciXtractStandardDeviation( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_STANDARD_DEVIATION, 1, XTRACT_SPECTRUM ) {}

// Average Deviation ------------------------------------------------------------------------------ //
ciXtractAverageDeviation::ciXtractAverageDeviation( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_AVERAGE_DEVIATION, 1, XTRACT_SPECTRUM ) {}

// Skewness --------------------------------------------------------------------------------------- //
ciXtractSkewness::ciXtractSkewness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SKEWNESS, 1, XTRACT_SPECTRUM ) {}

// Kurtosis --------------------------------------------------------------------------------------- //
ciXtractKurtosis::ciXtractKurtosis( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_KURTOSIS, 1, XTRACT_SPECTRUM ) {}



// Spectral Mean ---------------------------------------------------------------------------------- //
ciXtractSpectralMean::ciXtractSpectralMean( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_MEAN, 1, XTRACT_SPECTRUM ) {}

// Spectral Variance ------------------------------------------------------------------------------ //
ciXtractSpectralVariance::ciXtractSpectralVariance( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_VARIANCE, 1, XTRACT_SPECTRUM ) {}

// Spectral Standard Deviation -------------------------------------------------------------------- //
ciXtractSpectralStandardDeviation::ciXtractSpectralStandardDeviation( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_STANDARD_DEVIATION, 1, XTRACT_SPECTRUM ) {}

// Spectral Skewness ------------------------------------------------------------------------------ //
ciXtractSpectralSkewness::ciXtractSpectralSkewness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_SKEWNESS, 1, XTRACT_SPECTRUM ) {}

// Spectral Kurtosis ------------------------------------------------------------------------------ //
ciXtractSpectralKurtosis::ciXtractSpectralKurtosis( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_KURTOSIS, 1, XTRACT_SPECTRUM ) {}

// Spectral Centroid ------------------------------------------------------------------------------ //
ciXtractSpectralCentroid::ciXtractSpectralCentroid( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRAL_CENTROID, 1, XTRACT_SPECTRUM ) {}

// Irregularity_k --------------------------------------------------------------------------------- //
ciXtractIrregularityK::ciXtractIrregularityK( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_IRREGULARITY_K, 1, XTRACT_SPECTRUM ) {}

// Irregularity_j --------------------------------------------------------------------------------- //
ciXtractIrregularityJ::ciXtractIrregularityJ( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_IRREGULARITY_J, 1, XTRACT_SPECTRUM ) {}


// Tristimulus_1
ciXtractTristimulus1::ciXtractTristimulus1( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_TRISTIMULUS_1, 1, XTRACT_HARMONIC_SPECTRUM ) {}

// Smoothness
ciXtractSmoothness::ciXtractSmoothness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_, 1, XTRACT_SPECTRUM ) {}

// Spread
ciXtractSpread::ciXtractSpread( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_, 1, XTRACT_SPECTRUM ) {}

// Zcr
ciXtractZcr::ciXtractZcr( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_, 1, XTRACT_SPECTRUM ) {}

// Rolloff
ciXtractRolloff::ciXtractRolloff( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_, 1, XTRACT_SPECTRUM ) {}

// Loudness
ciXtractLoudness::ciXtractLoudness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_, 1, XTRACT_SPECTRUM ) {}

// Flatness
ciXtractFlatness::ciXtractFlatness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_, 1, XTRACT_SPECTRUM ) {}

// Flatness db
ciXtractFlatnessDb::ciXtractFlatnessDb( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_, 1, XTRACT_SPECTRUM ) {}

// Tonality
ciXtractTonality::ciXtractTonality( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_, 1, XTRACT_SPECTRUM ) {}

// Noisiness
ciXtractNoisiness::ciXtractNoisiness( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_, 1, XTRACT_SPECTRUM ) {}

// Rms Amplitude
ciXtractRmsAmplitude::ciXtractRmsAmplitude( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_, 1, XTRACT_SPECTRUM ) {}











// Spectrum --------------------------------------------------------------------------------------- //

ciXtractSpectrum::ciXtractSpectrum( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRUM, CIXTRACT_FFT_SIZE * 2 )
{
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
    updateArgs();
    
    doUpdate( frameN, mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
}


// Bark ------------------------------------------------------------------------------------------- //

ciXtractBark::ciXtractBark( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_BARK_COEFFICIENTS, XTRACT_BARK_BANDS, XTRACT_SPECTRUM )
{
    mBandLimits = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );
    
    xtract_init_bark( CIXTRACT_FFT_SIZE, CIXTRACT_SAMPLERATE, mBandLimits.get() );
}

void ciXtractBark::update( int frameN )
{
    ciXtractFeatureRef inputFeature = mXtract->getActiveFeature( mInputFeatureEnum );
    
    if ( !inputFeature )
        return;
    
    doUpdate( frameN, inputFeature->getDataRaw().get(), inputFeature->getDataSize(), mBandLimits.get(), mDataRaw.get() );
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
    ciXtractFeatureRef inputFeature = mXtract->getActiveFeature( mInputFeatureEnum );
    
    if ( !inputFeature )
        return;
    
    doUpdate( frameN, inputFeature->getDataRaw().get(), inputFeature->getDataSize(), &mMelFilters, mDataRaw.get() );
}



/*
// Autocorrelation                                                                                  //
// ------------------------------------------------------------------------------------------------ //
ciXtractAutocorrelation::ciXtractAutocorrelation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AUTOCORRELATION, name, CI_XTRACT_VECTOR, CIXTRACT_FFT_SIZE, CIXTRACT_PCM_SIZE )
{
    mEnumStr    = "XTRACT_AUTOCORRELATION";
    mDataInput  = mXtract->getPcmData();
}

void ciXtractAutocorrelation::update()
{
    xtract_autocorrelation( mDataInput.get(), CIXTRACT_PCM_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// AutoCorrelationFft                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractAutocorrelationFft::ciXtractAutocorrelationFft( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AUTOCORRELATION_FFT, name, CI_XTRACT_VECTOR, CIXTRACT_FFT_SIZE, CIXTRACT_PCM_SIZE )
{
    mEnumStr    = "XTRACT_AUTOCORRELATION_FFT";
    mDataInput  = mXtract->getPcmData();
    
    xtract_init_fft( CIXTRACT_PCM_SIZE, XTRACT_AUTOCORRELATION_FFT );
}

void ciXtractAutocorrelationFft::update()
{
    xtract_autocorrelation_fft( mDataInput.get(), CIXTRACT_PCM_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}


// Harmonic Spectrum                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractHarmonicSpectrum::ciXtractHarmonicSpectrum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_HARMONIC_SPECTRUM, name, CI_XTRACT_VECTOR, CIXTRACT_FFT_SIZE, CIXTRACT_PCM_SIZE )
{
	mDependencies.push_back( XTRACT_F0 );

    mEnumStr                = "XTRACT_HARMONIC_SPECTRUM";
    mDataInput              = mXtract->getPcmData();
    mParams["threshold"]    = ciXtractFeature::createFeatureParam( 0.3f, CI_XTRACT_PARAM_DOUBLE, std::map<std::string,double>() );
}

void ciXtractHarmonicSpectrum::update()
{
    mArgd[0] = *mXtract->getFeatureResultsRaw(XTRACT_F0).get();
    mArgd[1] = mParams["threshold"].val;
    xtract_harmonic_spectrum( mDataInput.get(), CIXTRACT_PCM_SIZE, mArgd, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Peak Spectrum                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractPeakSpectrum::ciXtractPeakSpectrum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_PEAK_SPECTRUM, name, CI_XTRACT_VECTOR, CIXTRACT_FFT_SIZE, CIXTRACT_PCM_SIZE )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr                = "XTRACT_PEAK_SPECTRUM";
    mDataInput              = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
    mParams["threshold"]    = ciXtractFeature::createFeatureParam( 0.0f, CI_XTRACT_PARAM_DOUBLE, std::map<std::string,double>() );
}

void ciXtractPeakSpectrum::update()
{
    mArgd[0] = CIXTRACT_SAMPLERATE_N;
    mArgd[1] = mParams["threshold"].val;
    xtract_peak_spectrum( mDataInput.get(), CIXTRACT_FFT_SIZE, mArgd, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Bark
// ------------------------------------------------------------------------------------------------ //
ciXtractBark::ciXtractBark( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_BARK_COEFFICIENTS, name, CI_XTRACT_VECTOR, XTRACT_BARK_BANDS )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr                = "XTRACT_BARK_COEFFICIENTS";
    mDataInput              = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
    mBandLimits             = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );
    mParams["threshold"]    = ciXtractFeature::createFeatureParam( 0.0f, CI_XTRACT_PARAM_DOUBLE, std::map<std::string,double>() );
    
    xtract_init_bark( CIXTRACT_FFT_SIZE, CIXTRACT_SAMPLERATE >> 1, mBandLimits.get() );
}

void ciXtractBark::update()
{
    mArgd[0]    = CIXTRACT_SAMPLERATE_N;
    mArgd[1]    = mParams["threshold"].val;
    xtract_bark_coefficients( mDataInput.get(), CIXTRACT_FFT_SIZE, mBandLimits.get(), mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Mfcc
// ------------------------------------------------------------------------------------------------ //
ciXtractMfcc::ciXtractMfcc( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_MFCC, name, CI_XTRACT_VECTOR, CIXTRACT_MFCC_FREQ_BANDS )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr                    = "XTRACT_MFCC";
    mDataInput                  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
    mMelFilters.n_filters       = CIXTRACT_MFCC_FREQ_BANDS;
    mMelFilters.filters         = (double **)malloc(CIXTRACT_MFCC_FREQ_BANDS * sizeof(double *));
    for( int n = 0; n < CIXTRACT_MFCC_FREQ_BANDS; ++n )
        mMelFilters.filters[n] = (double *)malloc(CIXTRACT_PCM_SIZE * sizeof(double));
    
    xtract_init_mfcc( CIXTRACT_FFT_SIZE, CIXTRACT_SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, CIXTRACT_MFCC_FREQ_MIN, CIXTRACT_MFCC_FREQ_MAX, mMelFilters.n_filters, mMelFilters.filters );
}

ciXtractMfcc::~ciXtractMfcc()
{
    for( auto n = 0; n < CIXTRACT_MFCC_FREQ_BANDS; ++n )
        free( mMelFilters.filters[n] );
    free( mMelFilters.filters );
}

void ciXtractMfcc::update()
{
    xtract_mfcc( mDataInput.get(), CIXTRACT_FFT_SIZE, &mMelFilters, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

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


// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //
// *************************************** SCALAR FEATURES **************************************** //
// ------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------ //

// F0                                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractF0::ciXtractF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_F0, name, CI_XTRACT_SCALAR )
{
    mEnumStr    = "XTRACT_F0";
    mDataInput  = mXtract->getPcmData();
}

void ciXtractF0::update()
{
    mArgd[0] = CIXTRACT_SAMPLERATE;
    xtract_f0( mDataInput.get(), CIXTRACT_PCM_SIZE, mArgd, mResultsRaw.get() );
}

// Failsafe F0                                                                                      //
// ------------------------------------------------------------------------------------------------ //

ciXtractFailsafeF0::ciXtractFailsafeF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FAILSAFE_F0, name, CI_XTRACT_SCALAR )
{
    mEnumStr    = "XTRACT_FAILSAFE_F0";
    mDataInput  = mXtract->getPcmData();
}

void ciXtractFailsafeF0::update()
{
    mArgd[0] = CIXTRACT_SAMPLERATE;
    xtract_failsafe_f0( mDataInput.get(), CIXTRACT_PCM_SIZE, mArgd, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Wavelet F0                                                                                       //
// ------------------------------------------------------------------------------------------------ //
ciXtractWaveletF0::ciXtractWaveletF0( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_WAVELET_F0, name, CI_XTRACT_SCALAR )
{
    mEnumStr    = "XTRACT_WAVELET_F0";
    mDataInput  = mXtract->getPcmData();
}

void ciXtractWaveletF0::update()
{
    mArgd[0] = CIXTRACT_SAMPLERATE;
    xtract_wavelet_f0( mDataInput.get(), CIXTRACT_PCM_SIZE, mArgd, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Mean                                                                                             //
// ------------------------------------------------------------------------------------------------ //
ciXtractMean::ciXtractMean( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_MEAN, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_MEAN";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractMean::update()
{
    xtract_mean( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Variance                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractVariance::ciXtractVariance( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_VARIANCE, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_MEAN );

    mEnumStr    = "XTRACT_VARIANCE";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractVariance::update()
{
    xtract_variance( mDataInput.get(), CIXTRACT_FFT_SIZE, mXtract->getFeatureResultsRaw(XTRACT_MEAN).get(), mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Standard Deviation                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractStandardDeviation::ciXtractStandardDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_STANDARD_DEVIATION, name, CI_XTRACT_SCALAR )
{	
	mDependencies.push_back( XTRACT_VARIANCE );

    mEnumStr    = "XTRACT_STANDARD_DEVIATION";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractStandardDeviation::update()
{
    xtract_standard_deviation( mDataInput.get(), CIXTRACT_FFT_SIZE, mXtract->getFeatureResultsRaw(XTRACT_VARIANCE).get(), mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Average Deviation                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractAverageDeviation::ciXtractAverageDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AVERAGE_DEVIATION, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_MEAN );

    mEnumStr    = "XTRACT_AVERAGE_DEVIATION";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractAverageDeviation::update()
{
    xtract_average_deviation( mDataInput.get(), CIXTRACT_FFT_SIZE, mXtract->getFeatureResultsRaw(XTRACT_MEAN).get(), mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Skewness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractSkewness::ciXtractSkewness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SKEWNESS, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_STANDARD_DEVIATION );

    mEnumStr    = "XTRACT_SKEWNESS";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSkewness::update()
{
    mArgd[0]    = *mXtract->getFeatureResultsRaw(XTRACT_MEAN).get();
    mArgd[1]    = *mXtract->getFeatureResultsRaw(XTRACT_STANDARD_DEVIATION).get();
    xtract_skewness( mDataInput.get(), CIXTRACT_FFT_SIZE, mArgd, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Kurtosis                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractKurtosis::ciXtractKurtosis( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_KURTOSIS, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_STANDARD_DEVIATION );

    mEnumStr    = "XTRACT_KURTOSIS";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractKurtosis::update()
{
    mArgd[0]    = *mXtract->getFeatureResultsRaw(XTRACT_MEAN).get();
    mArgd[1]    = *mXtract->getFeatureResultsRaw(XTRACT_STANDARD_DEVIATION).get();
    xtract_kurtosis( mDataInput.get(), CIXTRACT_FFT_SIZE, mArgd, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Spectral Mean                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralMean::ciXtractSpectralMean( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_MEAN, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_SPECTRAL_MEAN";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSpectralMean::update()
{
    xtract_spectral_mean( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Spectral Variance                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralVariance::ciXtractSpectralVariance( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_VARIANCE, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRAL_MEAN );

    mEnumStr    = "XTRACT_SPECTRAL_VARIANCE";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSpectralVariance::update()
{
    xtract_spectral_variance( mDataInput.get(), CIXTRACT_FFT_SIZE, mXtract->getFeatureResultsRaw(XTRACT_SPECTRAL_MEAN).get(), mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Spectral Standard Deviation                                                                      //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralStandardDeviation::ciXtractSpectralStandardDeviation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_STANDARD_DEVIATION, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRAL_VARIANCE );

    mEnumStr    = "XTRACT_SPECTRAL_STANDARD_DEVIATION";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSpectralStandardDeviation::update()
{
    xtract_spectral_standard_deviation( mDataInput.get(), CIXTRACT_FFT_SIZE, mXtract->getFeatureResultsRaw(XTRACT_SPECTRAL_VARIANCE).get(), mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Spectral Skewness                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralSkewness::ciXtractSpectralSkewness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_SKEWNESS, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRAL_MEAN );

    mEnumStr    = "XTRACT_SPECTRAL_SKEWNESS";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSpectralSkewness::update()
{
    xtract_spectral_skewness( mDataInput.get(), CIXTRACT_FFT_SIZE, mXtract->getFeatureResultsRaw(XTRACT_SPECTRAL_MEAN).get(), mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Spectral Kurtosis                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralKurtosis::ciXtractSpectralKurtosis( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_KURTOSIS, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRAL_MEAN );
	mDependencies.push_back( XTRACT_SPECTRAL_STANDARD_DEVIATION );

    mEnumStr    = "XTRACT_SPECTRAL_KURTOSIS";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSpectralKurtosis::update()
{
    mArgd[0]    = *mXtract->getFeatureResultsRaw(XTRACT_SPECTRAL_MEAN).get();
    mArgd[1]    = *mXtract->getFeatureResultsRaw(XTRACT_SPECTRAL_STANDARD_DEVIATION).get();
    xtract_spectral_kurtosis( mDataInput.get(), CIXTRACT_FFT_SIZE, mArgd, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Spectral Centroid                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralCentroid::ciXtractSpectralCentroid( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_CENTROID, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_SPECTRAL_CENTROID";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSpectralCentroid::update()
{  
    xtract_spectral_centroid( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Irregularity K                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractIrregularityK::ciXtractIrregularityK( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_IRREGULARITY_K, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_IRREGULARITY_K";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractIrregularityK::update()
{
    xtract_irregularity_k( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Irregularity J                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractIrregularityJ::ciXtractIrregularityJ( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_IRREGULARITY_J, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_IRREGULARITY_J";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractIrregularityJ::update()
{
    xtract_irregularity_j( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Tristimulus 1                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractTristimulus1::ciXtractTristimulus1( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_TRISTIMULUS_1, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_HARMONIC_SPECTRUM );

    mEnumStr    = "XTRACT_TRISTIMULUS_1";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_HARMONIC_SPECTRUM);
}

void ciXtractTristimulus1::update()
{
    xtract_tristimulus_1( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Smoothness                                                                                       //
// ------------------------------------------------------------------------------------------------ //
ciXtractSmoothness::ciXtractSmoothness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SMOOTHNESS, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_SMOOTHNESS";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSmoothness::update()
{
    xtract_smoothness( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Spread                                                                                           //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpread::ciXtractSpread( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPREAD, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRAL_CENTROID );

    mEnumStr    = "XTRACT_SPREAD";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSpread::update()
{
    xtract_spread( mDataInput.get(), CIXTRACT_FFT_SIZE, mXtract->getFeatureResultsRaw(XTRACT_SPECTRAL_CENTROID).get(), mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Zcr                                                                                              //
// ------------------------------------------------------------------------------------------------ //
ciXtractZcr::ciXtractZcr( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ZCR, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_ZCR";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractZcr::update()
{
    xtract_zcr( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Rolloff                                                                                          //
// ------------------------------------------------------------------------------------------------ //
ciXtractRolloff::ciXtractRolloff( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ROLLOFF, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr                = "XTRACT_ROLLOFF";
    mDataInput              = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
    mParams["threshold"]    = ciXtractFeature::createFeatureParam( 15.0f, CI_XTRACT_PARAM_DOUBLE, std::map<std::string,double>() );
}

void ciXtractRolloff::update()
{
    mArgd[0] = CIXTRACT_SAMPLERATE_N;
    mArgd[1] = mParams["threshold"].val;
    xtract_rolloff( mDataInput.get(), CIXTRACT_FFT_SIZE, mArgd, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Loudness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractLoudness::ciXtractLoudness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_LOUDNESS, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_BARK_COEFFICIENTS );

    mEnumStr    = "XTRACT_LOUDNESS";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_BARK_COEFFICIENTS);
}

void ciXtractLoudness::update()
{
    xtract_loudness( mDataInput.get(), XTRACT_BARK_BANDS, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Flatness                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractFlatness::ciXtractFlatness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FLATNESS, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_FLATNESS";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractFlatness::update()
{
    xtract_flatness( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}


// Flatness Db                                                                                      //
// ------------------------------------------------------------------------------------------------ //
ciXtractFlatnessDb::ciXtractFlatnessDb( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_FLATNESS_DB, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_FLATNESS );

    mEnumStr    = "XTRACT_FLATNESS_DB";
}

void ciXtractFlatnessDb::update()
{
    xtract_flatness_db( NULL, 0, mXtract->getFeatureResultsRaw(XTRACT_FLATNESS).get(), mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Tonality                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractTonality::ciXtractTonality( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_TONALITY, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_FLATNESS_DB );

    mEnumStr    = "XTRACT_TONALITY";
}

void ciXtractTonality::update()
{
    xtract_tonality( NULL, 0, mXtract->getFeatureResultsRaw(XTRACT_FLATNESS_DB).get(), mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// RMS Amplitude                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractRmsAmplitude::ciXtractRmsAmplitude( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_RMS_AMPLITUDE, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_RMS_AMPLITUDE";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractRmsAmplitude::update()
{
    xtract_rms_amplitude( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Spectral Inhamornicity                                                                           //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralInharmonicity::ciXtractSpectralInharmonicity( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_INHARMONICITY, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_PEAK_SPECTRUM );
	mDependencies.push_back( XTRACT_F0 );

    mEnumStr    = "XTRACT_SPECTRAL_INHARMONICITY";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_PEAK_SPECTRUM);
}

void ciXtractSpectralInharmonicity::update()
{
    xtract_spectral_inharmonicity( mDataInput.get(), CIXTRACT_FFT_SIZE, mXtract->getFeatureResultsRaw(XTRACT_F0).get(), mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Crest                                                                                            //
// ------------------------------------------------------------------------------------------------ //
ciXtractCrest::ciXtractCrest( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_CREST, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_HIGHEST_VALUE );
	mDependencies.push_back( XTRACT_MEAN );

    mEnumStr    = "XTRACT_CREST";
}

void ciXtractCrest::update()
{
    mArgd[0] = *mXtract->getFeatureResultsRaw(XTRACT_HIGHEST_VALUE).get();
    mArgd[1] = *mXtract->getFeatureResultsRaw(XTRACT_MEAN).get();
    
    xtract_crest( NULL, NULL, mArgd, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Power                                                                                            //
// ------------------------------------------------------------------------------------------------ //
ciXtractPower::ciXtractPower( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_POWER, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_POWER";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractPower::update()
{
    xtract_power( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Odd Even Ratio                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractOddEvenRatio::ciXtractOddEvenRatio( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ODD_EVEN_RATIO, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_HARMONIC_SPECTRUM );

    mEnumStr    = "XTRACT_ODD_EVEN_RATIO";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_HARMONIC_SPECTRUM);
}

void ciXtractOddEvenRatio::update()
{
    xtract_odd_even_ratio( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Sharpness                                                                                        //
// ------------------------------------------------------------------------------------------------ //
ciXtractSharpness::ciXtractSharpness( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SHARPNESS, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_SHARPNESS";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSharpness::update()
{
    xtract_sharpness( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Spectral Slope                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectralSlope::ciXtractSpectralSlope( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRAL_SLOPE, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_SPECTRAL_SLOPE";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSpectralSlope::update()
{
    xtract_spectral_slope( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Lowest Value                                                                                     //
// ------------------------------------------------------------------------------------------------ //
ciXtractLowestValue::ciXtractLowestValue( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_LOWEST_VALUE, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr                = "XTRACT_LOWEST_VALUE";
    mDataInput              = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
    mParams["lower_limit"]  = ciXtractFeature::createFeatureParam( 0.2f, CI_XTRACT_PARAM_DOUBLE, std::map<std::string,double>() );
}

void ciXtractLowestValue::update()
{
    xtract_lowest_value( mDataInput.get(), CIXTRACT_FFT_SIZE, &mParams["lower_limit"], mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Highest Value                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractHighestValue::ciXtractHighestValue( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_HIGHEST_VALUE, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_HIGHEST_VALUE";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractHighestValue::update()
{
    xtract_highest_value( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Sum                                                                                              //
// ------------------------------------------------------------------------------------------------ //
ciXtractSum::ciXtractSum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SUM, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_SUM";
    mDataInput  = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
}

void ciXtractSum::update()
{
    xtract_sum( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}

// Non-Zero Count                                                                                   //
// ------------------------------------------------------------------------------------------------ //
ciXtractNonZeroCount::ciXtractNonZeroCount( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_NONZERO_COUNT, name, CI_XTRACT_SCALAR )
{
	mDependencies.push_back( XTRACT_SPECTRUM );

    mEnumStr    = "XTRACT_NONZERO_COUNT";
}

void ciXtractNonZeroCount::update()
{
    mDataInput = mXtract->getFeatureResultsRaw(XTRACT_SPECTRUM);
    xtract_nonzero_count( mDataInput.get(), CIXTRACT_FFT_SIZE, NULL, mResultsRaw.get() );
    
    ciXtractFeature::update();
}
*/
