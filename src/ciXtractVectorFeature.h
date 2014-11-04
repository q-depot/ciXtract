/*
 *  ciXtractFeature.h
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef CI_XTRACT_VECTOR_FEATURE
#define CI_XTRACT_VECTOR_FEATURE

#pragma once

#include "ciXtractFeature.h"

//    xtract_spectrum
//    xtract_autocorrelation_fft
//    xtract_mfcc
//    xtract_dct
//    xtract_autocorrelation
//    xtract_amdf
//    xtract_asdf
//    xtract_bark_coefficients
//    xtract_peak_spectrum
//    xtract_harmonic_spectrum
//    xtract_lpc
//    xtract_lpcc
//    xtract_subbands

// Spectrum
class ciXtractSpectrum : public ciXtractFeature {
    
public:
    ciXtractSpectrum( ciXtract *xtract );
    ~ciXtractSpectrum();
    void update( int frameN  );
};

// Autocorrelation Fft
class ciXtractAutocorrelationFft : public ciXtractFeature {
    
public:
    ciXtractAutocorrelationFft( ciXtract *xtract );
    ~ciXtractAutocorrelationFft() {}
    void update( int frameN );
};

// Mfcc
class ciXtractMfcc : public ciXtractFeature {
    
public:
    ciXtractMfcc( ciXtract *xtract );
    ~ciXtractMfcc();
    void update( int frameN );
private:
    xtract_mel_filter   mMelFilters;
};

//    xtract_dct

// Autocorrelation
class ciXtractAutocorrelation : public ciXtractFeature {
    
public:
    ciXtractAutocorrelation( ciXtract *xtract );
    ~ciXtractAutocorrelation() {}
    void update( int frameN );
};

//    xtract_amdf
//    xtract_asdf

// Bark
class ciXtractBark : public ciXtractFeature {
    
public:
    ciXtractBark( ciXtract *xtract );
    ~ciXtractBark() {}
    void update( int frameN );
private:
    std::shared_ptr<int> mBandLimits;
};

// Peak Spectrum
class ciXtractPeakSpectrum : public ciXtractFeature {
    
public:
    ciXtractPeakSpectrum( ciXtract *xtract );
    ~ciXtractPeakSpectrum()  {}
};

// Harmonic Spectrum
class ciXtractHarmonicSpectrum : public ciXtractFeature {
    
public:
    ciXtractHarmonicSpectrum( ciXtract *xtract );
    ~ciXtractHarmonicSpectrum()  {}
    void update( int frameN );
};

//    xtract_lpc
//    xtract_lpcc
//    xtract_subbands

#endif
