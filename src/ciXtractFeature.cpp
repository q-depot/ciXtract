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


ciXtractFeature::ciXtractFeature( ciXtract *xtract, xtract_features_ featureEnum, uint32_t bufferSize, xtract_features_ inputFeature, std::vector<xtract_features_> extraDependencies )
: mXtract(xtract), mFeatureEnum(featureEnum), mDataBufferSize(bufferSize), mInputFeatureEnum(inputFeature)
{
    mDataSize           = mDataBufferSize;
    mDependencies       = extraDependencies;
    
    if ( inputFeature < XTRACT_FEATURES )
        mDependencies.insert( mDependencies.begin(), inputFeature );
    
    mIsEnable           = false;
    mLastUpdateAt       = -1;
    mMin                = 0.0f;
    mMax                = 1.0f;
    mGain               = 1.0f;
    mOffset             = 0.0f;
    mDamping            = 0.96f;
    mIsLog              = false;
    mData               = DataBuffer( new double[mDataBufferSize] );
    mDataRaw            = DataBuffer( new double[mDataBufferSize] );
    
    for( uint32_t k=0; k < mDataBufferSize; k++ )
    {
        mData.get()[k]      = 0.0;
        mDataRaw.get()[k]   = 0.0;
    }
    
    console() << "ciXtractFeature() " << getName() << " | ";
    for( auto k=0; k < mDependencies.size(); k++ )
        console() << mDependencies[k] << " ";
    console() << endl;
}


bool ciXtractFeature::prepareUpdate( int frameN )
{
    if ( isUpdated(frameN) || !checkDependencies(frameN) )
        return false;
    
    for( size_t k=0; k < mParams.size(); k++ )
        mArgd[k] = mParams[k]->getValue();
    
    return true;
}


void ciXtractFeature::update( int frameN )
{
    if ( !prepareUpdate( frameN ) )
        return;
    
    ciXtractFeatureRef inputFeature = mXtract->getActiveFeature( mInputFeatureEnum );
    
    if ( getEnum() == XTRACT_PEAK_SPECTRUM )
    {
        console() << inputFeature->getName() << " " << inputFeature->getBufferSize() << endl;
    }
    
    doUpdate( frameN, inputFeature->getDataRaw().get(), inputFeature->getBufferSize(), mArgd, mDataRaw.get() );
}


void ciXtractFeature::doUpdate( int frameN )
{
    ciXtractFeatureRef inputFeature = mXtract->getActiveFeature( mInputFeatureEnum );
    
    doUpdate( frameN, inputFeature->getDataRaw().get(), inputFeature->getBufferSize(), &mArgd[0], mDataRaw.get() );
}


void ciXtractFeature::doUpdate( int frameN, const double *inputData, const int inputDataSize, const void *args, double *outputData )
{
    xtract[mFeatureEnum]( inputData, inputDataSize, args, outputData );
    
    processData();
    
    mLastUpdateAt = frameN;
}


void ciXtractFeature::updateWithPcm( int frameN )
{
    if ( !prepareUpdate(frameN) )
        return;

    doUpdate( frameN, mXtract->getPcmData().get(), CIXTRACT_PCM_SIZE, mArgd, mDataRaw.get() );
}


bool ciXtractFeature::checkDependencies( int frameN )
{
//    if( !mInputData || mInputDataSize == 0 )
//        return false;
    
    ciXtractFeatureRef dep;
    for( size_t k=0; k < mDependencies.size(); k++ )
    {
        dep = mXtract->getActiveFeature( mDependencies[k] );
        if ( !dep )
            return false;
        
        if ( !dep->isUpdated( frameN ) )
            dep->update( frameN );
    }
    return true;
}


void ciXtractFeature::processData()
{
    float val;
    
    for( size_t i=0; i < mDataSize; i++ )
    {
        // clamp min-max range
        val = ( mDataRaw.get()[i] - mMin ) / ( mMax - mMin );
        
        // this function doesn't work properly.
        // val = min( (float)(i + 25) / (float)mResultsN, 1.0f ) * 100 * log10( 1.0f + val );
        
        if ( mIsLog )
            val = 0.01f * audio::linearToDecibel( val );
        
        val = mOffset + mGain * val;
        
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

