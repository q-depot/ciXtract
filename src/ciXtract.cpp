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
//    xtract_free_fft();
//    xtract_free_window( mWindowFunc );
}


void ciXtract::initFeatures()
{
    mWindowFunc = xtract_init_window( CIXTRACT_PCM_SIZE, XTRACT_HANN );
    
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpectrum>( this, "Spectrum" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractAutocorrelation>( this, "Auto Correlation" ) );
    
//    mFeatures.push_back( ciXtractFeature::create<ciXtractAutocorrelationFft>( this, "Auto Correlation Fft" ) );
    
     
//    mFeatures.push_back( ciXtractFeature::create<ciXtractHarmonicSpectrum>( this, "Harmonic Spectrum" ) );
//    mFeatures.push_back( ciXtractFeature::create<ciXtractF0>( this, "F0" ) );
//    mFeatures.push_back( ciXtractFeature::create<ciXtractFailsafeF0>( this, "Failsafe F0" ) );
//    mFeatures.push_back( ciXtractFeature::create<ciXtractWaveletF0>( this, "Wavelet F0" ) );
//    mFeatures.push_back( ciXtractFeature::create<ciXtractPeakSpectrum>( this, "Peak Spectrum" ) );
//    mFeatures.push_back( ciXtractFeature::create<ciXtractSubBands>( this, "Sub Bands" ) );
//    mFeatures.push_back( ciXtractFeature::create<ciXtractMfcc>( this, "Mfcc" ) );
//    mFeatures.push_back( ciXtractFeature::create<ciXtractBark>( this, "Bark" ) );
//    mFeatures.push_back( ciXtractFeature::create<ciXtractMean>( this, "Mean" ) );
    
/*
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
  */
    /*
    mFeatures.push_back( ciXtractFeature::create<ciXtractSmoothness>( this, "Smoothness" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractSpread>( this, "Spread" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractZcr>( this, "Zcr" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractRolloff>( this, "Rolloff" ) );
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
    mFeatures.push_back( ciXtractFeature::create<ciXtractNonZeroCount>( this, "Non-Zero Count" ) );
    mFeatures.push_back( ciXtractFeature::create<ciXtractCrest>( this, "Crest" ) );
    */
}


void ciXtract::update( const float *pcmData )
{
    for( size_t k=0; k < CIXTRACT_PCM_SIZE; k++ )
        mPcmDataRaw.get()[k] = pcmData[k];
    
    xtract_windowed( mPcmDataRaw.get(), CIXTRACT_PCM_SIZE, mWindowFunc, mPcmData.get() );
    
    
//	for( size_t k=0; k < CIXTRACT_PCM_SIZE; k++ )
//        mPcmData.get()[k] = pcmData[k];

    vector<ciXtractFeatureRef>::iterator    it;
    
    int frameN = getElapsedFrames();
    
    for( it = mFeatures.begin(); it!=mFeatures.end(); ++it )
        if ( (*it)->isEnable() )
            (*it)->update( frameN );
            
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
    for( size_t k=0; k < dependencies.size(); k++ )
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
	for( size_t k=0; k < mCalibrationFeatures.size(); k++ )
	{
		if ( getElapsedSeconds() - mCalibrationFeatures[k].StartedAt > CI_XTRACT_CALIBRATION_DURATION )
        {
            mCalibrationFeatures.erase( mCalibrationFeatures.begin() + k );
            continue;
        }
        
        mCalibrationFeatures[k].feature->calibrate();
        
        k++;
	}
}


void ciXtract::listFeatures()
{
    console() << endl << "--- ciXtract available features ---" << endl;
    
    vector<ciXtractFeatureRef>::iterator it;
    for( it = mFeatures.begin(); it != mFeatures.end(); ++it )
        console() << (*it)->getEnumStr() << endl;

    console() << "-----------------------------------" << endl << endl;
}


void ciXtract::drawPcm( ci::Rectf rect, const float *pcmData, size_t pcmSize )
{
    gl::pushMatrices();
    
    gl::translate( rect.x1, rect.y1 + rect.getHeight() * 0.5f );
    
	float   scale       = rect.getWidth() / (float)pcmSize;
    
	PolyLine<Vec2f>	leftBufferLine;
    
    gl::color( Color::gray( 0.4f ) );
	
	for( int i = 0; i < pcmSize; i++ )
    {
		float x = i * scale;
        float y = pcmData[i] * rect.getHeight() * 0.5f;
		leftBufferLine.push_back( Vec2f( x , y) );
	}
    
	gl::draw( leftBufferLine );

    gl::popMatrices();
}


void ciXtract::drawData( ciXtractFeatureRef feature, Rectf rect, bool drawRaw, ColorA plotCol, ColorA bgCol, ColorA labelCol )
{
    glPushMatrix();
    
    gl::drawString( feature->getName(), rect.getUpperLeft(), labelCol );
    
    rect.y1 += 10;
    
    std::shared_ptr<double> data    = drawRaw ? feature->getDataRaw() : feature->getData();
    float                   step    = rect.getWidth() / feature->getDataSize();
    float                   h       = rect.getHeight();
    float                   val, barY;
    
    gl::color( bgCol );
    gl::drawSolidRect( rect );
    
    gl::translate( rect.getUpperLeft() );
    
    gl::color( plotCol );
    
    glBegin( GL_QUADS );
    
    for( int i = 0; i < feature->getDataSize(); i++ )
    {
        val     = ( data.get()[i] - feature->getMin() ) / ( feature->getMax() - feature->getMin() );
        val     = math<float>::clamp( val, 0.0f, 1.0f );
        barY    = h * val;
        
        glVertex2f( i * step,           h );
        glVertex2f( ( i + 1 ) * step,   h );
        glVertex2f( ( i + 1 ) * step,   h-barY );
        glVertex2f( i * step,           h-barY );
    }
    
    glEnd();
    
    gl::popMatrices();
}
