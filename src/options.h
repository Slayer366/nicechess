/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : options.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
 **************************************************************************/

#ifndef OPTIONS_H
#define OPTIONS_H

#include <map>
#include <string>
#include <vector>

enum BoardType {GRANITE, WOOD};
enum PiecesType {BASIC, DEBUG};
enum PlayerType {HUMAN, NICE, RANDOM, TEST, EXTERNAL};
enum Protocol {XBOARD, UCI};

struct ChessEngine {
  std::string name;
  Protocol protocol;
  std::string executablename;
};

class Options {
 public:
  static Options* getInstance();

  int windowwidth;
  int windowheight;
  int windowx;
  int windowy;
  bool fullscreen, maximized;
  bool reflections, shadows;
  bool animations, historyarrows, ishighlightsquare, ishighlightpiece;
  BoardType board;
  PiecesType pieces;
  std::string player1type, player2type;
  int player1chessengineindex, player2chessengineindex;

  int player1ply, player2ply;
  unsigned int backgroundcolor;
  unsigned int whitesquarecolor;
  unsigned int blacksquarecolor;
  bool isusewhitesquarecolor;
  bool isuseblacksquarecolor;
  unsigned int whitepiececolor;
  unsigned int blackpiececolor;
  float backgroundcolorred;
  float backgroundcolorgreen;
  float backgroundcolorblue;
  float whitepiececolorred;
  float whitepiececolorgreen;
  float whitepiececolorblue;
  float blackpiececolorred;
  float blackpiececolorgreen;
  float blackpiececolorblue;
  std::string whitesquareimagefilename;
  std::string blacksquareimagefilename;
  int mincomputermovetimems;
  std::string modeldirectory;
  std::string texturedirectory;
  std::string fontfilename;
  std::vector<ChessEngine> chessengines;
  unsigned int whitesquaremodifycolor;
  bool ismodifywhitesquare;
  unsigned int blacksquaremodifycolor;
  bool ismodifyblacksquare;
  unsigned int squarehighlightcolor;
  float squarehighlightcolorred;
  float squarehighlightcolorgreen;
  float squarehighlightcolorblue;

  std::string getBoardString() 
  { return m_boardTypeString[board]; }

  std::string getPiecesString()
  { return m_piecesTypeString[pieces]; }

  void updateColors();

 private:

  Options();

  static Options* m_instance;

  std::map<BoardType, std::string>    m_boardTypeString;
  std::map<PiecesType, std::string>   m_piecesTypeString;
};

#endif

// end of file options.h
