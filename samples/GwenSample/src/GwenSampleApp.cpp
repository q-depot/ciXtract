/*
 *  ciLibXtract GwenSampleApp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/params/Params.h"
#include "cinder/Utilities.h"

#include "ciLibXtract.h"

#include "cigwen/GwenRendererGl.h"
#include "cigwen/GwenInput.h"
#include "cigwen/CinderGwen.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/WindowControl.h"
#include "Gwen/Controls/CollapsibleList.h"
#include "Gwen/Controls/CheckBox.h"
#include "Gwen/Events.h"


#include "ScalarControl.h"


using namespace ci;
using namespace ci::app;
using namespace std;


gl::TextureFontRef      mFontSmall;
gl::TextureFontRef      mFontMedium;
gl::TextureFontRef      mFontBig;



class GwenSampleApp : public AppNative, public Gwen::Event::Handler {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();
    
    void drawVectorData( string label, std::shared_ptr<double> data, int N, bool enable, float gain, Rectf rect, bool clamp );
    
    void drawScalarData( string label, double val, bool enable, float gain, Vec2f pos, bool clamp );
    
    void initGui();
    
    void toggleAllFeatures( bool enable );

    void toggleFeature( Gwen::Event::Info info );
    
    audio::Input            mInput;
    ciLibXtractRef          mLibXtract;
    
    params::InterfaceGlRef  mParams;
    
    float                   mGain[XTRACT_FEATURES];
    
    
	cigwen::GwenRendererGl  *mRenderer;
	cigwen::GwenInputRef    mGwenInput;
	Gwen::Controls::Canvas  *mCanvas;
    Gwen::Controls::WindowControl* mWindow;
};


void GwenSampleApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1200, 880 );
}


void GwenSampleApp::setup()
{
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
    
    mLibXtract = ciLibXtract::create( mInput );
    mLibXtract->enableFeature( XTRACT_SPECTRUM );
    
    for( auto k=0; k < XTRACT_FEATURES; k++ )
        mGain[k] = 1.0f;
    
    initGui();
    
    mFontSmall  = gl::TextureFont::create( Font( "Helvetica", 12 ) );
    mFontMedium = gl::TextureFont::create( Font( "Helvetica", 14 ) );
    mFontBig    = gl::TextureFont::create( Font( "Helvetica", 18 ) );
    
    //    setFullScreen(true, FullScreenOptions().kioskMode() );
}


void GwenSampleApp::mouseDown( MouseEvent event )
{
}


void GwenSampleApp::update()
{
    mLibXtract->update();
}


void GwenSampleApp::draw()
{
	// clear out the window with black
//	gl::clear( Color( 0.93f, 0.94f, 0.95f ) );
	gl::clear( Color( 0.91f, 0.94f, 0.96f ) );
    
    gl::enableAlphaBlending();
    
    int fftSize = PCM_SIZE >> 1;
    
    std::vector<ciLibXtract::FeatureCallback>::iterator it;
    
    Vec2f scalarDataOffset( 25, 15 );
    float margin = 35;
    string name;
    Rectf vectorDataOffset( 480, 15, 760, 100 );
    for( it = mLibXtract->mCallbacks.begin(); it != mLibXtract->mCallbacks.end(); ++it )
    {
        name = it->name; name.erase( name.begin(), name.begin() + 7 );
        
        if ( it->type == ciLibXtract::SCALAR_FEATURE )
        {
//            drawScalarData( name, mLibXtract->getScalarFeature( it->feature ), it->enable, 1.0f, scalarDataOffset, true );
//            scalarDataOffset += Vec2f( 0, margin );
        }
        else if ( it->feature == XTRACT_SPECTRUM || it->feature == XTRACT_PEAK_SPECTRUM || it->feature == XTRACT_HARMONIC_SPECTRUM )
        {
            drawVectorData( name, mLibXtract->getVectorFeature( it->feature ), fftSize, it->enable, 1.0f, vectorDataOffset, true );
            vectorDataOffset += Vec2f( 0, 15 + vectorDataOffset.getHeight() );
        }
        
        else if ( it->feature == XTRACT_BARK_COEFFICIENTS )
        {
            drawVectorData( name, mLibXtract->getVectorFeature( it->feature ), XTRACT_BARK_BANDS, it->enable, 1.0f, vectorDataOffset, true );
            vectorDataOffset += Vec2f( 0, 15 + vectorDataOffset.getHeight() );
        }
        
        else if ( it->feature == XTRACT_MFCC )
        {
            drawVectorData( name, mLibXtract->getVectorFeature( it->feature ), MFCC_FREQ_BANDS, it->enable, 1.0f, vectorDataOffset, true );
            vectorDataOffset += Vec2f( 0, 15 + vectorDataOffset.getHeight() );
        }
        
        else if ( it->feature == XTRACT_AUTOCORRELATION )
        {
            drawVectorData( name, mLibXtract->getVectorFeature( it->feature ), PCM_SIZE, it->enable, 1.0f, vectorDataOffset, true );
            vectorDataOffset += Vec2f( 0, 15 + vectorDataOffset.getHeight() );
        }
        
        else if ( it->feature == XTRACT_AUTOCORRELATION_FFT )
        {
            drawVectorData( name, mLibXtract->getVectorFeature( it->feature ), PCM_SIZE, it->enable, 1.0f, vectorDataOffset, true );
            vectorDataOffset += Vec2f( 0, 15 + vectorDataOffset.getHeight() );
        }
        
        else if ( it->feature == XTRACT_SUBBANDS )
        {
            drawVectorData( name, mLibXtract->getVectorFeature( it->feature ), SUBBANDS_N, it->enable, 1.0f, vectorDataOffset, true );
            vectorDataOffset += Vec2f( 0, 15 + vectorDataOffset.getHeight() );
        }
        
        if ( scalarDataOffset.y > getWindowHeight() - 45 )
        {
            scalarDataOffset.y = 25;
            scalarDataOffset.x += 230;
        }
    }
    
	mCanvas->RenderCanvas();
    
//    mParams->draw();
}


void GwenSampleApp::drawVectorData( string label, std::shared_ptr<double> data, int N, bool enable, float gain, Rectf rect, bool clamp )
{
    int padding = 5;
    
    Color col = enable ? Color::white() : Color::gray(0.5f);
    
    glPushMatrix();
    
    gl::color( col * 0.2f );
    gl::drawSolidRect( rect );
    
    rect.inflate( - Vec2i::one() * padding );
    gl::translate( rect.getUpperLeft() );
    
    glBegin( GL_QUADS );
    
    float step = rect.getWidth() / N;
    float h = rect.getHeight();
    
    for( int i = 0; i < N; i++ )
    {
		float barY = data.get()[i] * gain;
        
        if ( clamp )
            barY = math<float>::clamp( barY, 0.0f, h );
        
        gl::color( col );
        glVertex2f( i * step,           h );
        glVertex2f( ( i + 1 ) * step,   h );
        glVertex2f( ( i + 1 ) * step,   h-barY );
        glVertex2f( i * step,           h-barY );
	}
    
    glEnd();
    
    gl::color( col );
    mFontSmall->drawString( label, Vec2f( 5, 10 ) );
    
    gl::popMatrices();
}


void GwenSampleApp::drawScalarData( string label, double val, bool enable, float gain, Vec2f pos, bool clamp )
{
    float h         = 5;
    float w         = 200;
    float margin    = 10;
    
    ColorA col = ColorA::black();

    if ( !enable )
        col.a = 0.3f;
    
    val *= gain;
    
    if ( clamp )
        val = math<float>::clamp( val, 0.0f, 1.0f );
    
    glPushMatrix();
    
    gl::translate( pos );
    
    gl::color( col );
    mFontSmall->drawString( label, Vec2f::zero() );
    
    glBegin( GL_QUADS );
    
    gl::color( col * 0.1f );
    glVertex2f( 0, margin );
    glVertex2f( w, margin );
    glVertex2f( w, margin + h );
    glVertex2f( 0, margin + h );
    
    gl::color( col );
    glVertex2f( 0, margin );
    glVertex2f( w * val, margin );
    glVertex2f( w * val, margin + h );
    glVertex2f( 0, margin + h );
    
    glEnd();
    
    gl::popMatrices();
}



void GwenSampleApp::initGui()
{
    // TODO: find a better way.. WTF is this for?
	fs::path rootPath = getAppPath().parent_path().parent_path().parent_path().parent_path().parent_path().parent_path();
	addAssetDirectory( rootPath / "assets" );
    
	mRenderer = new cigwen::GwenRendererGl();
	mRenderer->Init();
    
	Gwen::Skin::TexturedBase* skin = new Gwen::Skin::TexturedBase( mRenderer );
	skin->Init( "DefaultSkin.png" );
    
	mCanvas = new Gwen::Controls::Canvas( skin );
	mCanvas->SetSize( getWindowWidth(), getWindowHeight() );
	mGwenInput = cigwen::GwenInput::create( mCanvas );
    
    Vec2f offset( 15, 15 );
    
//    ScalarControl *control = new ScalarControl( mCanvas, "IRREGULARITY_K", mLibXtract->findFeatureCbRef( XTRACT_IRREGULARITY_K ), mLibXtract );
//    control->SetPos( offset.x, offset.y );

    
    // Features toggle
    std::vector<ciLibXtract::FeatureCallback>::iterator itr;
    for( itr = mLibXtract->mCallbacks.begin(); itr != mLibXtract->mCallbacks.end(); ++itr )
    {
        if ( itr->type == ciLibXtract::SCALAR_FEATURE )
        {
            // Custom Control
            ScalarControl *control = new ScalarControl( mCanvas, itr->name, &(*itr), mLibXtract );
            control->SetPos( offset.x, offset.y );
            
            offset.y += SCALAR_CONTROL_HEIGHT + 25;

            if ( offset.y >= getWindowHeight() - SCALAR_CONTROL_HEIGHT )
            {
                offset.x += SCALAR_CONTROL_WIDTH + 15;
                offset.y = 15;
            }
        }
    }

}

//    mParams->addButton( "ALL ON", std::bind( &GwenSampleApp::toggleAllFeatures, this, true ) );
//    mParams->addButton( "ALL OFF", std::bind( &GwenSampleApp::toggleAllFeatures, this, false ) );
//    mParams->addSeparator();
//    std::vector<ciLibXtract::FeatureCallback>::iterator it;
//    string name;
//    for( it = mLibXtract->mCallbacks.begin(); it != mLibXtract->mCallbacks.end(); ++it )
//    {
//        name = it->name; name.erase( name.begin(), name.begin()+7 );
//        mParams->addButton( name, std::bind( &ciLibXtract::toggleFeature, mLibXtract, it->feature ) );
//    }


void GwenSampleApp::toggleFeature( Gwen::Event::Info info )
{
    if ( info.Packet->Integer == -1 )
        toggleAllFeatures(true);
    else if ( info.Packet->Integer == -2 )
        toggleAllFeatures(false);
    else
    {
        mLibXtract->toggleFeature( (xtract_features_)info.Packet->Integer );
    }
}


void GwenSampleApp::toggleAllFeatures( bool enable )
{
    std::vector<ciLibXtract::FeatureCallback>::iterator it;
    for( it = mLibXtract->mCallbacks.begin(); it != mLibXtract->mCallbacks.end(); ++it )
        if ( enable )
            mLibXtract->enableFeature( it->feature );
        else
            mLibXtract->disableFeature( it->feature );
}

CINDER_APP_NATIVE( GwenSampleApp, RendererGl )


