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


ciXtractFeature::ciXtractFeature( ciXtract *xtract, xtract_features_ featureEnum, uint32_t dataSize, uint32_t bufferSize, xtract_features_ inputFeature, std::vector<xtract_features_> extraDependencies )
: mXtract(xtract), mFeatureEnum(featureEnum), mDataSize(dataSize), mBufferDataSize(bufferSize), mInputFeatureEnum(inputFeature)
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
    mData               = DataBuffer( new double[mBufferDataSize] );
    mDataRaw            = DataBuffer( new double[mBufferDataSize] );
    
    for( uint32_t k=0; k < mBufferDataSize; k++ )
    {
        mData.get()[k]      = 0.0;
        mDataRaw.get()[k]   = 0.0;
    }
    
//    mParams.resize(4);      // features have at the most 4 args
    
//    console() << "ciXtractFeature() " << getName() << " | ";
//    for( auto k=0; k < mDependencies.size(); k++ )
//        console() << mDependencies[k] << " ";
//    console() << endl;
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
    if ( mParams.empty() )
        mArgd[0] = NULL;
    
    for( size_t k=0; k < mParams.size(); k++ )
        mArgd[k] = mParams[k] ? mParams[k]->getValue() : NULL;
}



// ------------------------------------------------------------------------------------------------ //
// Scalar features -------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //

// Mean
// ------------------------------------------------------------------------------------------------ //
ciXtractMean::ciXtractMean( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_MEAN, 1, 1, XTRACT_SPECTRUM ) {}

// Variance
// ------------------------------------------------------------------------------------------------ //
ciXtractVariance::ciXtractVariance( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_VARIANCE, 1, 1, XTRACT_SPECTRUM, std::vector<xtract_features_>(XTRACT_MEAN) ) {}


// Standard Deviation
// ------------------------------------------------------------------------------------------------ //
ciXtractStandardDeviation::ciXtractStandardDeviation( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_STANDARD_DEVIATION, 1, 1, XTRACT_SPECTRUM ) {}

// Average Deviation
// ------------------------------------------------------------------------------------------------ //

ciXtractAverageDeviation::ciXtractAverageDeviation( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_AVERAGE_DEVIATION, 1, 1, XTRACT_SPECTRUM ) {}


/*
// Skewness
// ------------------------------------------------------------------------------------------------ //
class ciXtractSkewness : public ciXtractFeature {
public:
    ciXtractSkewness( ciXtract *xtract );
    ~ciXtractSkewness() {}
};

// Kurtosis
// ------------------------------------------------------------------------------------------------ //
class ciXtractKurtosis : public ciXtractFeature {
public:
    ciXtractKurtosis( ciXtract *xtract );
    ~ciXtractKurtosis() {}
};


// Spectral Mean
// ------------------------------------------------------------------------------------------------ //
class ciXtractSpectralMean : public ciXtractFeature {
public:
    ciXtractSpectralMean( ciXtract *xtract );
    ~ciXtractSpectralMean() {}
};

// Spectral Variance
// ------------------------------------------------------------------------------------------------ //
class ciXtractSpectralVariance : public ciXtractFeature {
public:
    ciXtractSpectralVariance( ciXtract *xtract );
    ~ciXtractSpectralVariance() {}
};

// Spectral Standard Deviation
// ------------------------------------------------------------------------------------------------ //
class ciXtractSpectralStandardDeviation : public ciXtractFeature {
public:
    ciXtractSpectralStandardDeviation( ciXtract *xtract );
    ~ciXtractSpectralStandardDeviation() {}
};

// Spectral Skewness
// ------------------------------------------------------------------------------------------------ //
class ciXtractSpectralSkewness : public ciXtractFeature {
public:
    ciXtractSpectralSkewness( ciXtract *xtract );
    ~ciXtractSpectralSkewness() {}
};

// Spectral Kurtosis
// ------------------------------------------------------------------------------------------------ //
class ciXtractSpectralKurtosis : public ciXtractFeature {
public:
    ciXtractSpectralKurtosis( ciXtract *xtract );
    ~ciXtractSpectralKurtosis() {}
};

// Spectral Centroid
class ciXtractSpectralCentroid : public ciXtractFeature {
public:
    ciXtractSpectralCentroid( ciXtract *xtract );
    ~ciXtractSpectralCentroid() {}
};


PARAMS and ARGS can shared a pointer so there is no need to update the args array on each update, this also would help to pass other scalar features as parameters. 
in this way all the features can share exactly the same update function!
*/

// ------------------------------------------------------------------------------------------------ //
// Vector features -------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //

// Spectrum --------------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectrum::ciXtractSpectrum( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_SPECTRUM, CIXTRACT_FFT_SIZE, CIXTRACT_FFT_SIZE * 2 )
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

void ciXtractSpectrum::update( int frameN )
{
    updateArgs();
    
    doUpdate( frameN, mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
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


// Bark ------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------ //

ciXtractBark::ciXtractBark( ciXtract *xtract ) : ciXtractFeature( xtract, XTRACT_BARK_COEFFICIENTS, XTRACT_BARK_BANDS, XTRACT_BARK_BANDS, XTRACT_SPECTRUM )
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

//Extract Linear Predictive Coding Cepstral Coefficients.
//
//Parameters
//*data
//a pointer to the first element in an array of LPC coeffiecients e.g. a pointer to the second half of the array pointed to by *result from xtract_lpc()
//
//N
//the number of LPC coefficients to be considered
//
//*argv
//a pointer to a double representing the order of the result vector.
//This must be a whole number. According to Rabiner and Juang the ratio between the number (p) of LPC coefficients and the order (Q) of the LPC cepstrum is given by Q ~ (3/2)p where Q > p.
//
//*result
//a pointer to an array containing the resultant LPCC.
//
//
//An array of size Q, where Q is given by argv[0] must be allocated, and *result must point to its first element.




//int 	xtract_subbands (const double *data, const int N, const void *argv, double *result)


