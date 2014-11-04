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
    
    mDataSize = CIXTRACT_FFT_SIZE;
    
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
    
    doUpdate( frameN, inputFeature->getDataRaw().get(), inputFeature->getBufferSize(), &mMelFilters, mDataRaw.get() );
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
    
    doUpdate( frameN, inputFeature->getDataRaw().get(), inputFeature->getBufferSize(), mBandLimits.get(), mDataRaw.get() );
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
