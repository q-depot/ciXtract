/*
 *  ciXtractReceiver.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


using namespace ci;
using namespace ci::app;
using namespace std;


#include "ciXtractReceiver.h"

ciXtractReceiver::ciXtractReceiver( uint32_t port ) : mPort(port)
{
    try
    {
        mOscListener.setup( mPort );
        
        mReceiveDataThread = std::thread( &ciXtractReceiver::receiveData, this );
    }
    catch ( ... )
    {
        console() << "ciXtractReceiver osc listener NOT initialised!" << endl;
    }
}


ciXtractReceiver::~ciXtractReceiver()
{
    mRunReceiveData = false;
    
    if ( mReceiveDataThread.joinable() )
        mReceiveDataThread.join();
    
    mOscListener.shutdown();
}


void ciXtractReceiver::receiveData()
{
    mRunReceiveData = true;
    
    while( mRunReceiveData )
    {
        
    }
}


