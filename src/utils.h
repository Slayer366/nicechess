/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : utils.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
 **************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include "boardtheme.h"
#include "chessplayer.h"
#include "pieceset.h"

using std::string;
using std::vector;

void Quit(int returnCode);

BoardTheme* toBoard(BoardType board);
ChessPlayer* toPlayer(std::string playertype);
PieceSet* toPieces(PiecesType pieces);

void parseCommandLine(int argc, char* argv[]);

void printUsage();

int numParams(const vector<string>& args, int i);

void hsvToRgb(
  float fH,
  float fS,
  float fV,
  unsigned char * red,
  unsigned char * green,
  unsigned char * blue
);

void hsvToRgb32(
  float fH,
  float fS,
  float fV,
  unsigned int * color
);

void rgbToHsv(
  unsigned char red,
  unsigned char green,
  unsigned char blue,
  float * pfH,
  float * pfS,
  float * pfV
);

void rgb32ToHsv(
  unsigned int color,
  float * pfH,
  float * pfS,
  float * pfV
);

#endif

// end of file utils.h
