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

    mPcmData        = std::shared_ptr<double>( new double[ PCM_SIZE ] );
    for( size_t k=0; k < PCM_SIZE; k++ )
        mPcmData.get()[k] = 0.0f;
    
    initFeatures();
}


ciXtract::~ciXtract()
{
    // TODO clean up
    xtract_free_fft();
}


void ciXtract::initFeatures()
{
    mFeatures.push_back( ciXtractSpectrum::create( this, "Spectrum" ) );
    mFeatures.push_back( ciXtractAutocorrelation::create( this, "Auto Correlation" ) );
    // mFeatures.push_back( ciXtractAutocorrelationFft::create( this, "Auto Correlation Fft" ) );
    mFeatures.push_back( ciXtractHarmonicSpectrum::create( this, "Harmonic Spectrum" ) );    
    mFeatures.push_back( ciXtractF0::create( this, "F0" ) );
    mFeatures.push_back( ciXtractFailsafeF0::create( this, "Failsafe F0" ) );
    mFeatures.push_back( ciXtractWaveletF0::create( this, "Wavelet F0" ) );
    mFeatures.push_back( ciXtractPeakSpectrum::create( this, "Peak Spectrum" ) );
    mFeatures.push_back( ciXtractSubBands::create( this, "Sub Bands" ) );
    mFeatures.push_back( ciXtractMfcc::create( this, "Mfcc" ) );
    mFeatures.push_back( ciXtractBark::create( this, "Bark" ) );
    mFeatures.push_back( ciXtractMean::create( this, "Mean" ) );
    mFeatures.push_back( ciXtractVariance::create( this, "Variance" ) );
    mFeatures.push_back( ciXtractStandardDeviation::create( this, "Standard Deviation" ) );
    mFeatures.push_back( ciXtractAverageDeviation::create( this, "Average Deviation" ) );
    mFeatures.push_back( ciXtractSkewness::create( this, "Skewness" ) );
    mFeatures.push_back( ciXtractKurtosis::create( this, "Kurtosis" ) );
    mFeatures.push_back( ciXtractSpectralMean::create( this, "Spectral Mean" ) );
    mFeatures.push_back( ciXtractSpectralVariance::create( this, "Spectral Variance" ) );
    mFeatures.push_back( ciXtractSpectralStandardDeviation::create( this, "Spectral Standard Deviation" ) );
    mFeatures.push_back( ciXtractSpectralSkewness::create( this, "Spectral Skewness" ) );
    mFeatures.push_back( ciXtractSpectralKurtosis::create( this, "Spectral Kurtosis" ) );
    mFeatures.push_back( ciXtractSpectralCentroid::create( this, "Spectral Centroid" ) );
    mFeatures.push_back( ciXtractIrregularityK::create( this, "Irregularity K" ) );
    mFeatures.push_back( ciXtractIrregularityJ::create( this, "Irregularity J" ) );
    mFeatures.push_back( ciXtractTristimulus1::create( this, "Tristimulus 1" ) );
    mFeatures.push_back( ciXtractSmoothness::create( this, "Smoothness" ) );
    mFeatures.push_back( ciXtractSpread::create( this, "Spread" ) );
    mFeatures.push_back( ciXtractZcr::create( this, "Zcr" ) );
    mFeatures.push_back( ciXtractRolloff::create( this, "Rolloff" ) );
    mFeatures.push_back( ciXtractLoudness::create( this, "Loudness" ) );
    mFeatures.push_back( ciXtractFlatness::create( this, "Flatness" ) );
    mFeatures.push_back( ciXtractFlatnessDb::create( this, "Flatness Db" ) );
    mFeatures.push_back( ciXtractTonality::create( this, "Tonality" ) );
    mFeatures.push_back( ciXtractRmsAmplitude::create( this, "RMS Amplitude" ) );
    mFeatures.push_back( ciXtractSpectralInharmonicity::create( this, "Spectral Inhamornicity" ) );
    mFeatures.push_back( ciXtractPower::create( this, "Power" ) );
    mFeatures.push_back( ciXtractOddEvenRatio::create( this, "Odd Even Ratio" ) );
    mFeatures.push_back( ciXtractSharpness::create( this, "Sharpness" ) );
    mFeatures.push_back( ciXtractSpectralSlope::create( this, "Spectral Slope" ) );
    mFeatures.push_back( ciXtractLowestValue::create( this, "Lowest Value" ) );
    mFeatures.push_back( ciXtractHighestValue::create( this, "Highest Value" ) );
    mFeatures.push_back( ciXtractSum::create( this, "Sum" ) );
    mFeatures.push_back( ciXtractNonZeroCount::create( this, "Non-Zero Count" ) );
    mFeatures.push_back( ciXtractCrest::create( this, "Crest" ) );
}


void ciXtract::update()
{
    if ( !mInputSource )
        return;
    
    mPcmBuffer = mInputSource.getPcmBuffer();

	if( !mPcmBuffer )
		return;
    
  	audio::Buffer32fRef buff  = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
    for( size_t k=0; k < PCM_SIZE; k++ )
        mPcmData.get()[k] = buff->mData[k];
    
    vector<ciXtractFeatureRef>::iterator    it;
    
    for( it = mFeatures.begin(); it!=mFeatures.end(); ++it )
        if ( (*it)->isEnable() )
            (*it)->update();
            
    if ( isCalibrating() )
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
    
    console() << "ciXtract::getFeature FEATURE NOT FOUND! " << feature << endl;
    
    return ciXtractFeatureRef();
}


void ciXtract::calibrateFeatures()
{
    vector<ciXtractFeatureRef>::iterator it;
    for( it = mFeatures.begin(); it != mFeatures.end(); ++it )
        calibrateFeature( (*it) );
}


void ciXtract::calibrateFeature( ciXtractFeatureRef feature )
{
    feature->resetCalibration();
    ciXtractFeatureCalibration fc = { feature, getElapsedSeconds() };
    mCalibrationFeatures.push_back( fc );
}


void ciXtract::calibrateFeature( xtract_features_ featureEnum )
{
    ciXtractFeatureRef feature = getFeature( featureEnum );
    if ( feature )
        calibrateFeature( feature );
}


void ciXtract::updateCalibration()
{
    vector<ciXtractFeatureCalibration>::iterator it;
    for( it = mCalibrationFeatures.begin(); it!=mCalibrationFeatures.end(); )
    {
        if ( getElapsedSeconds() - it->StartedAt > CI_XTRACT_CALIBRATION_DURATION )
        {
            mCalibrationFeatures.erase( it );
            continue;
        }
        
        it->feature->calibrate();
        
        ++it;
    }
}

