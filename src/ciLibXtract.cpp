
#include "ciLibXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


ciLibXtract::ciLibXtract()
{
//    xtract_function_descriptor_t *descriptors = xtract_make_descriptors();
//    
//    console() << "Descriptor:\n";
//    console() << descriptors[XTRACT_MEAN].algo.p_name << endl;
//    console() << descriptors[XTRACT_MEAN].algo.p_desc << endl;
//    console() << descriptors[XTRACT_MEAN].argv.min << " --- " << descriptors[XTRACT_MEAN].argv.max << endl;
}


ciLibXtract::~ciLibXtract()
{
    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
        free( mel_filters.filters[n] );
    free( mel_filters.filters );
}

void ciLibXtract::init()
{
    xtract_init_fft( BLOCKSIZE, XTRACT_SPECTRUM );
    
    mPcmData            = std::shared_ptr<double>( new double[ BLOCKSIZE ] );
    mSpectrum           = std::shared_ptr<double>( new double[ BLOCKSIZE ] );
    mPeakSpectrum       = std::shared_ptr<double>( new double[ BLOCKSIZE ] );
    mBarks              = std::shared_ptr<double>( new double[ BLOCKSIZE ] );
    mHarmonicSpectrum   = std::shared_ptr<double>( new double[ BLOCKSIZE ] );
    mAutocorrelationFft = std::shared_ptr<double>( new double[ BLOCKSIZE ] );
    
    mMfccs              = std::shared_ptr<double>( new double[ MFCC_FREQ_BANDS ] );
    mBarkBandLimits     = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );

    mel_filters.n_filters = MFCC_FREQ_BANDS;
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
        mel_filters.filters[n] = (double *)malloc(BLOCKSIZE * sizeof(double));

    xtract_init_mfcc( BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters );
    
    xtract_init_bark( BLOCKSIZE >> 2, SAMPLERATE >> 1, mBarkBandLimits.get() );
}





void ciLibXtract::setPcmData( audio::PcmBuffer32fRef pcmBufferRef, bool isInterleaved )
{
    audio::Buffer32fRef buff;
    
    if ( isInterleaved )
    {
        buff = pcmBufferRef->getInterleavedData();
        
        size_t buffLength = buff->mSampleCount;
        
        for( size_t k=0; k < buffLength; k+=2 )
            mPcmData.get()[k/2] = buff->mData[k];
    }
    else
    {
    buff = pcmBufferRef->getChannelData( audio::CHANNEL_FRONT_LEFT );
        for( size_t k=0; k < BLOCKSIZE; k++ )
            mPcmData.get()[k] = buff->mData[k];
    }
    
}


void ciLibXtract::setSpectrum( std::shared_ptr<float> fftDataRef )
{
    for( size_t k=0; k < BLOCKSIZE >> 2; k++ )
        mSpectrum.get()[k] = fftDataRef.get()[k];
}


double ciLibXtract::getMean()
{
    double mean = 0.0f;
    xtract[XTRACT_MEAN]( mPcmData.get(), BLOCKSIZE / 2, NULL, &mean );
    return mean;
}


shared_ptr<double> ciLibXtract::getSpectrum()
{    
    mArgd[0] = SAMPLERATE / (double)1024;
    mArgd[1] = XTRACT_MAGNITUDE_SPECTRUM;           //  XTRACT_MAGNITUDE_SPECTRUM, XTRACT_LOG_MAGNITUDE_SPECTRUM, XTRACT_POWER_SPECTRUM, XTRACT_LOG_POWER_SPECTRUM
    mArgd[2] = 0.f;                                 // No DC component
    mArgd[3] = 0.f;                                 // No Normalisation
    
    xtract[XTRACT_SPECTRUM]( mPcmData.get(), 1024, mArgd, mSpectrum.get() );

    return mSpectrum;
}


shared_ptr<double> ciLibXtract::getAutocorrelationFft()
{
//    void *argd = NULL;
//    xtract_autocorrelation_fft( mPcmData.get(), BLOCKSIZE >> 2, argd, mAutocorrelationFft.get() );
    return mAutocorrelationFft;
}


shared_ptr<double> ciLibXtract::getMfcc()
{
    xtract_mfcc( mSpectrum.get(), BLOCKSIZE >> 2, &mel_filters, mMfccs.get() );
    
    return mMfccs;
}



shared_ptr<double> ciLibXtract::getPeakSpectrum( double threshold )
{
    mArgd[0] = SAMPLERATE / (double)( BLOCKSIZE >> 2 );     // samplerate / N
    mArgd[1] = threshold;                                   // peak threshold as percentage of the magnitude of the maximum peak found
    mArgd[2] = 0.f;
    mArgd[3] = 0.f;
    
    xtract_peak_spectrum( mSpectrum.get(), BLOCKSIZE >> 2, mArgd, mPeakSpectrum.get() );
    
    return mPeakSpectrum;
}




std::shared_ptr<double> ciLibXtract::getBarkCoefficients()
{
    xtract_bark_coefficients( mSpectrum.get(), BLOCKSIZE >> 2, mBarkBandLimits.get(), mBarks.get() );

    return mBarks;
}


std::shared_ptr<double> ciLibXtract::getHarmonicSpectrum()
{
    mArgd[0] = mF0;
    mArgd[1] = 0.3f;

    xtract_harmonic_spectrum( mPeakSpectrum.get(), BLOCKSIZE >> 2, mArgd, mHarmonicSpectrum.get() );

    return mHarmonicSpectrum;
}


// TODO doesn't work!

double ciLibXtract::getF0()
{
    double sr = SAMPLERATE / (double)BLOCKSIZE;
    
    xtract_f0( mPcmData.get(), BLOCKSIZE >> 1, &sr, &mF0 );
    
    return mF0;
}


double ciLibXtract::getSpectralCentroid()
{
    void *argd = NULL;
    
    xtract_spectral_centroid( mSpectrum.get(), BLOCKSIZE >> 2, argd, &mSpectralCentroid );
    
    return mSpectralCentroid;
}

