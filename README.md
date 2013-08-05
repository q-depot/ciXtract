#ciXtract
ciXtract is a CinderBlock for [LibXtract](https://github.com/jamiebullock/LibXtract), a real-time audio feature extraction library developed by [Jamie Bullock](http://jamiebullock.com/).


##Notes
Status: Alpha

Requires Cinder >= 0.8.5

The source code includes an Xcode example and CinderBlock templates.
The code has been tested on OSX only, but it should work on Windows too, the main difference is that LibXtract uses ooura on windows to get the Fft.


##Get the code
LibXtract is included as a git submodule, to clone the respository use the "--recursive" option:

cd CINDER_PATH/blocks

git clone --recursive git://github.com/q-depot/ciXtract.git


##How to use it
A working example can be found in the Samples folder, you can also use the TinderBox template to generate a new one.

```c++
#include "ciXtract.h"

// ...

audio::Input                	mInput;
ciXtractRef                 	mXtract;
vector<ciXtractFeatureRef>  	mFeatures;

// ...
	
void ciXtractBasicRenderApp::setup()
{
	// Initialise the audio input
    const std::vector<audio::InputDeviceRef>& devices = audio::Input::getDevices();
	for( std::vector<audio::InputDeviceRef>::const_iterator iter = devices.begin(); iter != devices.end(); ++iter )
    {
        if ( (*iter)->getName() == "Soundflower (2ch)" )
        {
            mInput = audio::Input( *iter );
            mInput.start();
            break;
        }
	}
 
    if ( !mInput )
        exit(-1);
 
	// initialise ciXtract
    mXtract     = ciXtract::create( mInput );
	
	// ciXtract always have a reference to the feature even if it's not enable(the values are simply not updated)
    mFeatures   = mXtract->getFeatures();
    
	// By default all the features are disable.
	// you can use enableFeature( xtract_features_ feature ) to enable each feature and its own dependencies
	// xtract_features_ is defined libxtract.h
	mXtract->enableFeature( XTRACT_SPECTRUM );
	
	// .. Or you can enable all the features using XTRACT_FEATURES, also defined in libxtract.h
    // for( auto k=0; k < XTRACT_FEATURES; k++ )
    //   mXtract->enableFeature( (xtract_features_)k );
}


void ciXtractBasicRenderApp::update()
{
	mXtract->update();
}


void ciXtractBasicRenderApp::draw()
{
	// ...
	
    ciXtractFeatureRef  feature;
    
    for( auto k=0; k < mFeatures.size(); k++ )
    {
		/*
        feature = mFeatures[k];
		feature->getType()
		feature->isEnable();
		feature->getName();
		feature->getResult();
		feature->getResultN();
		*/
		
		// do something ..
    }
}
```

##Data results and auto calibration
ciXtract always return the raw data.
I've added a simple auto calibration function that samples the results for 3 seconds and set the maximum and minimum values. This is a bit rough and doesn't always work properly, also some features like F0 don't need to be calibrated, this is more a quick way to display something on screen. It's usually better to manually find and set the range and use two values gain and offset to adjust the results.

```c++
mXtract->calibrateFeatures();	// calibrate all features

or

mXtract->calibrateFeature( XTRACT_SPECTRUM );

..

float min = feature->getResultMin();
float max = feature->getResultMax();
```

##Samples
####BasicSample
This sample is generated using the ciXtractBasicRender template and it shows all the available features in ciXtract.

####XtractSenderOSCApp
This is simple app that implements ciXtract and send the results via OSC using the feature name as OSC address. This app doesn't visualise the results, it's meant to be a lightweight components running in the background crunching numbers, it's up to the OSC recipient to adjust(gain, offset, damping etc..) and visualise the data.
The app comes with a xml settings file(assets/default.xml) which can be used to configure OSC(host, port), input device and enable or disable features.


##About LibXtract
> LibXtract is a simple, portable, lightweight library of audio feature extraction functions. The purpose of the library is to provide a relatively exhaustive set of feature extraction primatives that are designed to be 'cascaded' to create a extraction hierarchies.

[LibXtract on github](https://github.com/jamiebullock/LibXtract)

[LIBXTRACT: A LIGHTWEIGHT LIBRARY FOR AUDIO FEATURE EXTRACTION](https://s3-eu-west-1.amazonaws.com/papers/LibXtract-_a_lightweight_feature_extraction_library.pdf)

##Supported features

###Vector Features
* XTRACT_SPECTRUM
* XTRACT_AUTOCORRELATION
* ~~XTRACT_AUTOCORRELATION_FFT~~ Doesn't work properly, don't use it.
* XTRACT_HARMONIC_SPECTRUM
* XTRACT_PEAK_SPECTRUM
* XTRACT_SUBBANDS
* XTRACT_MFCC
* XTRACT_BARK_COEFFICIENTS

###Scalar Features

* XTRACT_F0
* XTRACT_FAILSAFE_F0
* XTRACT_WAVELET_F0
* XTRACT_MEAN
* XTRACT_VARIANCE
* XTRACT_STANDARD_DEVIATION
* XTRACT_AVERAGE_DEVIATION
* XTRACT_SKEWNESS
* XTRACT_KURTOSIS
* XTRACT_SPECTRAL_MEAN
* XTRACT_SPECTRAL_VARIANCE
* XTRACT_SPECTRAL_STANDARD_DEVIATION
* XTRACT_SPECTRAL_SKEWNESS
* XTRACT_SPECTRAL_KURTOSIS
* XTRACT_SPECTRAL_CENTROID
* XTRACT_IRREGULARITY_K
* XTRACT_IRREGULARITY_J
* XTRACT_TRISTIMULUS_1
* XTRACT_SMOOTHNESS
* XTRACT_SPREAD
* XTRACT_ZCR
* XTRACT_ROLLOFF
* XTRACT_LOUDNESS
* XTRACT_FLATNESS
* XTRACT_FLATNESS_DB
* XTRACT_TONALITY
* XTRACT_RMS_AMPLITUDE
* XTRACT_SPECTRAL_INHARMONICITY
* XTRACT_POWER
* XTRACT_ODD_EVEN_RATIO
* XTRACT_SHARPNESS
* XTRACT_SPECTRAL_SLOPE
* XTRACT_LOWEST_VALUE
* XTRACT_HIGHEST_VALUE
* XTRACT_SUM
* XTRACT_NONZERO_COUNT
* XTRACT_CREST


#Known Issues

* Auto correlation Fft doesn't work properly and crash often
* F0 functions don't work below 100Hz, this seems to be a limit in LibXtract depending on the resolution
* Auto-calibration sucks