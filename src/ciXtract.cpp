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
	mPcmData    = std::shared_ptr<double>( new double[ CIXTRACT_PCM_SIZE ] );
	mPcmDataRaw = std::shared_ptr<double>( new double[ CIXTRACT_PCM_SIZE ] );
    for( size_t k=0; k < CIXTRACT_PCM_SIZE; k++ )
    {
        mPcmData.get()[k]       = 0.0f;
        mPcmDataRaw.get()[k]    = 0.0f;
    }
    
    mWindowFunc = NULL;
    
    mLastUpdateAt = -1;
    
    initFeatures();
}


ciXtract::~ciXtract()
{
    xtract_free_window( mWindowFunc );
}


void ciXtract::initFeatures()
{
    mActiveFeatures.clear();
    
    // create the window function
    mWindowFunc = xtract_init_window( CIXTRACT_PCM_SIZE, XTRACT_HANN );
    
    // Vector
    mAvailableFeatures[ XTRACT_SPECTRUM ]                       = ciXtractFeature::create<ciXtractSpectrum>( this );
    mAvailableFeatures[ XTRACT_AUTOCORRELATION_FFT ]            = ciXtractFeature::create<ciXtractAutocorrelationFft>( this );
    mAvailableFeatures[ XTRACT_MFCC ]                           = ciXtractFeature::create<ciXtractMfcc>( this );
    mAvailableFeatures[ XTRACT_AUTOCORRELATION ]                = ciXtractFeature::create<ciXtractAutocorrelation>( this );
    mAvailableFeatures[ XTRACT_BARK_COEFFICIENTS ]              = ciXtractFeature::create<ciXtractBark>( this );
    mAvailableFeatures[ XTRACT_PEAK_SPECTRUM ]                  = ciXtractFeature::create<ciXtractPeakSpectrum>( this );
    mAvailableFeatures[ XTRACT_HARMONIC_SPECTRUM ]              = ciXtractFeature::create<ciXtractHarmonicSpectrum>( this );
    
    // Scalar
    mAvailableFeatures[ XTRACT_MEAN ]                           = ciXtractFeature::create<ciXtractMean>( this );
    mAvailableFeatures[ XTRACT_VARIANCE ]                       = ciXtractFeature::create<ciXtractVariance>( this );
    mAvailableFeatures[ XTRACT_STANDARD_DEVIATION ]             = ciXtractFeature::create<ciXtractStandardDeviation>( this );
    mAvailableFeatures[ XTRACT_AVERAGE_DEVIATION ]              = ciXtractFeature::create<ciXtractAverageDeviation>( this );
    mAvailableFeatures[ XTRACT_SKEWNESS ]                       = ciXtractFeature::create<ciXtractSkewness>( this );
    mAvailableFeatures[ XTRACT_KURTOSIS ]                       = ciXtractFeature::create<ciXtractKurtosis>( this );
    mAvailableFeatures[ XTRACT_SPECTRAL_MEAN ]                  = ciXtractFeature::create<ciXtractSpectralMean>( this );
    mAvailableFeatures[ XTRACT_SPECTRAL_VARIANCE ]              = ciXtractFeature::create<ciXtractSpectralVariance>( this );
    mAvailableFeatures[ XTRACT_SPECTRAL_STANDARD_DEVIATION ]    = ciXtractFeature::create<ciXtractSpectralStandardDeviation>( this );
    mAvailableFeatures[ XTRACT_SPECTRAL_SKEWNESS ]              = ciXtractFeature::create<ciXtractSpectralSkewness>( this );
    mAvailableFeatures[ XTRACT_SPECTRAL_KURTOSIS ]              = ciXtractFeature::create<ciXtractSpectralKurtosis>( this );
    mAvailableFeatures[ XTRACT_SPECTRAL_CENTROID ]              = ciXtractFeature::create<ciXtractSpectralCentroid>( this );
    mAvailableFeatures[ XTRACT_IRREGULARITY_K ]                 = ciXtractFeature::create<ciXtractIrregularityK>( this );
    mAvailableFeatures[ XTRACT_IRREGULARITY_J ]                 = ciXtractFeature::create<ciXtractIrregularityJ>( this );
    mAvailableFeatures[ XTRACT_TRISTIMULUS_1 ]                  = ciXtractFeature::create<ciXtractTristimulus1>( this );
    mAvailableFeatures[ XTRACT_SMOOTHNESS ]                     = ciXtractFeature::create<ciXtractSmoothness>( this );
    mAvailableFeatures[ XTRACT_SPREAD ]                         = ciXtractFeature::create<ciXtractSpread>( this );
    mAvailableFeatures[ XTRACT_ZCR ]                            = ciXtractFeature::create<ciXtractZcr>( this );
    mAvailableFeatures[ XTRACT_LOUDNESS ]                       = ciXtractFeature::create<ciXtractLoudness>( this );
    mAvailableFeatures[ XTRACT_FLATNESS ]                       = ciXtractFeature::create<ciXtractFlatness>( this );
    mAvailableFeatures[ XTRACT_FLATNESS_DB ]                    = ciXtractFeature::create<ciXtractFlatnessDb>( this );
    mAvailableFeatures[ XTRACT_TONALITY ]                       = ciXtractFeature::create<ciXtractTonality>( this );
    mAvailableFeatures[ XTRACT_RMS_AMPLITUDE ]                  = ciXtractFeature::create<ciXtractRmsAmplitude>( this );
    mAvailableFeatures[ XTRACT_SPECTRAL_INHARMONICITY ]         = ciXtractFeature::create<ciXtractSpectralInharmonicity>( this );
    mAvailableFeatures[ XTRACT_CREST ]                          = ciXtractFeature::create<ciXtractCrest>( this );
    mAvailableFeatures[ XTRACT_POWER ]                          = ciXtractFeature::create<ciXtractPower>( this );
    mAvailableFeatures[ XTRACT_ODD_EVEN_RATIO ]                 = ciXtractFeature::create<ciXtractOddEvenRatio>( this );
    mAvailableFeatures[ XTRACT_SHARPNESS ]                      = ciXtractFeature::create<ciXtractSharpness>( this );
    mAvailableFeatures[ XTRACT_SPECTRAL_SLOPE ]                 = ciXtractFeature::create<ciXtractSpectralSlope>( this );
    mAvailableFeatures[ XTRACT_LOWEST_VALUE ]                   = ciXtractFeature::create<ciXtractLowestValue>( this );
    mAvailableFeatures[ XTRACT_HIGHEST_VALUE ]                  = ciXtractFeature::create<ciXtractHighestValue>( this );
    mAvailableFeatures[ XTRACT_SUM ]                            = ciXtractFeature::create<ciXtractSum>( this );
    mAvailableFeatures[ XTRACT_HPS ]                            = ciXtractFeature::create<ciXtractHps>( this );
    mAvailableFeatures[ XTRACT_F0 ]                             = ciXtractFeature::create<ciXtractF0>( this );
    mAvailableFeatures[ XTRACT_FAILSAFE_F0 ]                    = ciXtractFeature::create<ciXtractFailsafeF0>( this );
    mAvailableFeatures[ XTRACT_WAVELET_F0 ]                     = ciXtractFeature::create<ciXtractWaveletF0>( this );
    mAvailableFeatures[ XTRACT_NONZERO_COUNT ]                  = ciXtractFeature::create<ciXtractNonZeroCount>( this );

}

void ciXtract::update( const float *pcmData )
{
    mLastUpdateAt = getElapsedFrames();
    
	for( size_t k=0; k < CIXTRACT_PCM_SIZE; k++ )
        mPcmDataRaw.get()[k] = pcmData[k];
    
    xtract_windowed( mPcmDataRaw.get(), CIXTRACT_PCM_SIZE, mWindowFunc, mPcmData.get() );
    
    vector<ciXtractFeatureRef>::iterator    it;
    
    for( it = mActiveFeatures.begin(); it!=mActiveFeatures.end(); ++it )
        (*it)->update( mLastUpdateAt );
}


void ciXtract::enableFeature( xtract_features_ feature )
{
    if ( feature >= XTRACT_FEATURES )
        return;
    
    ciXtractFeatureRef f = mAvailableFeatures[ feature ];

    if ( !f || f->mIsEnable )
        return;
    
    f->mIsEnable = true;
    mActiveFeatures.push_back( f );
    
    vector<xtract_features_> dependencies = f->mDependencies;
    for( size_t k=0; k < dependencies.size(); k++ )
        enableFeature( dependencies[k] );
}


void ciXtract::disableFeature( xtract_features_ feature )
{
    ciXtractFeatureRef f = getActiveFeature( feature );
    if ( !f )
        return;
    
    f->mIsEnable = false;
    
    // remove the feature from mActiveFeatures
    for( size_t k=0; k < mActiveFeatures.size(); k++ )
    {
        if ( mActiveFeatures[k] == f )
        {
            mActiveFeatures.erase( mActiveFeatures.begin() + k );
            break;
        }
    }
    
    // disable all features that depends on this one
    std::vector<ciXtractFeatureRef>::iterator it;
    for( it = mActiveFeatures.begin(); it != mActiveFeatures.end(); ++it )
        if ( featureDependsOn( (*it)->mFeatureEnum, feature ) )
            disableFeature( (*it)->mFeatureEnum );
}


bool ciXtract::featureDependsOn( xtract_features_ this_feature, xtract_features_ test_feature )
{
    if ( this_feature >= XTRACT_FEATURES || test_feature >= XTRACT_FEATURES )
        return false;
    
    ciXtractFeatureRef f = mAvailableFeatures[ this_feature ];
    if ( !f )
        return false;
    
    vector<xtract_features_> dependencies = f->mDependencies;
    for( size_t i=0; i < dependencies.size(); i++ )
        if ( test_feature == dependencies[i] )
            return true;
    
    return false;
}


ciXtractFeatureRef ciXtract::getActiveFeature( xtract_features_ feature )
{
    std::vector<ciXtractFeatureRef>::iterator it;
    for( it = mActiveFeatures.begin(); it != mActiveFeatures.end(); ++it )
        if ( (*it)->getEnum() == feature )
            return (*it);
    
    console() << "ciXtract::getFeature FEATURE NOT FOUND! " << feature << endl;
    
    return ciXtractFeatureRef();
}

void ciXtract::listFeatures()
{
    console() << endl << "--- ciXtract available features ---" << endl;
    console() << endl << "--- TODO!!!!! ---" << endl;
    
//    vector<ciXtractFeatureRef>::iterator it;
//    for( it = mFeatures.begin(); it != mFeatures.end(); ++it )
//        console() << (*it)->getEnumStr() << endl;

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

    gl::drawString( feature->getName(), rect.getUpperLeft(), labelCol, Font( "Arial", 8 ) );
    
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
