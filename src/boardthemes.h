/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : boardthemes.h
 * Authors : Mike Cook, Joe Flint, Bencsik Roland
 **************************************************************************/
 
#ifdef INBOARDTHEME_H
#include "texture.h"
#include "board.h"
#include "boardposition.h"
#include "timer.h"

class GraniteTheme : public BoardTheme {
 public:
  GraniteTheme();
  void draw(const ChessGameState & cgs);
  void hoverPosition(const BoardPosition& bp);
  void setMoveStartPosition(const BoardPosition& bp);
   bool load();
   bool loadGL();
  void unloadGL();
  void updateBoardTexture();
 protected:
  void highlightSquare(int file, int rank, float alpha, float width);
  void hoverUnhoverPosition(
    const BoardPosition& hoverbp,
    const BoardPosition& unhoverbp
  );

  Texture m_blacktexture, m_whitetexture, m_boardtexture;
  BoardPosition m_lasthoverpos;
  BoardPosition m_movestartpos;
  double m_overlayalpha[Board::BOARDSIZE][Board::BOARDSIZE];
  Timer m_overlaytimer[Board::BOARDSIZE][Board::BOARDSIZE];
};

#endif
