/*
 *  ciXtractReceiver.h
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef CI_XTRACT_RECEIVER
#define CI_XTRACT_RECEIVER

#pragma once

#include "cinder/Thread.h"
#include "OscListener.h"

//#include "libxtract.h"
//#include "cinder/audio/Input.h"
//
//#include "cinder/gl/TextureFont.h"
//#include "ciXtractFeature.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class ciXtractReceiver;
typedef std::shared_ptr<ciXtractReceiver>   ciXtractReceiverRef;


class ciXtractReceiver {

public:
    
    static ciXtractReceiverRef create( uint32_t port = 8000 ) { return ciXtractReceiverRef( new ciXtractReceiver( port ) ); }
    
    ~ciXtractReceiver();
    
//    void enableFeature( xtract_features_ feature );
//    
//    void disableFeature( xtract_features_ feature );
//    
//    void toggleFeature( xtract_features_ feature );
//    
//    std::vector<ciXtractFeatureRef> getFeatures() { return mFeatures; };
//    
//    ciXtractFeatureRef getFeature( xtract_features_ feature );

    
private:
    
    ciXtractReceiver( uint32_t port );
    
    void receiveData();
    
    
private:
    
	osc::Listener 	mOscListener;
    uint32_t        mPort;
    std::thread     mReceiveDataThread;
    bool            mRunReceiveData;
    
};

#endif
