/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : texture.cpp
 * Authors : Joe Flint, Bencsik Roland
 * Description: OpenGL Texture loading with the help of SDL_Image
 **************************************************************************/

#include "texture.h"
#include "SDL_image.h"

#define d1printf(...)
//#define d1printf(...) printf("%s:%d:", __FUNCTION__, __LINE__); printf(__VA_ARGS__);

typedef void (APIENTRY *PFNGLGENERATEMIPMAP)(GLenum target);

using namespace std;

// Copy Constructor
Texture::Texture(const Texture & tex)
{
  if(tex.m_loaded) {
    m_img = tex.m_img;
    tex.m_img->refcount++;
  }
  m_texture = tex.m_texture;
  m_loaded = tex.m_loaded;
  m_glloaded = tex.m_glloaded;
}

// Assignment operator
Texture& Texture::operator= (const Texture & tex)
{
  Texture temp(tex);
  temp.swap(*this);
  return *this;
}

bool Texture::load( const string & filename )
{
  // Don't try to load if already loaded
  // Can't just call unload because it isn't thread safe
  if(m_loaded) {
    return false;
  }

  if (filename == "")
  {
    return false;
  }
  
  // Name is ok, now try to load the image data from the file
  m_img = IMG_Load( filename.c_str() );

  if( m_img == NULL ) {
    // Image file failed to load for some reason
    return false;
  }

  // Make sure the size is a power of two
  if( nearestPow2( m_img->w ) != m_img->w 
      || nearestPow2( m_img->h ) != m_img->h ) {
    SDL_FreeSurface( m_img );
    return false;
  }

  m_loaded = true;
  return true;
}

void Texture::unload()
{
  if(!m_loaded)
    return;

  unloadGL();
  
  // Free the image data
  SDL_FreeSurface( m_img );

  m_loaded = false;
}

void Texture::createSurface(int width, int height)
{
  if(m_loaded) {
    return;
  }

  m_img =
    SDL_CreateRGBSurfaceWithFormat(
      0,
      width,
      height,
      32,
      SDL_PIXELFORMAT_BGRA32
    );

  m_loaded = true;
}

void Texture::setPixelFormat(SDL_PixelFormatEnum pixelformat)
{
  SDL_Surface * surface;

  if(! m_loaded) {
    return;
  }

  if (m_img->format->format == pixelformat)
  {
    return;
  }

  surface =
    SDL_CreateRGBSurfaceWithFormat(
      0,
      m_img->w,
      m_img->h,
      32,
      pixelformat
    );

  SDL_BlitSurface(m_img, NULL, surface, NULL);

  SDL_FreeSurface(m_img);

  m_img = surface;
}

SDL_Surface * Texture::getSurface()
{
  return m_img;
}

void Texture::use() const
{
  if(!m_loaded || !m_glloaded)
    return;
  glBindTexture( GL_TEXTURE_2D, m_texture );
}

void Texture::loadGL()
{
  if(!m_loaded || m_glloaded)
    return;

  // Make sure texturing isn't erroneously disabled
  glPushAttrib(GL_ENABLE_BIT);
  
  // Load the image data into an OpenGL texture
  glEnable(GL_TEXTURE_2D);
  m_texture = loadSurface(m_img);

  glPopAttrib();
  m_glloaded = true;
}

void Texture::unloadGL()
{
  // Make sure the OpenGL data is actually loaded
  if(!m_glloaded)
    return;
  glDeleteTextures( 1, &m_texture );
  m_glloaded = false;
}

GLuint Texture::loadSurface( SDL_Surface * surf )
{
  GLuint newtext;
  // Create a new texture handle
  glGenTextures( 1, &newtext );
  // Bind to the new handle
  glBindTexture( GL_TEXTURE_2D, newtext );
  // Load the actual data
//  gluBuild2DMipmaps( GL_TEXTURE_2D, surf->format->BytesPerPixel, surf->w, 
//          surf->h, getSurfaceFormat(), GL_UNSIGNED_BYTE,
//          surf->pixels );

  glTexImage2D(GL_TEXTURE_2D, 0, surf->format->BytesPerPixel, surf->w, surf->h, 0,
               getSurfaceFormat(), GL_UNSIGNED_BYTE, surf->pixels);

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // Try runtime glGenerateMipmap first
    PFNGLGENERATEMIPMAP generateMipmap = nullptr;
    generateMipmap = (PFNGLGENERATEMIPMAP)SDL_GL_GetProcAddress("glGenerateMipmap");
    if (!generateMipmap)
        generateMipmap = (PFNGLGENERATEMIPMAP)SDL_GL_GetProcAddress("glGenerateMipmapEXT");

    if (generateMipmap)
    {
        // Success! Generate mipmaps at runtime
        generateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        // glGenerateMipmap not available; fallback to GLU if you want
        #ifdef HAVE_GLU
            #include "GL/glu.h"
            gluBuild2DMipmaps( GL_TEXTURE_2D, surf->format->BytesPerPixel, surf->w, 
                    surf->h, getSurfaceFormat(), GL_UNSIGNED_BYTE,
                    surf->pixels );
        #else
            std::cerr << "Warning: glGenerateMipmap not available; mipmaps skipped\n";
        #endif
    }

    m_glloaded = true;
    m_texture = newtext;

  return newtext;
}

GLenum Texture::getSurfaceFormat()
{
  SDL_PixelFormat * f = m_img->format;
  int r, g, b, a;
  r = f->Rmask;
  g = f->Gmask;
  b = f->Bmask;
  a = f->Amask;
  // TODO This might be endian dependent, should look into it
  if(a == 0) {
    // No alpha channel present
    if( b < g && g < r )
      return GL_BGR;
    else
      return GL_RGB;
  } else {
    // Alpha channel present
    if( b < g && g < r )
      return GL_BGRA;
    else
      return GL_RGBA;
  }
}

unsigned int Texture::nearestPow2( unsigned int n )
{
  unsigned int pow2;
  for( pow2 = 2; pow2 < n; pow2 *= 2 )  {}
  return pow2;
}

void Texture::swap(Texture & tex)
{
  std::swap(m_img, tex.m_img);
  std::swap(m_texture, tex.m_texture);
  std::swap(m_loaded, tex.m_loaded);
  std::swap(m_glloaded, tex.m_glloaded);
}

int Texture::getWidth()
{
  return (m_loaded ? m_img->w : 0);
}

int Texture::getHeight()
{
  return (m_loaded ? m_img->h : 0);
}

bool Texture::isLoaded()
{
  return m_loaded;
}
