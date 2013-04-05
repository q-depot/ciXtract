
#include "ciLibXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;


ciLibXtract::ciLibXtract()
{
    xtract_function_descriptor_t *descriptors = xtract_make_descriptors();
    
    
    console() << "Descriptor:\n";
    console() << descriptors[XTRACT_MEAN].algo.p_name << endl;
    console() << descriptors[XTRACT_MEAN].algo.p_desc << endl;
    console() << descriptors[XTRACT_MEAN].argv.min << " --- " << descriptors[XTRACT_MEAN].argv.max << endl;

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
    
    mArgd[0] = SAMPLERATE / (double)BLOCKSIZE;
    mArgd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    
    mArgd[2] = 0.f;                                 // No DC component
    mArgd[3] = 0.f;                                 // No Normalisation

    mPcmDataSampleCount = 0;
    
//    for(int n = 0; n < BLOCKSIZE; ++n)
//        mInputData[n] = 0.0f;
    
    mel_filters.n_filters = MFCC_FREQ_BANDS;
    
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    
    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
        mel_filters.filters[n] = (double *)malloc(BLOCKSIZE * sizeof(double));
    
    xtract_init_mfcc( BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters );
    
    mMfccs = shared_ptr<double>( new double[ MFCC_FREQ_BANDS ] );
}


void ciLibXtract::setPcmData( audio::Buffer32fRef pcmBufferRef )
{
    mPcmData.reset();
    mSpectrum.reset();
    
    mPcmDataSampleCount = pcmBufferRef->mSampleCount;
    
    mPcmData    = shared_ptr<double>( new double[ mPcmDataSampleCount ] );
    mSpectrum   = shared_ptr<double>( new double[ mPcmDataSampleCount ] );
    //    double mMfccs[MFCC_FREQ_BANDS];
    double *data        = mPcmData.get();
    double *spectrum    = mSpectrum.get();
    
    for( size_t k=0; k < mPcmDataSampleCount; k++ )
    {
        (*mPcmData)[k]     = pcmBufferRef->mData[k];
        spectrum[k] = 0.0f;
    }
}


float ciLibXtract::getMean()
{
    double mean = 0.0f;
    xtract[XTRACT_MEAN]( mPcmData.get(), mPcmDataSampleCount, NULL, &mean );
    return mean;
}


shared_ptr<double> ciLibXtract::getSpectrum()
{
    xtract[XTRACT_SPECTRUM]( mPcmData.get(), mPcmDataSampleCount, mArgd, mSpectrum.get() );

    return mSpectrum;
}


shared_ptr<double> ciLibXtract::getMfcc()
{
    /* compute the MFCCs */
    mel_filters.n_filters = MFCC_FREQ_BANDS;
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
    {
        mel_filters.filters[n] = (double *)malloc(BLOCKSIZE * sizeof(double));
    }

    xtract_init_mfcc( BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters );
    
    xtract_mfcc( mSpectrum.get(), BLOCKSIZE >> 1, &mel_filters, mMfccs.get() );
    
//    double *mfccs = mMfccs.get();
    /* print the MFCCs */
//    printf("MFCCs:\n");
//    for( int n = 0; n < MFCC_FREQ_BANDS; ++n )
//    {
//        printf("band: %d\t", n);
//        if(n < 10) {
//            printf("\t");
//        }
//        printf("coeff: %f\n", mfccs[n]);
//    }

    return mMfccs;
}


//	float * fftBuffer = mFftDataRef.get();
//
//
//    int c =0;
//	for( size_t k=0; k < BLOCKSIZE; k++ )
//        mFftBuffer[k] = fftBuffer[k];
//
//    double mean = 0.0f;
//    //    xtract[XTRACT_MEAN]( mInputData, BLOCKSIZE, NULL, &mean );
//
//    if ( mean > mMean )
//        mMean = mean;
//    else
//        mMean *= 0.9f;
//
//    mPcmBuffer = mInput.getPcmBuffer();
//	if( ! mPcmBuffer )
//		return;
//    audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
//
//    for( size_t k=0; k < BLOCKSIZE; k++ )
//        mInputData[k] = leftBuffer->mData[k];
//
//    xtract[XTRACT_SPECTRUM]( mInputData, BLOCKSIZE, mArgd, mSpectrum );
//
//    xtract_mfcc( mSpectrum, BLOCKSIZE >> 1, &mel_filters, mMfccs );
//}

