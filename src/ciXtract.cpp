/*
 *  ciXtract.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2014 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */

#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


ciXtract::ciXtract()
{
	mPcmDataRaw = std::shared_ptr<double>( new double[ CIXTRACT_PCM_SIZE ] );
	mPcmData    = std::shared_ptr<double>( new double[ CIXTRACT_PCM_SIZE ] );
    
    for( size_t k=0; k < CIXTRACT_PCM_SIZE; k++ )
        mPcmData.get()[k] = 0.0f;
    
    mWindowFunc = NULL;
    
    initFeatures();
}


ciXtract::~ciXtract()
{
    mFeatures.clear();
    
    xtract_free_fft();
    xtract_free_window( mWindowFunc );
}


void ciXtract::initFeatures()
{
    // init the window function
    // FFT assumes that the source of the transform represents a periodic waveform
    // The windowing function is a function applied to a signal prior to taking an FFT
    // in order to attempt to reduce some of the artefacts introduced by the assumptions of the FFT
    mWindowFunc = xtract_init_window( CIXTRACT_PCM_SIZE, XTRACT_HANN );

    
    // init Vector features
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpectrum>( this, "Spectrum" ) );
//    mFeatures.push_back( ciXtractFeature::create<ciXtractAutocorrelationFft>( this, "Auto Correlation Fft" ) ); // doesn't work, never will!
    mFeatures.push_back( ciXtractFeature::create<ciXtractMfcc>( this, "Mfcc" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractDct>( this, "Dct" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractAutocorrelation>( this, "Auto Correlation" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractAmdf>( this, "Amdf" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractAsdf>( this, "Asdf" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractBark>( this, "Bark" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractPeakSpectrum>( this, "Peak Spectrum" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractHarmonicSpectrum>( this, "Harmonic Spectrum" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractLpc>( this, "Lpc" ) );
    //    lpcc
    mFeatures.push_back( ciXtractFeature::create<ciXtractSubBands>( this, "Sub Bands" ) );
    
    // init Scalar features
    
    mFeatures.push_back( ciXtractFeature::create<ciXtractMean>( this, "Mean" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractVariance>( this, "Variance" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractStandardDeviation>( this, "Standard Deviation" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractAverageDeviation>( this, "Average Deviation" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSkewness>( this, "Skewness" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractKurtosis>( this, "Kurtosis" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpectralMean>( this, "Spectral Mean" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpectralVariance>( this, "Spectral Variance" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpectralStandardDeviation>( this, "Spectral Standard Deviation" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpectralSkewness>( this, "Spectral Skewness" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpectralKurtosis>( this, "Spectral Kurtosis" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpectralCentroid>( this, "Spectral Centroid" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractIrregularityK>( this, "Irregularity K" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractIrregularityJ>( this, "Irregularity J" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractTristimulus1>( this, "Tristimulus 1" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSmoothness>( this, "Smoothness" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpread>( this, "Spread" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractZcr>( this, "Zcr" ) );
//    mFeatures.push_back( ciXtractFeature::create<ciXtractRolloff>( this, "Rolloff" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractLoudness>( this, "Loudness" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractFlatness>( this, "Flatness" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractFlatnessDb>( this, "Flatness Db" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractTonality>( this, "Tonality" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractRmsAmplitude>( this, "RMS Amplitude" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpectralInharmonicity>( this, "Spectral Inhamornicity" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractPower>( this, "Power" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractOddEvenRatio>( this, "Odd Even Ratio" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSharpness>( this, "Sharpness" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpectralSlope>( this, "Spectral Slope" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractLowestValue>( this, "Lowest Value" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractHighestValue>( this, "Highest Value" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSum>( this, "Sum" ) );
    // hps
    mFeatures.push_back( ciXtractFeature::create<ciXtractF0>( this, "F0" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractFailsafeF0>( this, "Failsafe F0" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractWaveletF0>( this, "Wavelet F0" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractNonZeroCount>( this, "Non-Zero Count" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractCrest>( this, "Crest" ) );

}


void ciXtract::update( const float *pcmData, float pcmGain )
{
    // convert float to double (can't use memcpy)
    for( size_t k=0; k < CIXTRACT_PCM_SIZE; k++ )
        mPcmDataRaw.get()[k] = pcmData[k] * pcmGain;
    
    xtract_windowed( mPcmDataRaw.get(), CIXTRACT_PCM_SIZE, mWindowFunc, mPcmData.get() );

    vector<ciXtractFeatureRef>::iterator    it;
    
    int frameN = getElapsedFrames();
    
    for( it = mFeatures.begin(); it!=mFeatures.end(); ++it )
        if ( (*it)->isEnable() )
            (*it)->update( frameN );
}

// TODO: if there is a missing dep the feature should be disable <<<<<<<<<<<<
bool ciXtract::enableFeature( xtract_features_ feature )
{
    ciXtractFeatureRef f = getFeature( feature );
    
    if ( !f )
        return false;

    vector<xtract_features_> dependencies = f->mDependencies;
    for( size_t k=0; k < dependencies.size(); k++ )
        if ( dependencies[k] < XTRACT_FEATURES )                // ignore PCM and NO_FEATURE enums
            if ( !enableFeature( dependencies[k] ) )
                return false;
    
    f->mIsEnable = true;
    
    return true;
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
    for( size_t i=0; i < dependencies.size(); i++ )
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
    
    console() << "ciXtract::getFeature(): Feature NOT found! " << xtract_features_names[feature] << endl;
    
    return ciXtractFeatureRef();
}


void ciXtract::listFeatures()
{
    console() << endl << "--- ciXtract available features ---" << endl;
    
    vector<ciXtractFeatureRef>::iterator it;
    for( it = mFeatures.begin(); it != mFeatures.end(); ++it )
        console() << xtract_features_names[(*it)->getEnum()] << endl;

    console() << "-----------------------------------" << endl << endl;
}

