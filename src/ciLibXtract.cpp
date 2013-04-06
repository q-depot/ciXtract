
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
    
    mPcmData        = std::shared_ptr<double>( new double[ BLOCKSIZE ] );
    mSpectrum       = std::shared_ptr<double>( new double[ BLOCKSIZE ] );
    mBarks          = std::shared_ptr<double>( new double[ BLOCKSIZE ] );
    mMfccs          = std::shared_ptr<double>( new double[ MFCC_FREQ_BANDS ] );
    mBarkBandLimits = std::shared_ptr<int>( new int[ XTRACT_BARK_BANDS ] );

    mel_filters.n_filters = MFCC_FREQ_BANDS;
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
        mel_filters.filters[n] = (double *)malloc(BLOCKSIZE * sizeof(double));

    xtract_init_mfcc( BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters );
    
    xtract_init_bark( BLOCKSIZE >> 2, SAMPLERATE >> 1, mBarkBandLimits.get() );
}


void ciLibXtract::setPcmData( audio::Buffer32fRef pcmBufferRef )
{
    for( size_t k=0; k < BLOCKSIZE; k++ )
        mPcmData.get()[k] = pcmBufferRef->mData[k];
}


void ciLibXtract::setSpectrum( std::shared_ptr<float> fftDataRef )
{
    for( size_t k=0; k < BLOCKSIZE >> 2; k++ )
        mSpectrum.get()[k] = fftDataRef.get()[k];
}


float ciLibXtract::getMean()
{
    double mean = 0.0f;
    xtract[XTRACT_MEAN]( mPcmData.get(), BLOCKSIZE / 2, NULL, &mean );
    return mean;
}


shared_ptr<double> ciLibXtract::getSpectrum()
{
    
    return mSpectrum;
    
    
    
    mArgd[0] = SAMPLERATE / (double)BLOCKSIZE;
    mArgd[1] = XTRACT_MAGNITUDE_SPECTRUM;           //  XTRACT_MAGNITUDE_SPECTRUM, XTRACT_LOG_MAGNITUDE_SPECTRUM, XTRACT_POWER_SPECTRUM, XTRACT_LOG_POWER_SPECTRUM
    mArgd[2] = 0.f;                                 // No DC component
    mArgd[3] = 1.f;                                 // No Normalisation
    
    xtract[XTRACT_SPECTRUM]( mPcmData.get(), BLOCKSIZE / 2, mArgd, mSpectrum.get() );

    return mSpectrum;
}


shared_ptr<double> ciLibXtract::getMfcc()
{
    xtract_mfcc( mSpectrum.get(), BLOCKSIZE >> 2, &mel_filters, mMfccs.get() );

    return mMfccs;
}


std::shared_ptr<double> ciLibXtract::getBarks()
{
    xtract_bark_coefficients( mSpectrum.get(), BLOCKSIZE >> 2, mBarkBandLimits.get(), mBarks.get() );

    return mBarks;
}

