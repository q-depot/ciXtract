/*
 *  ciXtractUtilities.h
 *
 *  Created by Andrea Cuius
 *  The MIT License (MIT)
 *  Copyright (c) 2014 Nocte Studio Ltd.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef CI_XTRACT_UTILITIES
#define CI_XTRACT_UTILITIES

#pragma once

namespace ciXtractUtilities {
    
    //! draw the PCM waveform
    void drawPcm( ci::Rectf rect, audio::Buffer *buff )
    {
        int pcmSize = buff->getSize() / buff->getNumChannels();

        gl::pushMatrices();
        
        gl::translate( rect.x1, rect.y1 + rect.getHeight() * 0.5f );
        
        float   scale   = rect.getWidth() / (float)pcmSize;
        float   x, y;
        
        PolyLine<Vec2f>	leftBufferLine;
        
        gl::color( Color::gray( 0.4f ) );
        
        for( int i = 0; i < pcmSize; i++ )
        {
            x = i * scale;
            y = buff->getData()[i] * rect.getHeight() * 0.5f;
            leftBufferLine.push_back( Vec2f( x , y) );
        }
        
        gl::draw( leftBufferLine );
        
        gl::popMatrices();
    }


    //! draw the feature data
    void drawData(   ciXtractFeatureRef  feature,
                            Rectf               rect,
                            gl::TextureFontRef  font,
                            ci::ColorA          plotCol     = ci::ColorA::white(),
                            ci::ColorA          bgCol       = ci::ColorA( 1.0f, 1.0f, 1.0f, 0.1f ),
                            ci::ColorA          labelCol    = ci::ColorA::white() )
    {
        glPushMatrix();
        
        gl::color( labelCol );
        
        font->drawString( feature->getName(), rect.getUpperLeft() );
        
        rect.y1 += 10;
        
        std::shared_ptr<double> data    = feature->getData();
        float                   step    = rect.getWidth() / feature->getDataSize();
        float                   h       = rect.getHeight();
        float                  val, barY;
        
        gl::color( bgCol );
        gl::drawSolidRect( rect );
        
        gl::translate( rect.getUpperLeft() );
        
        gl::color( plotCol );
        
        glBegin( GL_QUADS );
        
        for( size_t i = 0; i < feature->getDataSize(); i++ )
        {
            val     = (float)data.get()[i];
            val     = math<float>::clamp( val, 0.0f, 1.0f );
            barY    = h * val;
            
            glVertex2f( i * step,           h );
            glVertex2f( ( i + 1 ) * step,   h );
            glVertex2f( ( i + 1 ) * step,   h-barY );
            glVertex2f( i * step,           h-barY );
        }
        
        glEnd();
        
        gl::popMatrices();
    }

};

#endif
