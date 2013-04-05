
#ifndef CILIBXTRACT
#define CILIBXTRACT

#include "libxtract.h"
#include "cinder/audio/PcmBuffer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define BLOCKSIZE 2048
#define SAMPLERATE 44100
#define PERIOD 100
#define MFCC_FREQ_BANDS 32
#define MFCC_FREQ_MIN 20
#define MFCC_FREQ_MAX 20000


class ciLibXtract {
    
public:
    
    ciLibXtract();
    
    ~ciLibXtract();
    
    void init();
    
    float getMean();
    
    std::shared_ptr<double> getSpectrum();
    
    std::shared_ptr<double> getMfcc();
    
    void setPcmData( ci::audio::Buffer32fRef pcmBufferRef );
    
private:
    
    std::shared_ptr<double> mPcmData;
    size_t                  mPcmDataSampleCount;
    
    std::shared_ptr<double> mSpectrum;
    std::shared_ptr<double> mMfccs;
    
//    double mMfccs[MFCC_FREQ_BANDS];
    double mArgd[4];
    
    xtract_mel_filter mel_filters;

};

#endif


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

