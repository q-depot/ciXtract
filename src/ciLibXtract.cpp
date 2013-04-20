
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
    
    mPcmData            = std::shared_ptr<double>( new double[ PCM_BUFF_SIZE ] );
    mSpectrum           = std::shared_ptr<double>( new double[ PCM_BUFF_SIZE ] );
    mPeakSpectrum       = std::shared_ptr<double>( new double[ PCM_BUFF_SIZE ] );
    mBarks              = std::shared_ptr<double>( new double[ XTRACT_BARK_BANDS ] );
    mHarmonicSpectrum   = std::shared_ptr<double>( new double[ PCM_BUFF_SIZE ] );
    mAutocorrelationFft = std::shared_ptr<double>( new double[ PCM_BUFF_SIZE ] );
    
    mMfccs              = std::shared_ptr<double>( new double[ MFCC_FREQ_BANDS ] );
    mBarkBandLimits     = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );

    mel_filters.n_filters = MFCC_FREQ_BANDS;
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
        mel_filters.filters[n] = (double *)malloc(BLOCKSIZE * sizeof(double));

    // TODO double check initialisation !!! <<< 
    xtract_init_mfcc( BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters );
    
    xtract_init_bark( BLOCKSIZE >> 2, SAMPLERATE >> 1, mBarkBandLimits.get() );
}


void ciLibXtract::setInterleavedData( audio::Buffer32fRef buff )
{
    for( size_t k=0; k < PCM_BUFF_SIZE; k++ )
        mPcmData.get()[k] = buff->mData[k*2];
}


void ciLibXtract::setSpectrum( std::shared_ptr<float> fftDataRef )
{
    for( size_t k=0; k < FFT_SIZE; k++ )
        mSpectrum.get()[k] = fftDataRef.get()[k];
}


double ciLibXtract::getMean()
{
    double mean = 0.0f;
    xtract[XTRACT_MEAN]( mPcmData.get(), PCM_BUFF_SIZE, NULL, &mean );
    return mean;
}


shared_ptr<double> ciLibXtract::getSpectrum( bool normalised )
{    
    mArgd[0] = SAMPLERATE / (double)PCM_BUFF_SIZE;
    mArgd[1] = XTRACT_MAGNITUDE_SPECTRUM;           //  XTRACT_MAGNITUDE_SPECTRUM, XTRACT_LOG_MAGNITUDE_SPECTRUM, XTRACT_POWER_SPECTRUM, XTRACT_LOG_POWER_SPECTRUM
    mArgd[2] = 0.f;                                 // No DC component
    mArgd[3] = normalised;                          // No Normalisation
    
    xtract[XTRACT_SPECTRUM]( mPcmData.get(), PCM_BUFF_SIZE, mArgd, mSpectrum.get() );

    return mSpectrum;
}


shared_ptr<double> ciLibXtract::getAutocorrelationFft()
{
    void *argd = NULL;
    xtract_autocorrelation_fft( mPcmData.get(), PCM_BUFF_SIZE, argd, mAutocorrelationFft.get() );
    return mAutocorrelationFft;
}


shared_ptr<double> ciLibXtract::getMfcc()
{
    xtract_mfcc( mSpectrum.get(), FFT_SIZE, &mel_filters, mMfccs.get() );
    
    return mMfccs;
}



shared_ptr<double> ciLibXtract::getPeakSpectrum( double threshold )
{
    mArgd[0] = SAMPLERATE / (double)FFT_SIZE;     // samplerate / N
    mArgd[1] = threshold;                                   // peak threshold as percentage of the magnitude of the maximum peak found
    mArgd[2] = 0.f;
    mArgd[3] = 0.f;
    
    xtract_peak_spectrum( mSpectrum.get(), FFT_SIZE, mArgd, mPeakSpectrum.get() );
    
    return mPeakSpectrum;
}




std::shared_ptr<double> ciLibXtract::getBarkCoefficients()
{
    xtract_bark_coefficients( mSpectrum.get(), FFT_SIZE, mBarkBandLimits.get(), mBarks.get() );

    return mBarks;
}


std::shared_ptr<double> ciLibXtract::getHarmonicSpectrum()
{
    mArgd[0] = mF0;
    mArgd[1] = 0.3f;

    xtract_harmonic_spectrum( mPeakSpectrum.get(), FFT_SIZE, mArgd, mHarmonicSpectrum.get() );

    return mHarmonicSpectrum;
}


// TODO doesn't work!

double ciLibXtract::getF0()
{
    double sr = SAMPLERATE / (double)PCM_BUFF_SIZE;
    
    xtract_f0( mPcmData.get(), PCM_BUFF_SIZE, &sr, &mF0 );
    
    return mF0;
}


double ciLibXtract::getSpectralCentroid()
{
    void *argd = NULL;
    
    xtract_spectral_centroid( mSpectrum.get(), FFT_SIZE, argd, &mSpectralCentroid );
    
    return mSpectralCentroid;
}

