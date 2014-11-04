/*
 *  ciXtractVectorFeature.h
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


// Spectrum
class ciXtractSpectrum : public ciXtractFeature {
    
public:
    ciXtractSpectrum( ciXtract *xtract, std::string name );
    ~ciXtractSpectrum() {}
    void doUpdate( int frameN );
    
};

// Autocorrelation
class ciXtractAutocorrelation : public ciXtractFeature {
    
public:
    ciXtractAutocorrelation( ciXtract *xtract, std::string name );
    ~ciXtractAutocorrelation() {}
    void doUpdate( int frameN );
    
};

// Autocorrelation Fft
class ciXtractAutocorrelationFft : public ciXtractFeature {
    
public:
    ciXtractAutocorrelationFft( ciXtract *xtract, std::string name );
    ~ciXtractAutocorrelationFft() {}
    void doUpdate( int frameN );
    
private:
    
};

// Harmonic Spectrum
class ciXtractHarmonicSpectrum : public ciXtractFeature {
    
public:
    ciXtractHarmonicSpectrum( ciXtract *xtract, std::string name );
    ~ciXtractHarmonicSpectrum() {}
    void doUpdate( int frameN );
    
private:
    
};

// Peak Spectrum
class ciXtractPeakSpectrum : public ciXtractFeature {
    
public:
    ciXtractPeakSpectrum( ciXtract *xtract, std::string name );
    ~ciXtractPeakSpectrum() {}
    void doUpdate( int frameN );
    
private:
    
};

// SubBands
class ciXtractSubBands : public ciXtractFeature {
    
public:
    ciXtractSubBands( ciXtract *xtract, std::string name );
    ~ciXtractSubBands() {}
    void doUpdate( int frameN );
    
private:
    
};

// Mffc
class ciXtractMfcc : public ciXtractFeature {
    
public:
    ciXtractMfcc( ciXtract *xtract, std::string name );
    ~ciXtractMfcc();
    void doUpdate( int frameN );
    
private:
    
    xtract_mel_filter   mMelFilters;
};

// Bark
class ciXtractBark : public ciXtractFeature {
    
public:
    ciXtractBark( ciXtract *xtract, std::string name );
    ~ciXtractBark() {}
    void doUpdate( int frameN );
    
private:
    
    std::shared_ptr<int>    mBandLimits;
    
};

#endif
