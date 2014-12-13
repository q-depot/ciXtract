#ciXtract
ciXtract is a CinderBlock for [LibXtract](https://github.com/jamiebullock/LibXtract), a real-time audio feature extraction library developed by [Jamie Bullock](http://jamiebullock.com/).

##Notes

**Requires Cinder >= 0.8.6**

This CinderBlock includes the Xcode and VS2012 examples and the CinderBlock template.  
There are several issues related to LibXtract, some of the features are either inaccurate or not working yet.  
On Windows seems there is a problem witht the last bin in the Fft, I'll update the block as soon as these issues are fixed in LibXtract


##Get the code
LibXtract is included as headers + precompiled static library for OSX and Windows.

`cd CINDER_PATH/blocks`

`git clone git://github.com/q-depot/ciXtract.git`


##How to use it

```c++
// include the headers
#include "cinder/audio/Context.h"
#include "cinder/audio/MonitorNode.h"

#include "ciXtract.h"
#include "ciXtractUtilities.h"
```

```c++
ciXtractRef                     mXtract;			// declare a ciXtract object
vector<ciXtractFeatureRef>      mFeatures;			// declare a vector to save a reference of the features

audio::InputDeviceNodeRef       mInputDeviceNode;	// this is what you need to get the PCM buffer using the Cinder audio api
audio::MonitorNodeRef           mMonitorNode;
audio::Buffer                   mPcmBuffer;
```

```c++
void TestApp::setup()
{
	// Initialise the audio input
    auto ctx = audio::Context::master();

    vector<audio::DeviceRef> devices = audio::Device::getInputDevices();
    console() << "List audio devices:" << endl;
    for( size_t k=0; k < devices.size(); k++ )
        console() << devices[k]->getName() << endl;

    // find and initialise a device by name
     audio::DeviceRef dev;

     dev = audio::Device::findDeviceByName( "Soundflower (2ch)" );                              // on OSX i use Soundflower to hijack the system audio
     
     if ( !dev )                                                                                
         dev = audio::Device::findDeviceByName( "CABLE Output (VB-Audio Virtual Cable)" );      // on Windows there is similar tool called VB Cable
    
     if ( !dev )                                                                                // initialise default input device
        mInputDeviceNode = ctx->createInputDeviceNode();
     else
         mInputDeviceNode = ctx->createInputDeviceNode( dev );

    // initialise MonitorNode to get the PCM data
    auto monitorFormat = audio::MonitorNode::Format().windowSize( CIXTRACT_PCM_SIZE );
    mMonitorNode = ctx->makeNode( new audio::MonitorNode( monitorFormat ) );

    // pipe the input device into the MonitorNode
    mInputDeviceNode >> mMonitorNode;

    // InputDeviceNode (and all InputNode subclasses) need to be enabled()'s to process audio. So does the Context:
    mInputDeviceNode->enable();
    ctx->enable();


    // Initialise ciXtract
    mXtract     = ciXtract::create();
    mFeatures   = mXtract->getFeatures();

    // List all available features, this prints out the enumerator that can be used to get the feature
    mXtract->listFeatures();

    // Features are disabled by default, call enableFeature( feature_enum ) or enableAllFeatures()
    mXtract->enableAllFeatures();
}
```

```c++
void TestApp::update()
{
	mPcmBuffer = mMonitorNode->getBuffer();
    
    float pcmGain = 2.0f;

    if ( !mPcmBuffer.isEmpty() )								// ensure the PCM buffer exists
        mXtract->update( mPcmBuffer.getData(), pcmGain );		// update ciXtract, optionally you can pass the gain for the PCM signal
}
```

```c++
void TestApp::draw()
{
	// draw the PCM and a feature using the ciXtractUtilities

    ciXtractUtilities::drawPcm( Rectf( 0.0f, 0.0f, getWindowWidth(), 60.0f ), &mPcmBuffer );	// draw the PCM buffer

	ciXtractFeatureRef feature = mXtract->getFeature( XTRACT_SPECTRUM );						// get a feature
	ciXtractUtilities::drawData( feature, Rectf( 15, 15, 200, 90 ), mFont );					// draw the feature, you must pass a TextureFontRef
																								// optionally you can specify plot/bg/label colors
}
```

##About LibXtract
> LibXtract is a simple, portable, lightweight library of audio feature extraction functions. The purpose of the library is to provide a relatively exhaustive set of feature extraction primatives that are designed to be 'cascaded' to create a extraction hierarchies.

[LibXtract on github](https://github.com/jamiebullock/LibXtract)

[LIBXTRACT: A LIGHTWEIGHT LIBRARY FOR AUDIO FEATURE EXTRACTION](https://s3-eu-west-1.amazonaws.com/papers/LibXtract-_a_lightweight_feature_extraction_library.pdf)

[LibXtract documentation](http://jamiebullock.github.io/LibXtract/documentation/modules.html)


##License
The MIT License (MIT)

Copyright (c) 2014 Nocte Studio Ltd. - [www.nocte.co.uk](http://www.nocte.co.uk)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
