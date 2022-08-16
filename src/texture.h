/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _TEXTURELOADER_H
#define _TEXTURELOADER_H

#include <iostream>
#include <string>
#include <vector>
#include "SDL.h"
#include "SDL_opengl.h"

class Texture
{
  public:
    Texture() : m_loaded(false), m_glloaded(false) {}
  
    // Copy constructor
    Texture(const Texture & tex);
    
    Texture(const std::string & filename) 
    : m_loaded(false), m_glloaded(false)
    {
      load(filename);
    }

    ~Texture()
      { unload(); }
    
    // Assignment operator
    Texture & operator= (const Texture & tex);
    
    bool load(const std::string & filename);

    // Sets the texture as the current OpenGL 2D texture
    void use() const;

    // Unload texture data (not thread safe)
    void unload();

    // Load texture into OpenGL (not thread safe)
    void loadGL();

    // Free all OpenGL resources (not thread safe)
    void unloadGL();

    int getWidth();

    int getHeight();

    bool isLoaded();

    void createSurface(int width, int height);

    void setPixelFormat(SDL_PixelFormatEnum pixelformat);

    SDL_Surface * getSurface();

    GLenum getSurfaceFormat();

  protected:
    GLuint loadSurface( SDL_Surface * surf );

    unsigned int nearestPow2( unsigned int n );

    // Swap for assignment operator
    void swap(Texture & tex);
    
    SDL_Surface *m_img;

    GLuint m_texture;

    bool m_loaded, m_glloaded;

  
};
#endif
