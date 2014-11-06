/*
 *  ciXtractVectorFeature.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2014 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#include "ciXtractVectorFeature.h"
#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


// Spectrum                                                                                         //
// ------------------------------------------------------------------------------------------------ //
ciXtractSpectrum::ciXtractSpectrum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SPECTRUM, name, (xtract_features_)CIXTRACT_PCM_FEATURE, CIXTRACT_FFT_SIZE, CIXTRACT_FFT_SIZE * 2 )
{
	std::map<std::string,double> opts;
	opts["Magnitude"]		= XTRACT_MAGNITUDE_SPECTRUM;
	opts["Log Magnitude"]	= XTRACT_LOG_MAGNITUDE_SPECTRUM;
	opts["Power"]			= XTRACT_POWER_SPECTRUM;
	opts["Log Power"]		= XTRACT_LOG_POWER_SPECTRUM;

    // params
    mParams["dc"]			= ciXtractFeature::createFeatureParam( false, CI_XTRACT_PARAM_BOOL, std::map<std::string,double>() );
    mParams["norm"]			= ciXtractFeature::createFeatureParam( false, CI_XTRACT_PARAM_BOOL, std::map<std::string,double>() );
	mParams["type"]			= ciXtractFeature::createFeatureParam( XTRACT_MAGNITUDE_SPECTRUM, CI_XTRACT_PARAM_ENUM, opts ); 
    
    
    setLog( true );
}

void ciXtractSpectrum::enable( bool isEnable )
{
    if ( !mIsInit )
        xtract_init_fft( CIXTRACT_PCM_SIZE, XTRACT_SPECTRUM );
    
    ciXtractFeature::enable(isEnable);
}

void ciXtractSpectrum::doUpdate( int frameN )
{
    mArgd[0] = CIXTRACT_SAMPLERATE_N;
    mArgd[1] = mParams["type"].val;
    mArgd[2] = mParams["dc"].val;
    mArgd[3] = mParams["norm"].val;
    
    xtract_spectrum( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}



// AutoCorrelationFft                                                                               //
// ------------------------------------------------------------------------------------------------ //
ciXtractAutocorrelationFft::ciXtractAutocorrelationFft( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AUTOCORRELATION_FFT, name, (xtract_features_)CIXTRACT_PCM_FEATURE, CIXTRACT_PCM_SIZE ) {}

void ciXtractAutocorrelationFft::enable( bool isEnable )
{
    if ( !mIsInit )
        xtract_init_fft( CIXTRACT_PCM_SIZE, XTRACT_AUTOCORRELATION_FFT );
    
    ciXtractFeature::enable(isEnable);
}

void ciXtractAutocorrelationFft::doUpdate( int frameN )
{
    xtract_autocorrelation_fft( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}

// Mfcc
// ------------------------------------------------------------------------------------------------ //
ciXtractMfcc::ciXtractMfcc( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_MFCC, name, XTRACT_SPECTRUM, CIXTRACT_MFCC_FREQ_BANDS ) {}

void ciXtractMfcc::enable( bool isEnable )
{
    if ( !mIsInit )
    {
        mMelFilters.n_filters       = CIXTRACT_MFCC_FREQ_BANDS;
        mMelFilters.filters         = (double **)malloc(CIXTRACT_MFCC_FREQ_BANDS * sizeof(double *));
        for( int n = 0; n < CIXTRACT_MFCC_FREQ_BANDS; ++n )
            mMelFilters.filters[n] = (double *)malloc(CIXTRACT_PCM_SIZE * sizeof(double));
        
        xtract_init_mfcc( CIXTRACT_PCM_SIZE, CIXTRACT_SAMPLERATE >> 1, XTRACT_EQUAL_GAIN,
                          CIXTRACT_MFCC_FREQ_MIN, CIXTRACT_MFCC_FREQ_MAX, mMelFilters.n_filters, mMelFilters.filters );
    }
    
    ciXtractFeature::enable(isEnable);
}

ciXtractMfcc::~ciXtractMfcc()
{
    for( auto n = 0; n < CIXTRACT_MFCC_FREQ_BANDS; ++n )
        free( mMelFilters.filters[n] );
    free( mMelFilters.filters );
}

void ciXtractMfcc::doUpdate( int frameN )
{
    xtract_mfcc( mInputBuffer.data.get(), mInputBuffer.dataSize, &mMelFilters, mDataRaw.get() );
}


// Dct                                                                                              //
// ------------------------------------------------------------------------------------------------ //
ciXtractDct::ciXtractDct( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_DCT, name, (xtract_features_)CIXTRACT_PCM_FEATURE, CIXTRACT_PCM_SIZE ) {}

void ciXtractDct::enable( bool isEnable )
{
    if ( !mIsInit )
        xtract_init_fft( CIXTRACT_PCM_SIZE, XTRACT_DCT );
    
    ciXtractFeature::enable(isEnable);
}

void ciXtractDct::doUpdate( int frameN )
{
    xtract_dct( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Autocorrelation                                                                                  //
// ------------------------------------------------------------------------------------------------ //
ciXtractAutocorrelation::ciXtractAutocorrelation( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AUTOCORRELATION, name, (xtract_features_)CIXTRACT_PCM_FEATURE, CIXTRACT_PCM_SIZE ) {}

void ciXtractAutocorrelation::enable( bool isEnable )
{
    if ( !mIsInit )
        xtract_init_fft( CIXTRACT_PCM_SIZE, XTRACT_AUTOCORRELATION_FFT );
    
    ciXtractFeature::enable(isEnable);
}

void ciXtractAutocorrelation::doUpdate( int frameN )
{
    xtract_autocorrelation( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Amdf                                                                                             //
// ------------------------------------------------------------------------------------------------ //

ciXtractAmdf::ciXtractAmdf( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_AMDF, name, (xtract_features_)CIXTRACT_PCM_FEATURE, CIXTRACT_PCM_SIZE ) {}

void ciXtractAmdf::doUpdate( int frameN )
{
    xtract_amdf( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Asdf                                                                                             //
// ------------------------------------------------------------------------------------------------ //
ciXtractAsdf::ciXtractAsdf( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_ASDF, name, (xtract_features_)CIXTRACT_PCM_FEATURE, CIXTRACT_PCM_SIZE ) {}

void ciXtractAsdf::doUpdate( int frameN )
{
    xtract_asdf( mInputBuffer.data.get(), mInputBuffer.dataSize, NULL, mDataRaw.get() );
}


// Bark                                                                                             //
// ------------------------------------------------------------------------------------------------ //
ciXtractBark::ciXtractBark( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_BARK_COEFFICIENTS, name, XTRACT_SPECTRUM, XTRACT_BARK_BANDS )
{
    mBandLimits             = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );
    mParams["threshold"]    = ciXtractFeature::createFeatureParam( 0.0f, CI_XTRACT_PARAM_DOUBLE, std::map<std::string,double>() );
}

void ciXtractBark::enable( bool isEnable )
{
    if ( !mIsInit )
        xtract_init_bark( CIXTRACT_FFT_SIZE, CIXTRACT_SAMPLERATE >> 1, mBandLimits.get() );
    
    ciXtractFeature::enable(isEnable);
}

void ciXtractBark::doUpdate( int frameN )
{
    mArgd[0]    = CIXTRACT_SAMPLERATE_N;
    mArgd[1]    = mParams["threshold"].val;
    xtract_bark_coefficients( mInputBuffer.data.get(), mInputBuffer.dataSize, mBandLimits.get(), mDataRaw.get() );
}


// Peak Spectrum                                                                                    //
// ------------------------------------------------------------------------------------------------ //
ciXtractPeakSpectrum::ciXtractPeakSpectrum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_PEAK_SPECTRUM, name, XTRACT_SPECTRUM, CIXTRACT_FFT_SIZE, CIXTRACT_PCM_SIZE )
{
    mParams["threshold"] = ciXtractFeature::createFeatureParam( 0.0f, CI_XTRACT_PARAM_DOUBLE, std::map<std::string,double>() );
}

void ciXtractPeakSpectrum::doUpdate( int frameN )
{
    mArgd[0] = CIXTRACT_SAMPLERATE_N;
    mArgd[1] = mParams["threshold"].val;
    xtract_peak_spectrum( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Harmonic Spectrum                                                                                //
// ------------------------------------------------------------------------------------------------ //
ciXtractHarmonicSpectrum::ciXtractHarmonicSpectrum( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_HARMONIC_SPECTRUM, name, XTRACT_PEAK_SPECTRUM, CIXTRACT_FFT_SIZE )
{
	mDependencies.push_back( XTRACT_F0 );
    
    mParams["threshold"]    = ciXtractFeature::createFeatureParam( 0.3f, CI_XTRACT_PARAM_DOUBLE, std::map<std::string,double>() );
}

void ciXtractHarmonicSpectrum::doUpdate( int frameN )
{
    mArgd[0] = *mXtract->getFeatureDataRaw(XTRACT_F0).get();
    mArgd[1] = mParams["threshold"].val;
    
    xtract_harmonic_spectrum( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Lpc
// ------------------------------------------------------------------------------------------------ //
ciXtractLpc::ciXtractLpc( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_LPC, name, XTRACT_AUTOCORRELATION, 2 * ( CIXTRACT_PCM_SIZE - 1 ) ) {}

void ciXtractLpc::doUpdate( int frameN )
{
    xtract_lpc( mInputBuffer.data.get(), mInputBuffer.dataSize, mArgd, mDataRaw.get() );
}


// Lpcc
// ------------------------------------------------------------------------------------------------ //

// Sub Bands
// ------------------------------------------------------------------------------------------------ //
ciXtractSubBands::ciXtractSubBands( ciXtract *xtract, std::string name )
: ciXtractFeature( xtract, XTRACT_SUBBANDS, name, XTRACT_SPECTRUM, CIXTRACT_SUBBANDS_N )
{
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

void ciXtractSubBands::doUpdate( int frameN )
{
    int argd[4] = { (int)mParams["function"].val, CIXTRACT_SUBBANDS_N, (int)mParams["scale"].val, (int)mParams["bin_offset"].val };
    
    xtract_subbands( mInputBuffer.data.get(), mInputBuffer.dataSize, argd, mDataRaw.get() );
}

