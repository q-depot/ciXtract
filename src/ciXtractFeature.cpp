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


ciXtractFeature::ciXtractFeature( ciXtract *xtract, xtract_features_ featureEnum, int dataSize, int bufferSize )
: mXtract(xtract), mFeatureEnum(featureEnum), mDataSize(dataSize), mBufferDataSize(bufferSize)
{
    if ( mBufferDataSize < 1 )
        mBufferDataSize = mDataSize;
    
    mIsEnable           = false;
    mLastUpdateAt       = -1;
    mMin                = 0.0f;
    mMax                = 1.0f;
    mGain               = 1.0f;
    mOffset             = 0.0f;
    mDamping            = 0.96f;
    mIsLog              = false;
    mData               = DataBuffer( new double[mBufferDataSize] );
    mDataRaw            = DataBuffer( new double[mBufferDataSize] );
    mInputDataSize      = 0;
    mArgdPtr            = &mArgd[0];
    
    for( uint32_t k=0; k < mBufferDataSize; k++ )
    {
        mData.get()[k]      = 0.0;
        mDataRaw.get()[k]   = 0.0;
    }
    
//    console() << "ciXtractFeature() " << getName() << " | ";
//    for( auto k=0; k < mDependencies.size(); k++ )
//        console() << mDependencies[k] << " ";
//    console() << endl;
}


void ciXtractFeature::update( int frameN )
{
    if ( isUpdated(frameN) || !checkDependencies(frameN) )  // don't update twice, check dependencies are ready
        return;
    
    updateArgs();
    
    xtract[mFeatureEnum]( mInputData.get(), mInputDataSize, mArgdPtr, mDataRaw.get() );
    
    processData();
    
    mLastUpdateAt = frameN;
}


bool ciXtractFeature::checkDependencies( int frameN )
{
    if( !mInputData || mInputDataSize == 0 )
        return false;
    
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

void ciXtractFeature::addInput( xtract_features_ feature )
{
    if ( feature < XTRACT_FEATURES )
    {
        mInputData      = mXtract->getAvailableFeature( feature )->getDataRaw();
        mInputDataSize  = mXtract->getAvailableFeature( feature )->getDataSize();
        mDependencies.push_back( feature );
    }
    else
    {
        mInputData      = mXtract->getPcmData();
        mInputDataSize  = CIXTRACT_PCM_SIZE;
    }
}


// ------------------------------------------------------------------------------------------------ //
// Scalar features -------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //

// Mean
// ------------------------------------------------------------------------------------------------ //
void ciXtractMean::init()
{
    addInput( XTRACT_SPECTRUM );
}


// Variance
// ------------------------------------------------------------------------------------------------ //
void ciXtractVariance::init()
{
    addInput( XTRACT_SPECTRUM );
    mDependencies.push_back( XTRACT_MEAN );
    
    mArgdPtr = mXtract->getAvailableFeature( XTRACT_MEAN )->getDataRaw().get();
}


// Standard Deviation
// ------------------------------------------------------------------------------------------------ //
void ciXtractStandardDeviation::init()
{
    addInput( XTRACT_SPECTRUM );
    mDependencies.push_back( XTRACT_VARIANCE );
    
    mArgdPtr = mXtract->getAvailableFeature( XTRACT_VARIANCE )->getDataRaw().get();
}

// Average Deviation
// ------------------------------------------------------------------------------------------------ //
void ciXtractAverageDeviation::init()
{
    addInput( XTRACT_SPECTRUM );
    mDependencies.push_back( XTRACT_MEAN );
    
    mArgdPtr = mXtract->getAvailableFeature( XTRACT_MEAN )->getDataRaw().get();
}


// Skewness
// ------------------------------------------------------------------------------------------------ //
void ciXtractSkewness::init()
{
    addInput( XTRACT_SPECTRUM );
    mDependencies.push_back( XTRACT_MEAN );
    mDependencies.push_back( XTRACT_STANDARD_DEVIATION );
}

void ciXtractSkewness::updateArgs()
{
    mArgd[0] = *mXtract->getActiveFeature( XTRACT_MEAN )->getDataRaw().get();
    mArgd[1] = *mXtract->getActiveFeature( XTRACT_STANDARD_DEVIATION )->getDataRaw().get();
}

// Kurtosis
// ------------------------------------------------------------------------------------------------ //
void ciXtractKurtosis::init()
{
    addInput( XTRACT_SPECTRUM );
    mDependencies.push_back( XTRACT_MEAN );
    mDependencies.push_back( XTRACT_STANDARD_DEVIATION );
}

void ciXtractKurtosis::updateArgs()
{
    mArgd[0] = *mXtract->getActiveFeature( XTRACT_MEAN )->getDataRaw().get();
    mArgd[1] = *mXtract->getActiveFeature( XTRACT_STANDARD_DEVIATION )->getDataRaw().get();
}

// Spectral Mean
void ciXtractSpectralMean::init()
{
    addInput( XTRACT_SPECTRUM );
    mArgdPtr = NULL;
}


// Spectral Variance
void ciXtractSpectralVariance::init()
{
    addInput( XTRACT_SPECTRUM );
    mDependencies.push_back( XTRACT_SPECTRAL_MEAN );
    
    mArgdPtr = mXtract->getAvailableFeature( XTRACT_SPECTRAL_MEAN )->getDataRaw().get();
}


// Spectral Standard Deviation
void ciXtractSpectralStandardDeviation::init()
{
    addInput( XTRACT_SPECTRUM );
    mDependencies.push_back( XTRACT_SPECTRAL_VARIANCE );
    
    mArgdPtr = mXtract->getAvailableFeature( XTRACT_SPECTRAL_VARIANCE )->getDataRaw().get();
}


// Spectral Skewness
void ciXtractSpectralSkewness::init()
{
    addInput( XTRACT_SPECTRUM );
    mDependencies.push_back( XTRACT_SPECTRAL_MEAN );
    mDependencies.push_back( XTRACT_SPECTRAL_STANDARD_DEVIATION );
}

void ciXtractSpectralSkewness::updateArgs()
{
    mArgd[0] = *mXtract->getActiveFeature( XTRACT_SPECTRAL_MEAN )->getDataRaw().get();
    mArgd[1] = *mXtract->getActiveFeature( XTRACT_SPECTRAL_STANDARD_DEVIATION )->getDataRaw().get();
}

// Spectral Kurtosis
void ciXtractSpectralKurtosis::init()
{
    addInput( XTRACT_SPECTRUM );
    mDependencies.push_back( XTRACT_SPECTRAL_MEAN );
    mDependencies.push_back( XTRACT_SPECTRAL_STANDARD_DEVIATION );
}

void ciXtractSpectralKurtosis::updateArgs()
{
    mArgd[0] = *mXtract->getActiveFeature( XTRACT_SPECTRAL_MEAN )->getDataRaw().get();
    mArgd[1] = *mXtract->getActiveFeature( XTRACT_SPECTRAL_STANDARD_DEVIATION )->getDataRaw().get();
}


// Spectral Centroid
void ciXtractSpectralCentroid::init()
{
    addInput( XTRACT_SPECTRUM );
    mArgdPtr = NULL;
}




// ------------------------------------------------------------------------------------------------ //
// Vector features -------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //

// Spectrum --------------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //

void ciXtractSpectrum::init()
{
    // LibXtract init
	xtract_init_fft( CIXTRACT_PCM_SIZE, XTRACT_SPECTRUM );
    
    // Input Data
    addInput( (xtract_features_)CIXTRACT_PCM_INPUT );
    
    // Params & Args
    mArgd[0] = CIXTRACT_SAMPLERATE;
    
    mParams.push_back( FeatureParam::create( "type", XTRACT_MAGNITUDE_SPECTRUM, &mArgd[1] ) );
    mParams.back()->addOption( "Magnitude",     XTRACT_MAGNITUDE_SPECTRUM );
    mParams.back()->addOption( "Log Magnitude", XTRACT_LOG_MAGNITUDE_SPECTRUM );
    mParams.back()->addOption( "Power",         XTRACT_POWER_SPECTRUM );
    mParams.back()->addOption( "Log Power",     XTRACT_LOG_POWER_SPECTRUM );
    
    mParams.push_back( FeatureParam::create( "dc", 0.0, &mArgd[2] )->addOptionBool() );
    mParams.push_back( FeatureParam::create( "norm", 0.0, &mArgd[3] )->addOptionBool() );
}

// AutocorrelationFft ----------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //
/*
ciXtractAutocorrelationFft::ciXtractAutocorrelationFft( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_AUTOCORRELATION_FFT, CIXTRACT_PCM_SIZE, CIXTRACT_PCM_SIZE ) {}

void ciXtractAutocorrelationFft::update( int frameN )
{
//    xtract[XTRACT_AUTOCORRELATION_FFT]( mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
//    
//    processData();
//    
//    mLastUpdateAt = frameN;
//    
    doUpdate( frameN, mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
}
*/
/*
// Mfcc ------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //

ciXtractMfcc::ciXtractMfcc( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_MFCC, CIXTRACT_MFCC_FREQ_BANDS, CIXTRACT_MFCC_FREQ_BANDS, XTRACT_SPECTRUM )
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

//int 	xtract_dct (const double *data, const int N, const void *argv, double *result)


// Autocorrelation -------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //

ciXtractAutocorrelation::ciXtractAutocorrelation( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_AUTOCORRELATION, CIXTRACT_PCM_SIZE, CIXTRACT_PCM_SIZE ) {}

void ciXtractAutocorrelation::update( int frameN )
{
    doUpdate( frameN, mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
}


// Amdf ------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //
ciXtractAmdf::ciXtractAmdf( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_AMDF, CIXTRACT_FFT_SIZE, CIXTRACT_FFT_SIZE, XTRACT_SPECTRUM ) {}


// Asdf ------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //
ciXtractAsdf::ciXtractAsdf( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_ASDF, CIXTRACT_FFT_SIZE, CIXTRACT_FFT_SIZE, XTRACT_SPECTRUM ) {}

*/
// Bark ------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //


void ciXtractBark::init()
{
    // LibXtract init
    mBandLimits = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );
    xtract_init_bark( CIXTRACT_FFT_SIZE, CIXTRACT_SAMPLERATE, mBandLimits.get() );

    // Input Data
    mInputData      = mXtract->getAvailableFeature( XTRACT_SPECTRUM )->getDataRaw();
    mInputDataSize  = mXtract->getAvailableFeature( XTRACT_SPECTRUM )->getDataSize();
    mDependencies.push_back( XTRACT_SPECTRUM );

    // Params & Args
    mArgdPtr = mBandLimits.get();
}

/*
//int 	xtract_peak_spectrum (const double *data, const int N, const void *argv, double *result)
//int 	xtract_harmonic_spectrum (const double *data, const int N, const void *argv, double *result)



// Lpc -------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //

ciXtractLpc::ciXtractLpc( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_LPC, 2*(CIXTRACT_PCM_SIZE-1), 2*(CIXTRACT_PCM_SIZE-1), XTRACT_AUTOCORRELATION ) {}


// Lpcc ------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //

ciXtractLpcc::ciXtractLpcc( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_LPCC, 2*(CIXTRACT_PCM_SIZE-1), 2*(CIXTRACT_PCM_SIZE-1), XTRACT_AUTOCORRELATION ) {}

void ciXtractLpcc::update( int frameN )
{
    
}
*/


//int 	xtract_subbands (const double *data, const int N, const void *argv, double *result)


