/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : chessplayer.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
 **************************************************************************/

#ifndef CHESSPLAYER_H
#define CHESSPLAYER_H

#include "SDL.h"

#include "boardmove.h"
#include "chessgamestate.h"

#include <string>

class ChessPlayer {
 public:
   
  ChessPlayer()
    : m_initialized(false), m_is_thinking(false), m_is_stop_thinking(false),
    m_is_human(false), m_trustworthy(true)
    {}

  virtual ~ChessPlayer() {}

  virtual void newGame() {}

  virtual void startGame() {}

  virtual void loadGame(const ChessGameState& cgs) {}

  virtual void opponentMove(const BoardMove & move, const ChessGameState & cgs) {}

  virtual bool isThinking() const
    { return m_is_thinking; }

  virtual bool isHuman() const
    { return m_is_human; }

  virtual void think(const ChessGameState & cgs) = 0;

  virtual int getPly()
    { return m_ply; }

  virtual void setPly(int ply)
    { m_ply = ply; }

  virtual void setStopThinking(bool is_stop_thinking)
    { m_is_stop_thinking = is_stop_thinking; }

  virtual bool isStopThinking() const
    { return m_is_stop_thinking; }

  virtual BoardMove getMove()
    { return m_move; }
  
  virtual void setThinking(bool is_thinking)
    { m_is_thinking = is_thinking; }    

  virtual bool needMove()
    { return false; }

  virtual void sendMove(const BoardMove & bm) {}
    
  virtual void undoMove() {}

  void setIsWhite(bool is_white)
    { m_is_white = is_white; }
  
  bool isWhite() const
    { return m_is_white; }

  bool isTrustworthy() const
    { return m_trustworthy; }

  Piece::Color getColor() const
    { return (m_is_white ? Piece::WHITE : Piece::BLACK); }

 protected:
  virtual void runChessEngine() {};

  bool m_is_white;
  bool m_is_thinking;
  bool m_is_stop_thinking;
  bool m_is_human;  
  bool m_trustworthy;
  BoardMove m_move;
  int m_ply;
  std::string m_name;
  std::string m_executablename;

  int m_to[2];
  int m_from[2];
  bool m_initialized;
};

ChessPlayer * PlayerFactory(const std::string & playertype);

#define INCHESSPLAYER_H
  #include "chessplayers.h"
#undef INCHESSPLAYER_H

#endif
