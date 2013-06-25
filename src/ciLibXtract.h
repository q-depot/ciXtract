
#ifndef CILIBXTRACT
#define CILIBXTRACT

#pragma once

#include "libxtract.h"
#include "cinder/audio/Output.h"


using namespace ci;
using namespace ci::app;
using namespace std;

//#define BLOCKSIZE           2048
#define PCM_BUFF_SIZE       1024
#define FFT_SIZE            512
#define SAMPLERATE          44100
#define PERIOD              100
#define MFCC_FREQ_BANDS     32
#define MFCC_FREQ_MIN       20
#define MFCC_FREQ_MAX       20000
#define SUB_BANDS           32

/*
class AudioSource;
typedef std::shared_ptr<AudioSource>    AudioSOurceRef;

class AudioSource {

public:
    
    static AudioSourceRef create( ci::audio::SourceRef source ) { return AudioSourceRef( new AudioSource( source ) ); }
    static AudioSourceRef create( ci::audio::TrackRef )         { return AudioSourceRef( new AudioSource( source ) ); }
    
    ~AudioSource() {}
    
    
private:
    
    AudioSource( ci::audio::SourceRef source ) : mSourceAudioRef(source) {}
    AudioSource( ci::audio::TrackRef source ) : mSourceAudioRef(source) {}


private:
    
    ci::audio::SourceRef    mSourceAudioRef;
    ci::audio::TrackRef     mSourceTrackRef;
    
};
*/









class ciLibXtract {
    
public:
    
    ciLibXtract();
    
    ~ciLibXtract();
    
    void init();
    
    void setInterleavedData( ci::audio::Buffer32fRef buff );

    void setSpectrum( std::shared_ptr<float> fftDataRef );
    
    void update();
    
    void enableFeature( xtract_features_ feature )  { mFeaturesEnable[ feature ] = true; }
    void disableFeature( xtract_features_ feature ) { mFeaturesEnable[ feature ] = false; }

    
    void setSource( ci::audio::SourceRef source );
    void setSource( audio::TrackRef source );

    
    // ------------------------------------ //
    //    Scalar extraction functions       //
    // ------------------------------------ //
    
public:
   
    // 	Extract frequency domain spectrum from time domain signal.
    std::shared_ptr<double> getSpectrum( xtract_spectrum_ spectrumType, bool normalised );
    
    // 	Extract autocorrelation from time domain signal using FFT based method.
    std::shared_ptr<double> getAutocorrelationFft();
    
    // 	Extract Mel Frequency Cepstral Coefficients based on a method described by Rabiner.
    std::shared_ptr<double> getMfcc();
    
    // 	Extract the Discrete Cosine transform of a time domain signal.
//    std::shared_ptr<double> getDct() { return std::shared_ptr<double>( new double ); }
    
    // 	Extract autocorrelation from time domain signal using time-domain autocorrelation technique.
//    std::shared_ptr<double> getAutocorrelation() { return std::shared_ptr<double>( new double ); }
    
    // 	Extract Average Magnitude Difference Function from time domain signal.
//    std::shared_ptr<double> getAmdf() { return std::shared_ptr<double>( new double ); }
    
    // 	Extract Average Squared Difference Function from time domain signal.
//    std::shared_ptr<double> getAsdf() { return std::shared_ptr<double>( new double ); }
    
    // 	Extract Bark band coefficients based on a method.
    std::shared_ptr<double> getBarkCoefficients();
    
    // 	Extract the amplitude and frequency of spectral peaks from a magnitude spectrum.
    std::shared_ptr<double> getPeakSpectrum( double threshold = 0.3f );
    
    // 	Extract the harmonic spectrum of from a of a peak spectrum.
    std::shared_ptr<double> getHarmonicSpectrum();
    
    // 	Extract Linear Predictive Coding Coefficients.
//    std::shared_ptr<double> getLpc() { return std::shared_ptr<double>( new double ); }

    // 	Extract Linear Predictive Coding Cepstral Coefficients.
//    std::shared_ptr<double> getLpcc() { return std::shared_ptr<double>( new double ); }

    // 	Extract subbands from a spectrum.
    std::shared_ptr<double> getSubBands();
    
    
    
    
    // ------------------------------------ //
    //    Scalar extraction functions       //
    // ------------------------------------ //
    
public:
    
    // 	Extract the mean of an input vector.
    double getMean();
    
    // 	Extract the variance of an input vector.
    double 	getVariance();

    //  Extract the deviation of an input vector.
    double 	getStandardDeviation();
    
    // 	Extract the average deviation of an input vector.
    double 	getAverageDeviation();

    // 	Extract the skewness of an input vector.
//    double 	getSkewness() { return 0.0f; }
    
    // 	Extract the kurtosis of an input vector.
//    double 	getKurtosis() { return 0.0f; }
    
    // 	Extract the mean of an input spectrum.
//    double 	getSpectralMean() { return 0.0f; }
    
    // 	Extract the variance of an input spectrum.
//    double 	getSpectralVariance() { return 0.0f; }
    
    // 	Extract the deviation of an input spectrum.
//    double 	getSpectralStandardDeviation() { return 0.0f; }
    
    // 	Extract the average deviation of an input spectrum.
//    double 	getSpectralSkewness() { return 0.0f; }
    
    // 	Extract the kurtosis of an input spectrum.
//    double 	getSpectralKurtosis() { return 0.0f; }
    
    // 	Extract the centroid of an input vector.
    double 	getSpectralCentroid();
    
    // 	Calculate the Irregularity of an input vector using a method described by Krimphoff (1994)
    double 	getIrregularityK();
    
    // 	Calculate the Irregularity of an input vector using a method described by Jensen (1999)
    double 	getIrregularityJ();
    
    // 	Calculate the Tristimulus of an input vector using a method described by Pollard and Jansson (1982)
//    double 	getTristimulus1() { return 0.0f; }
    
//    double 	getTristimulus2() { return 0.0f; }
    
//    double 	getTristimulus3() { return 0.0f; }
    
    // 	Extract the smoothness of an input vector using a method described by McAdams (1999)
//    double 	getSmoothness() { return 0.0f; }
    
    // 	Extract the spectral spread of an input vector using a method described by Casagrande(2005)
    double 	getSpread();
    
    // 	Extract the zero crossing rate of an input vector.
    double 	getZcr();
    
    // 	Extract the spectral rolloff of an input vector using a method described by Bee Suan Ong (2005)
//    double 	getRolloff() { return 0.0f; }
    
    // 	Extract the 'total loudness' of an input vector using a method described by Moore, Glasberg et al (2005)
    double 	getLoudness();
    
    // 	Extract the spectral flatness measure of an input vector, where the flatness measure (SFM) is defined as the ratio of the geometric mean to the arithmetic mean of a magnitude spectrum.
    double 	getFlatness();
    
    // 	Extract the LOG spectral flatness measure of an input vector.
    double 	getFlatnessDb();
    
    // 	Extract the tonality factor of an input vector using a method described by Peeters 2003.
    double 	getTonality();
    
    // 	Extract the noisiness of an input vector using a method described by Tae Hong Park (2000)
//    double 	getNoisiness() { return 0.0f; }
    
    // 	Extract the RMS amplitude of an input vector using a method described by Tae Hong Park (2000)
//    double 	getRmsAmplitude() { return 0.0f; }
    
    // 	Extract the Inharmonicity of an input vector.
//    double 	getSpectralInharmonicity() { return 0.0f; }
    
    // 	Extract the spectral crest of an input vector using a method described by Peeters (2003)
//    double 	getCrest() { return 0.0f; }
    
    // 	Extract the Spectral Power of an input vector using a method described by Bee Suan Ong (2005)
    double 	getPower();
    
    // 	Extract the Odd to even harmonic ratio of an input vector.
//    double 	getOddEvenRatio() { return 0.0f; }
    
    // 	Extract the Sharpness of an input vector.
//    double 	getSharpness() { return 0.0f; }
    
    // 	Extract the Slope of an input vector using a method described by Peeters(2003)
//    double 	getSpectralSlope() { return 0.0f; }
    
    // 	Extract the value of the lowest value in an input vector.
//    double 	getLowestValue() { return 0.0f; }
    
    // 	Extract the value of the highest value in an input vector.
//    double 	getHighestValue() { return 0.0f; }
    
    // 	Extract the sum of the values in an input vector.
//    double 	getSum() { return 0.0f; }
    
    // 	Extract the Pitch of an input vector using Harmonic Product Spectrum (HPS) analysis.
//    double 	getHps() { return 0.0f; }
    
    // 	Extract the fundamental frequency of an input vector.
    double 	getF0();
    
    // 	Extract the fundamental frequency of an input vector.
    double 	getFailsafeF0();
    
    // 	Extract the number of non-zero elements in an input vector.
//    double 	getNonzeroCount() { return 0.0f; }
    
    
    // ------------------------------------ //
    //            Other functions           //
    // ------------------------------------ //
    
public:
    
    bool getOnSet( float threshold = 1.0f, float vel = 0.9f, float gain = 1.0f );
    
private:
    
    
    
//    audio::Buffer32fRef buff;
    
    std::shared_ptr<double> mPcmData;
    std::shared_ptr<double> mSpectrum;
    std::shared_ptr<double> mAutocorrelationFft;
    std::shared_ptr<double> mPeakSpectrum;
    std::shared_ptr<double> mMfccs;
    std::shared_ptr<double> mBarks;
    std::shared_ptr<double> mHarmonicSpectrum;
    std::shared_ptr<double> mSubBands;
    
    double                  mArgd[4];
    
    double                  mMean;
    double                  mF0;
    double                  mFailsafeF0;
    double                  mSpectralCentroid;
    double                  mSpread;
    double                  mLoudness;
    double                  mIrregularityK;
    double                  mIrregularityJ;
    double                  mZcr;
    double                  mAverageDeviation;
    double                  mStandardDeviation;
    double                  mVariance;
    double                  mFlatness;
    double                  mFlatnessDb;
    double                  mPower;
    double                  mTonality;
    
    xtract_mel_filter       mel_filters;
    
    std::shared_ptr<int>    mBarkBandLimits;
    
    bool                    mFeaturesEnable[ XTRACT_FEATURES ];
    
    double                  mBarkOnSetAvg;
    
};

#endif
