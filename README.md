#ciXtract
ciXtract is a CinderBlock for LibXtract, a real-time audio feature extraction library developed by Jamie Bullock.

Status: Alpha

Requires Cinder >= 0.8.5

The source code includes an Xcode example and CinderBlock templates.
The code has been tested on OSX but it should work fine on Windows, the main difference is that LibXtract uses ooura on windows to generate the Fft.


##Get the code
LibXtract is included as a git submodule, to clone the respository add the "--recursive" option:

cd CINDER_PATH/blocks

git clone --recursive git://github.com/q-depot/ciXtract.git


##Notes
Some of the features are not fully working
##About LibXtract
LibXtract is a simple, portable, lightweight library of audio feature extraction functions. The purpose of the library is to provide a relatively exhaustive set of feature extraction primatives that are designed to be 'cascaded' to create a extraction hierarchies.

https://github.com/jamiebullock/LibXtract

#Todo

* implement cinder block xml
* add osc
* fix auto calibration
* damping
* use git submodule for LibXtract
* atof used for the gui should cast to double
* how to treat nan results?
* data capture and export(json/xml), this is also to compare features with sonic visualiser
* remove mDataN from features, it's pointless


#Issues

* Auto correlation Fft either crash or doesn't work
* Auto calibration doesn't always get the bouddaries right
* none of the F0 functions work below 100Hz

##Supported features


###Vector Features

###Scalar Features

