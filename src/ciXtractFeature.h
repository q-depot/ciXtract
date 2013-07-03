/*
 *  ciXtract.h
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef CI_XTRACT_FEATURE
#define CI_XTRACT_FEATURE

#pragma once

#include "libxtract.h"

class ciXtractFeature;
typedef std::shared_ptr<ciXtractFeature>       ciXtractFeatureRef;


enum ciXtractFeatureType {
    CI_XTRACT_VECTOR,
    CI_XTRACT_SCALAR
};


class ciXtractFeature {

public:
    
    static ciXtractFeatureRef create( xtract_features_ feature, std::string name, ciXtractFeatureType type, std::function<void()> cb, std::vector<xtract_features_> dependencies, uint32_t n )
    {
        return ciXtractFeatureRef( new ciXtractFeature( feature, name, type, cb, dependencies, n ) );
    }
    
    void update()
    {
        if ( !mIsEnable )
            return;
        
        mCb();
    }
    
    bool isEnable() { return mIsEnable; }
    
    void enable( bool enable = true ) { mIsEnable = enable; }
    
    void toggle() { mIsEnable = !mIsEnable; }

    
private:
    
    ciXtractFeature( xtract_features_ feature, std::string name, ciXtractFeatureType type, std::function<void()> cb, std::vector<xtract_features_> dependencies, uint32_t n )
    {
        mFeature        = feature;
        mName           = name;
        mType           = type;
        mCb             = cb;
        mDependencies   = dependencies;
        mResultN        = n;
        mResultMin      = 0.0f;
        mResultMax      = 1.0f;
        mIsEnable       = false;
    }

    
private:
    
    xtract_features_                mFeature;
    std::string                     mName;
    ciXtractFeatureType             mType;
    std::function<void()>           mCb;
    std::vector<xtract_features_>   mDependencies;
    uint32_t                        mResultN;
    double                          mResultMin;
    double                          mResultMax;
    bool                            mIsEnable;
    
};

#endif
