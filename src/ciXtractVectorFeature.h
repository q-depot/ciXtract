/*
 *  ciXtractVectorFeature.h
 *
 *  Created by Andrea Cuius
 *  The MIT License (MIT)
 *  Copyright (c) 2014 Nocte Studio Ltd.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef CI_XTRACT_VECTOR_FEATURE
#define CI_XTRACT_VECTOR_FEATURE

#pragma once

#include "ciXtractFeature.h"

//    spectrum
//    autocorrelation_fft
//    mfcc
//    dct
//    autocorrelation
//    amdf
//    asdf
//    bark_coefficients
//    peak_spectrum
//    harmonic_spectrum
//    lpc
//    lpcc
//    subban


// Spectrum
class ciXtractSpectrum : public ciXtractFeature {
    
public:
    ciXtractSpectrum( ciXtract *xtract, std::string name );
    ~ciXtractSpectrum() {}
    void doUpdate( int frameN );
    void enable( bool isEnable );
};


// Autocorrelation Fft
class ciXtractAutocorrelationFft : public ciXtractFeature {
    
public:
    ciXtractAutocorrelationFft( ciXtract *xtract, std::string name );
    ~ciXtractAutocorrelationFft() {}
    void doUpdate( int frameN );
    void enable( bool isEnable );
};


// Mffc
class ciXtractMfcc : public ciXtractFeature {
    
public:
    ciXtractMfcc( ciXtract *xtract, std::string name );
    ~ciXtractMfcc();
    void doUpdate( int frameN );
    void enable( bool isEnable );
    
private:
    xtract_mel_filter   mMelFilters;
};


// Dct
class ciXtractDct : public ciXtractFeature {
    
public:
    ciXtractDct( ciXtract *xtract, std::string name );
    ~ciXtractDct() {}
    void doUpdate( int frameN );
    void enable( bool isEnable );
};


// Autocorrelation
class ciXtractAutocorrelation : public ciXtractFeature {
    
public:
    ciXtractAutocorrelation( ciXtract *xtract, std::string name );
    ~ciXtractAutocorrelation() {}
    void doUpdate( int frameN );
    void enable( bool isEnable );
};


// Amdf
class ciXtractAmdf : public ciXtractFeature {
    
public:
    ciXtractAmdf( ciXtract *xtract, std::string name );
    ~ciXtractAmdf() {}
    void doUpdate( int frameN );
};


// Asdf
class ciXtractAsdf : public ciXtractFeature {
    
public:
    ciXtractAsdf( ciXtract *xtract, std::string name );
    ~ciXtractAsdf() {}
    void doUpdate( int frameN );
};


// Bark
class ciXtractBark : public ciXtractFeature {
    
public:
    ciXtractBark( ciXtract *xtract, std::string name );
    ~ciXtractBark() {}
    void doUpdate( int frameN );
    void enable( bool isEnable );
    
private:
    std::shared_ptr<int>    mBandLimits;
};


// Peak Spectrum
class ciXtractPeakSpectrum : public ciXtractFeature {
    
public:
    ciXtractPeakSpectrum( ciXtract *xtract, std::string name );
    ~ciXtractPeakSpectrum() {}
    void doUpdate( int frameN );
};


// Harmonic Spectrum
class ciXtractHarmonicSpectrum : public ciXtractFeature {
    
public:
    ciXtractHarmonicSpectrum( ciXtract *xtract, std::string name );
    ~ciXtractHarmonicSpectrum() {}
    void doUpdate( int frameN );
};


// Lpc
class ciXtractLpc : public ciXtractFeature {
    
public:
    ciXtractLpc( ciXtract *xtract, std::string name );
    ~ciXtractLpc() {}
    void doUpdate( int frameN );
};

//    lpcc

// SubBands
class ciXtractSubBands : public ciXtractFeature {
    
public:
    ciXtractSubBands( ciXtract *xtract, std::string name );
    ~ciXtractSubBands() {}
    void doUpdate( int frameN );
};


#endif
