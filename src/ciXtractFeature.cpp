/*
 *  ciXtractFeature.cpp
 *
 *  Created by Andrea Cuius
 *  The MIT License (MIT)
 *  Copyright (c) 2014 Nocte Studio Ltd.
 *
 *  www.nocte.co.uk
 *
 */

#include "cinder/audio/Utilities.h"
#include "ciXtractFeature.h"
#include "ciXtract.h"

using namespace ci;
using namespace ci::app;
using namespace std;

ciXtractFeature::ciXtractFeature( ciXtract *xtract, xtract_features_ feature, std::string name, xtract_features_ inputFeature, uint32_t resultsN, int dataBufferSize )
: mXtract(xtract), mFeature(feature), mName(name), mInputFeature(inputFeature), mDataSize(resultsN)
{
    mMin            = 0.0f;
    mMax            = 1.0f;
    mIsEnable       = false;
    mIsInit         = false;
    mGain           = 1.0f;
    mOffset         = 0.0f;
    mDamping        = 0.96f;
    mIsLog          = false;
    mLastUpdateAt   = -1;
    mDataBufferSize = dataBufferSize;
    
    if ( mDataBufferSize < 0.0 )
        mDataBufferSize = resultsN;
    
    mDataRaw = std::shared_ptr<double>( new double[mDataBufferSize] );
    mData    = std::shared_ptr<double>( new double[mDataSize] );
    
    for( uint32_t k=0; k < mDataSize; k++ )
    {
        mDataRaw.get()[k]   = 0.0;
        mData.get()[k]      = 0.0;
    }
    
    mDependencies.push_back( inputFeature );
}


void ciXtractFeature::update( int frameN )
{
    // check dependencies
    if ( isUpdated(frameN) || !checkDependencies( frameN ) )
        return;
    
    // call sub-class update
    doUpdate( frameN );
    
    mLastUpdateAt = frameN;
    
    processData( frameN );
}


void ciXtractFeature::processData( int frameN )
{
    double val;
    
    for( size_t i=0; i < mDataSize; i++ )
    {
        /*
        val = mDataRaw.get()[i];
        
        if ( mIsLog )
            val = 0.005f * audio::linearToDecibel( val );
            
        val = mOffset + mGain * val;
        
        */
        
        val = mGain * mDataRaw.get()[i];
        
        if ( mIsLog )
            val = 0.005 * audio::linearToDecibel( val );
        
        val     += mOffset;
        val     = ( val - mMin ) / ( mMax - mMin );
        val     = math<double>::clamp( val, 0.0, 1.0 );
        
        if ( mDamping > 0.0f && val < mData.get()[i] )
            mData.get()[i] = std::max( 0.0, mData.get()[i] - mDamping );
        else
            mData.get()[i] = val;
    }
}


bool ciXtractFeature::checkDependencies( int frameN )
{
    ciXtractFeatureRef dep;
    
    if ( mInputFeature != CIXTRACT_NO_FEATURE && !mInputBuffer.data )
    {
        console() << "ciXtractFeature::checkDependencies(): feature " << getName() << "missing input " << mInputFeature << endl;
        mXtract->disableFeature( mFeature );
        return false;
    }
    
    // extra dependencies
    for( size_t k=0; k < mDependencies.size(); k++ )
    {
        if ( mDependencies[k] >= XTRACT_FEATURES )
            continue;
        
        dep = mXtract->getFeature( mDependencies[k] );
        
        if ( !dep )                                         // DEPENDENCY NOT FOUND! disable this feature and all the features that depend on it
        {
            console() << "ciXtractFeature::checkDependencies(): feature " << getName() << "missing dependency " << mInputFeature << endl;
            mXtract->disableFeature( mFeature );
            return false;
        }
        
        if ( !dep->isUpdated( frameN ) )
            dep->update( frameN );
    }
    
    return true;
}


void ciXtractFeature::enable( bool isEnable )
{
    mIsEnable = isEnable;
    
    if ( mIsEnable )    console() << "ciXtract enable feature: " << getName() << endl;
    else                console() << "ciXtract disable feature: " << getName() << endl;
    
    if ( !mIsEnable || mIsInit )
        return;
    
    mIsInit = true;
    
    ciXtractFeatureRef dep;

    // input data
    if ( mInputFeature != CIXTRACT_NO_FEATURE && !mInputBuffer.data )
    {
        if ( mInputFeature < XTRACT_FEATURES )
        {
            dep = mXtract->getFeature( mInputFeature );
            if ( !dep )
            {
                // INPUT NOT FOUND! disable this feature and all the features that depend on it
                console() << "ciXtractFeature::checkDependencies(): feature " << getName() << "missing input " << mInputFeature << endl;
                mXtract->disableFeature( mFeature );
            }
            mInputBuffer.data       = dep->getDataRaw();
            mInputBuffer.dataSize   = dep->getDataSize();
        }
        else
        {
            mInputBuffer.data       = mXtract->getPcmData();
            mInputBuffer.dataSize   = CIXTRACT_PCM_SIZE;
        }
    }
}
