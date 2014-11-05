/*
 *  ciXtractFeature.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2014 . All rights reserved.
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
//    mMin = std::numeric_limits<double>::max();
//    mMax = std::numeric_limits<double>::lowest();
    
    mIsEnable       = false;
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
    float val;
    
    for( size_t i=0; i < mDataSize; i++ )
    {
//        mData.get()[i] = mOffset + mGain * mDataRaw.get()[i];
//        continue;
        
        val = mOffset + mGain * mDataRaw.get()[i];
        
        
        // clamp min-max range
//        val = ( mDataRaw.get()[i] - mMin ) / ( mMax - mMin );
//        val = mDataRaw.get()[i];

//        if ( mDataRaw.get()[i] > mMax )
//            mMax = mDataRaw.get()[i];
//        if ( mDataRaw.get()[i] < mMin )
//            mMin = mDataRaw.get()[i];
        
        val = ( val - mMin ) / ( mMax - mMin );
        
        // this function doesn't work properly.
        // val = min( (float)(i + 25) / (float)mResultsN, 1.0f ) * 100 * log10( 1.0f + val );

//        if ( mIsLog )
//            val = 0.01f * audio::linearToDecibel( val );
    
//        val = mOffset + mGain * val;
        
        val = math<float>::clamp( val, 0.0f, 1.0f );
        
        if ( mDamping > 0.0f )
        {
            if (  val >= mData.get()[i] )
                mData.get()[i] = val;
            else
                mData.get()[i] *= mDamping;
        }
        else
            mData.get()[i] = val;
    }
}


bool ciXtractFeature::checkDependencies( int frameN )
{
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
                console() << "ciXtractFeature::checkDependencies(): disable feature " << getName() << endl;
                mXtract->disableFeature( mFeature );
                return false;
            }
            mInputBuffer.data       = dep->getDataRaw();
            mInputBuffer.dataSize   = dep->getDataSize(); // dep->getBufferSize();
        }
        else
        {
            mInputBuffer.data       = mXtract->getPcmData();
            mInputBuffer.dataSize   = CIXTRACT_PCM_SIZE;
        }
    }
    
    // extra dependencies
    for( size_t k=0; k < mDependencies.size(); k++ )
    {
        if ( mDependencies[k] >= XTRACT_FEATURES )
            continue;
        
        dep = mXtract->getFeature( mDependencies[k] );
        if ( !dep )
        {
            // DEPENDENCY NOT FOUND! disable this feature and all the features that depend on it
            console() << "ciXtractFeature::checkDependencies(): feature " << getName() << "missing dependency " << mInputFeature << endl;
            console() << "ciXtractFeature::checkDependencies(): disable feature " << getName() << endl;
            mXtract->disableFeature( mFeature );
            return false;
        }
        
        if ( !dep->isUpdated( frameN ) )
            dep->update( frameN );
    }
    
    return true;
}

