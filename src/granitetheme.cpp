/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : granitetheme.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
 **************************************************************************/

#include <iostream>
#include <string>

#include "boardtheme.h"
#include "config.h"
#include "gamecore.h"
#include "options.h"
#include "SDL_opengl.h"
#include "texture.h"
#include "utils.h"

#define d1printf(...)
//#define d1printf(...) printf("%s:%d:", __FUNCTION__, __LINE__); printf(__VA_ARGS__);

using namespace std;

static const int BOARDSIZE = 8;

GraniteTheme::GraniteTheme()
{
  for(int i = 0; i < Board::BOARDSIZE; i++)
    for(int j = 0; j < Board::BOARDSIZE; j++)
      m_overlayalpha[i][j] = 0.0;
}

void GraniteTheme::highlightSquare(int file, int rank, float alpha, float width)
{
  Options* opts = Options::getInstance();

  glDisable(GL_DEPTH_TEST);
  glColor4d(
    opts->squarehighlightcolorred,
    opts->squarehighlightcolorgreen,
    opts->squarehighlightcolorblue,
    alpha
  );
  glNormal3f(0.0, 7.0, 0.0);
  glBegin(GL_QUADS);
    glVertex3f(file+0.0,     0.0, -rank-0.0);
    glVertex3f(file+width,   0.0, -rank-0.0);        
    glVertex3f(file+width,   0.0, -rank-1.0);
    glVertex3f(file+0.0,     0.0, -rank-1.0);

    glVertex3f(file+width,   0.0, -rank-1+width);
    glVertex3f(file+1-width, 0.0, -rank-1+width);
    glVertex3f(file+1-width, 0.0, -rank-1.0);        
    glVertex3f(file+width,   0.0, -rank-1.0);

    glVertex3f(file+1-width, 0.0, -rank-0.0);
    glVertex3f(file+1.0,     0.0, -rank-0.0);        
    glVertex3f(file+1.0,     0.0, -rank-1.0);
    glVertex3f(file+1-width, 0.0, -rank-1.0);

    glVertex3f(file+width,   0.0, -rank-0.0);
    glVertex3f(file+1-width, 0.0, -rank-0.0);
    glVertex3f(file+1-width, 0.0, -rank-width);        
    glVertex3f(file+width,   0.0, -rank-width);
  glEnd();
  glEnable(GL_DEPTH_TEST);
}

void GraniteTheme::draw(const ChessGameState & cgs)
{
  Options* opts = Options::getInstance();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  // Draw the squares
  bool white_square = false;

  glEnable(GL_TEXTURE_2D);

  m_boardtexture.use();

  // Draw the square
  glColor4d(1.0, 1.0, 1.0, (opts->reflections ? 0.85 : 1.0));
  glNormal3f(0.0, 7.0, 0.0);
  glBegin(GL_QUADS);
    glTexCoord2d( (0)*1/8.0, -(0)*1/8.0 ); glVertex3f(0, 0.0, -(0));
    glTexCoord2d( (8)*1/8.0, -(0)*1/8.0 ); glVertex3f(8, 0.0, -(0));
    glTexCoord2d( (8)*1/8.0, -(8)*1/8.0 ); glVertex3f(8, 0.0, -(8));
    glTexCoord2d( (0)*1/8.0, -(8)*1/8.0 ); glVertex3f(0, 0.0, -(8));
  glEnd();

  glDisable(GL_TEXTURE_2D);

  for (int i = 0; i < 8 && i < BOARDSIZE; ++i) {
    for (int j = 0; j < BOARDSIZE; ++j) {
      if (opts->ishighlightsquare)
      {
        if(m_overlaytimer[i][j].started()) {
          m_overlaytimer[i][j]++;
          m_overlayalpha[i][j] += m_overlaytimer[i][j].change();

          GameCore::getInstance()->requestRepaint();
        }

        if(m_overlaytimer[i][j].done()) {
          m_overlayalpha[i][j] = 0.0;
          m_overlaytimer[i][j].resetDone();
        }

        if(m_overlayalpha[i][j] != 0) {
          highlightSquare(i, j, m_overlayalpha[i][j], 0.06);
        }
      }
    }
  }
  
  drawMoveArrows(cgs);

  // Draw the edges of the board
  glPushMatrix();
  glEnable( GL_LIGHTING );
  // Move back to the center of the board.
  glTranslated( 4, 0, -4 );  
  glBegin( GL_QUADS );
    // Darker of the edge colors.
    glColor4d( 0.2, 0.2, 0.2, 0.95 );
    glNormal3d( 0, 7, 0);
    // White side.
    glVertex3d( -4.1, 0,  4.1 );
    glVertex3d(  4.1, 0,  4.1 );
    glVertex3d(  4.0, 0,  4.0 );
    glVertex3d( -4.0, 0,  4.0 );
    // Right of white side.
    glVertex3d( -4.1, 0, -4.1 );
    glVertex3d( -4.1, 0,  4.1 );
    glVertex3d( -4.0, 0,  4.0 );
    glVertex3d( -4.0, 0, -4.0 );
    // Black side.
    glVertex3d(  4.1, 0, -4.1 );
    glVertex3d( -4.1, 0, -4.1 );
    glVertex3d( -4.0, 0, -4.0 );
    glVertex3d(  4.0, 0, -4.0 );
    // Left of white side.
    glVertex3d(  4.1, 0,  4.1 );
    glVertex3d(  4.1, 0, -4.1 );
    glVertex3d(  4.0, 0, -4.0 );
    glVertex3d(  4.0, 0,  4.0 );
    // Drawing sides of the darker portion.
    glColor3d( 0.2, 0.2, 0.2 );
    glNormal3d( 0, 0, 7);
    glVertex3d( -4.1, -0.075, 4.1 );
    glVertex3d(  4.1, -0.075, 4.1 );
    glVertex3d(  4.1, 0, 4.1 );
    glVertex3d( -4.1, 0, 4.1 );

    glNormal3d( 7, 0, 0 );
    glVertex3d(  4.1, -0.075,  4.1 );
    glVertex3d(  4.1, -0.075, -4.1 );
    glVertex3d(  4.1, 0, -4.1 );
    glVertex3d(  4.1, 0,  4.1 );
    
    glNormal3d( 0, 0, -7 );
    glVertex3d(  4.1, -0.075, -4.1 );
    glVertex3d( -4.1, -0.075, -4.1 );
    glVertex3d( -4.1, 0, -4.1 );
    glVertex3d(  4.1, 0, -4.1 );

    glNormal3d( -7, 0, 0 );
    glVertex3d( -4.1, -0.075, -4.1 );
    glVertex3d( -4.1, -0.075,  4.1 );
    glVertex3d( -4.1, 0,  4.1 );
    glVertex3d( -4.1, 0, -4.1 );

    // Now draw the outer border in light gray.
    glColor4d( 0.45, 0.45, 0.45, 0.95 );
    glNormal3d( 0, 7, 0 );
    // White side.
    glVertex3d( -4.2, -0.075,  4.2 );
    glVertex3d(  4.2, -0.075,  4.2 );
    glVertex3d(  4.1, -0.075,  4.1 );
    glVertex3d( -4.1, -0.075,  4.1 );

    // Right of white side.
    glVertex3d( -4.2, -0.075, -4.2 );
    glVertex3d( -4.2, -0.075,  4.2 );
    glVertex3d( -4.1, -0.075,  4.1 );
    glVertex3d( -4.1, -0.075, -4.1 );

    // Black side.
    glVertex3d(  4.2, -0.075, -4.2 );
    glVertex3d( -4.2, -0.075, -4.2 );
    glVertex3d( -4.1, -0.075, -4.1 );
    glVertex3d(  4.1, -0.075, -4.1 );

    // Left of white side.
    glVertex3d(  4.2, -0.075,  4.2 );
    glVertex3d(  4.2, -0.075, -4.2 );
    glVertex3d(  4.1, -0.075, -4.1 );
    glVertex3d(  4.1, -0.075,  4.1 );

    // Drawing sides of the darker portion.
    glColor3d( 0.45, 0.45, 0.45 );
    glNormal3d( 0, 0, 7);
    glVertex3d( -4.2, -0.15, 4.2 );
    glVertex3d(  4.2, -0.15, 4.2 );
    glVertex3d(  4.2, -0.075, 4.2 );
    glVertex3d( -4.2, -0.075, 4.2 );

    glNormal3d( 7, 0, 0 );
    glVertex3d(  4.2, -0.15,  4.2 );
    glVertex3d(  4.2, -0.15, -4.2 );
    glVertex3d(  4.2, -0.075, -4.2 );
    glVertex3d(  4.2, -0.075,  4.2 );
    
    glNormal3d( 0, 0, -7 );
    glVertex3d(  4.2, -0.15, -4.2 );
    glVertex3d( -4.2, -0.15, -4.2 );
    glVertex3d( -4.2, -0.075, -4.2 );
    glVertex3d(  4.2, -0.075, -4.2 );

    glNormal3d( -7, 0, 0 );
    glVertex3d( -4.2, -0.15, -4.2);
    glVertex3d( -4.2, -0.15,  4.2 );
    glVertex3d( -4.2, -0.075,  4.2 );
    glVertex3d( -4.2, -0.075, -4.2 );
  glEnd();
  glDisable(GL_BLEND);
  glPopMatrix();
}

void GraniteTheme::hoverUnhoverPosition(
  const BoardPosition& hoverbp,
  const BoardPosition& unhoverbp
)
{
  if (unhoverbp.isValid()) {
    m_overlaytimer[unhoverbp.x()][unhoverbp.y()]
      .setType(Timer::LOGARITHMIC);
    m_overlaytimer[unhoverbp.x()][unhoverbp.y()]
      .setDuration(0.1);
    m_overlaytimer[unhoverbp.x()][unhoverbp.y()]
      .setRange(0.7, 0);
    m_overlaytimer[unhoverbp.x()][unhoverbp.y()]
      .start();

    GameCore::getInstance()->requestRepaint();
  }

  if(hoverbp.isValid()) {
    m_overlayalpha[hoverbp.x()][hoverbp.y()] = 1.0;
    m_overlaytimer[hoverbp.x()][hoverbp.y()].stop();

    GameCore::getInstance()->requestRepaint();
  }
}
 
void GraniteTheme::hoverPosition(const BoardPosition& bp)
{
  hoverUnhoverPosition(
    bp,
    (m_lasthoverpos != m_movestartpos ? m_lasthoverpos : BoardPosition())
  );
  
  m_lasthoverpos = bp;
}
 
void GraniteTheme::setMoveStartPosition(const BoardPosition& bp)
{
  hoverUnhoverPosition(bp, m_movestartpos);

  m_movestartpos = bp;
}
 
bool GraniteTheme::load()
{
  Options* opts = Options::getInstance();
  bool iswhiteloaded;
  bool isblackloaded;
  int whitewidth;
  int whiteheight;
  SDL_Surface * whitesurface;
  int blackwidth;
  int blackheight;
  SDL_Surface * blacksurface;
  bool isforcesinglecolor;
  int imagesize;

  iswhiteloaded = false;
  isblackloaded = false;
  isforcesinglecolor = false;
  imagesize = 0;

  if (! opts->isusewhitesquarecolor)
  {
    iswhiteloaded = m_whitetexture.load(opts->whitesquareimagefilename);
  }

  if (! opts->isuseblacksquarecolor)
  {
    isblackloaded = m_blacktexture.load(opts->blacksquareimagefilename);
  }

  if (iswhiteloaded)
  {
    whitewidth = m_whitetexture.getWidth();
    whiteheight = m_whitetexture.getHeight();

    if (whitewidth == whiteheight)
    {
      imagesize = whitewidth;
    }
    else
    {
      m_whitetexture.unload();
      iswhiteloaded = false;
    }
  }

  if (isblackloaded)
  {
    blackwidth = m_blacktexture.getWidth();
    blackheight = m_blacktexture.getHeight();

    if (
      blackwidth == blackheight
      &&
      (
        imagesize == 0
        ||
        (
          imagesize != 0
          && imagesize == blackwidth
        )
      )
    )
    {
      if (imagesize == 0)
      {
        imagesize = blackwidth;
      }
    }
    else
    {
      m_blacktexture.unload();
      isblackloaded = false;
    }
  }

  if (imagesize == 0)
  {
    imagesize = 1024;
  }

  if (iswhiteloaded)
  {
    m_whitetexture.setPixelFormat(SDL_PIXELFORMAT_BGRA32);
  }
  else
  {
    m_whitetexture.createSurface(imagesize, imagesize);
    m_whitetexture.setPixelFormat(SDL_PIXELFORMAT_BGRA32);
    whitesurface = m_whitetexture.getSurface();
    SDL_FillRect(whitesurface, NULL, opts->whitesquarecolor);
  }

  if (isblackloaded)
  {
    m_blacktexture.setPixelFormat(SDL_PIXELFORMAT_BGRA32);
  }
  else
  {
    m_blacktexture.createSurface(imagesize, imagesize);
    m_blacktexture.setPixelFormat(SDL_PIXELFORMAT_BGRA32);
    blacksurface = m_blacktexture.getSurface();
    SDL_FillRect(blacksurface, NULL, opts->blacksquarecolor);
  }

  updateBoardTexture();

  return true;
}

bool GraniteTheme::loadGL()
{
  m_boardtexture.loadGL();
  return true;
}

void GraniteTheme::unloadGL()
{
  m_boardtexture.unloadGL();
}

void GraniteTheme::updateBoardTexture()
{
  int width;
  int height;
  SDL_Surface * whitesurface;
  SDL_Surface * blacksurface;
  SDL_Surface * boardsurface;
  int x;
  int y;
  unsigned char * srcpixels;
  unsigned char * dstpixels;
  unsigned char alpha;
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char refred;
  unsigned char refgreen;
  unsigned char refblue;
  float refhue;
  float refsaturation;
  float refvalue;
  float hue;
  float saturation;
  float value;
  int startx;
  int endx;
  int starty;
  int endy;
  bool iswhite;
  bool ismodify;
  Options* opts = Options::getInstance();
  float whitesquaremodifycolorhue;
  float whitesquaremodifycolorsaturation;
  float whitesquaremodifycolorvalue;
  float blacksquaremodifycolorhue;
  float blacksquaremodifycolorsaturation;
  float blacksquaremodifycolorvalue;

  width = m_whitetexture.getWidth();
  height = m_whitetexture.getHeight();

  if (! m_boardtexture.isLoaded())
  {
    m_boardtexture.createSurface(width, height);
    m_boardtexture.setPixelFormat(SDL_PIXELFORMAT_BGRA32);
  }

  whitesurface = m_whitetexture.getSurface();
  blacksurface = m_blacktexture.getSurface();
  boardsurface = m_boardtexture.getSurface();

  rgb32ToHsv(
    opts->whitesquaremodifycolor,
    &whitesquaremodifycolorhue,
    &whitesquaremodifycolorsaturation,
    &whitesquaremodifycolorvalue
  );

  rgb32ToHsv(
    opts->blacksquaremodifycolor,
    &blacksquaremodifycolorhue,
    &blacksquaremodifycolorsaturation,
    &blacksquaremodifycolorvalue
  );

  whitesquaremodifycolorvalue = whitesquaremodifycolorvalue * 2 - 1;
  blacksquaremodifycolorvalue = blacksquaremodifycolorvalue * 2 - 1;

  iswhite = true;

  for(starty=0; starty<height; starty+=height/8)
  {
    iswhite = ! iswhite;

    endy = starty + height / 8;

    for(startx=0; startx<width; startx+=width/8)
    {
      iswhite = ! iswhite;

      endx = startx + width / 8;

      if (iswhite)
      {
        refhue        = whitesquaremodifycolorhue;
        refsaturation = whitesquaremodifycolorsaturation;
        refvalue      = whitesquaremodifycolorvalue;
        ismodify      = opts->ismodifywhitesquare;
      }
      else
      {
        refhue        = blacksquaremodifycolorhue;
        refsaturation = blacksquaremodifycolorsaturation;
        refvalue      = blacksquaremodifycolorvalue;
        ismodify      = opts->ismodifyblacksquare;
      }

      for(y=starty; y<endy; ++y)
      {
        if (iswhite)
        {
          srcpixels = ((unsigned char *)whitesurface->pixels) + (y * width + startx) * 4;
        }
        else
        {
          srcpixels = ((unsigned char *)blacksurface->pixels) + (y * width + startx) * 4;
        }

        dstpixels = ((unsigned char *)boardsurface->pixels) + (y * width + startx) * 4;

        for(x=startx; x<endx; ++x)
        {
          blue  = *srcpixels; srcpixels++;
          green = *srcpixels; srcpixels++;
          red   = *srcpixels; srcpixels++;
          alpha = *srcpixels; srcpixels++;

//          d1printf("%d,%d: %02x%02x%02x%02x\n", x, y, alpha, red, green, blue);

          if (ismodify)
          {
            rgbToHsv(
              red,
              green,
              blue,
              &hue,
              &saturation,
              &value
            );

            hsvToRgb(
              refhue,
              refsaturation,
              refvalue + value,
              &red,
              &green,
              &blue
            );
          }

          alpha = 0xff;

//          d1printf("%d,%d: %02x%02x%02x%02x\n", x, y, alpha, red, green, blue);

          *dstpixels = blue;  dstpixels++;
          *dstpixels = green; dstpixels++;
          *dstpixels = red;   dstpixels++;
          *dstpixels = alpha; dstpixels++;
        }
      }
    }
  }
}

// End of file granitetheme.cpp
