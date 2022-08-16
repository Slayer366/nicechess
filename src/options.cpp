/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : options.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
 **************************************************************************/

#include "options.h"

Options* Options::m_instance = 0;

Options* Options::getInstance()
{
  if(m_instance == 0) {
    m_instance = new Options();
  }
  return m_instance;
}

Options::Options()
{
  // Defaults for all the options
  windowwidth = 800;
  windowheight = 600;
  windowx = 0;
  windowy = 0;
  animations = false;
  fullscreen = false;
  maximized = false;
  historyarrows = false;
  reflections = false;
  shadows = false;
  ishighlightsquare = false;
  squarehighlightcolor = 0xffbf00;
  ishighlightpiece = false;
  board = GRANITE;
  pieces = BASIC;
  player1type = "Human";
  player2type = "Nice";
  player1chessengineindex = 0;
  player2chessengineindex = 0;
  player1ply = 3;
  player2ply = 3;
  backgroundcolor = 0xd1f5f7;
  whitesquarecolor = 0xf3f4f8;
  blacksquarecolor = 0x4ea2c7;
  isusewhitesquarecolor = false;
  isuseblacksquarecolor = false;
  whitepiececolor = 0xf5d9ad;
  blackpiececolor = 0xc28040;
  whitesquareimagefilename = WHITE_SQUARES_IMAGE;
  blacksquareimagefilename = BLACK_SQUARES_IMAGE;
  modeldirectory = MODELS_DIR;
  texturedirectory = "";
  fontfilename = FONT_FILENAME;
  mincomputermovetimems = 1000;

  // Initialize the enum maps
  m_boardTypeString[GRANITE] = "Granite";
  m_boardTypeString[WOOD] = "Wood";

  m_piecesTypeString[BASIC] = "Basic";
  m_piecesTypeString[DEBUG] = "Debug";
}

void Options::updateColors()
{
  backgroundcolorred        = ((backgroundcolor >> 16) & 0xff) / 255.0f;
  backgroundcolorgreen      = ((backgroundcolor >> 8) & 0xff) / 255.0f;
  backgroundcolorblue       = ((backgroundcolor >> 0) & 0xff) / 255.0f;

  whitepiececolorred        = ((whitepiececolor >> 16) & 0xff) / 255.0f;
  whitepiececolorgreen      = ((whitepiececolor >> 8) & 0xff) / 255.0f;
  whitepiececolorblue       = ((whitepiececolor >> 0) & 0xff) / 255.0f;

  blackpiececolorred        = ((blackpiececolor >> 16) & 0xff) / 255.0f;
  blackpiececolorgreen      = ((blackpiececolor >> 8) & 0xff) / 255.0f;
  blackpiececolorblue       = ((blackpiececolor >> 0) & 0xff) / 255.0f;

  squarehighlightcolorred   = ((squarehighlightcolor >> 16) & 0xff) / 255.0f;
  squarehighlightcolorgreen = ((squarehighlightcolor >> 8) & 0xff) / 255.0f;
  squarehighlightcolorblue  = ((squarehighlightcolor) & 0xff) / 255.0f;
}

// end of file options.cpp
