
#ifndef CILIBXTRACT
#define CILIBXTRACT

#include "libxtract.h"
#include "cinder/audio/PcmBuffer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define BLOCKSIZE           2048
#define SAMPLERATE          44100
#define PERIOD              100
#define MFCC_FREQ_BANDS     32
#define MFCC_FREQ_MIN       20
#define MFCC_FREQ_MAX       20000


class ciLibXtract {
    
public:
    
    ciLibXtract();
    
    ~ciLibXtract();
    
    void init();
    
    float getMean();
    
    void setPcmData( ci::audio::Buffer32fRef pcmBufferRef );
    
    void setSpectrum( std::shared_ptr<float> fftDataRef );
    
    std::shared_ptr<double> getSpectrum();
    
    std::shared_ptr<double> getMfcc();
    
    std::shared_ptr<double> getBarks();
    
    
private:
    
    std::shared_ptr<double> mPcmData;
    std::shared_ptr<double> mSpectrum;
    std::shared_ptr<double> mMfccs;
    std::shared_ptr<double> mBarks;

    double                  mArgd[4];
    
    xtract_mel_filter       mel_filters;
    
    std::shared_ptr<int>    mBarkBandLimits;
};

#endif
