/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : chessplayers.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
 **************************************************************************/

#ifdef INCHESSPLAYER_H

#include <vector>

using std::vector;

class HumanPlayer : public ChessPlayer {
 public:
  HumanPlayer();

  /**
   * Kills time while the Human user thinks about what move he/she
   * wants to make.
   */
  void think(const ChessGameState & cgs);
  
  /**
   * Returns true because the Human user needs to input a move before
   * play can continue;
   */
  bool needMove() { return true; }
  
  /**
   * Sets the specified move as this players move to make.
   */
  void sendMove(const BoardMove & m);
};

class NicePlayer : public ChessPlayer {
 public:
  NicePlayer();
  void think(const ChessGameState & cgs);
  void opponentMove(const BoardMove & move, const ChessGameState & cgs);

 protected:
  int evaluateBoard(const Board & board, Piece::Color color);
  int search(Board board, Piece::Color color, int depth, int alpha, int beta, BoardMove& move);
  int pawnBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);
  int knightBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);
  int bishopBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);
  int rookBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);
  int queenBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);
  int kingBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);

  int numAttackedSquares(const unsigned long long & pieceAttacks);

  bool isIsolatedPawn(const BoardPosition & bp, const Board & board);
  bool isDoubledPawn(const BoardPosition & bp, const Board & board);
   
  // Used to calculate positional bonuses
  static int m_bishop[64];
  static int m_knight[64];
  static int m_wpawn[64];
  static int m_bpawn[64];
  static int m_wking[64];
  static int m_bking[64];
  static int m_end_king[64];
};

class RandomPlayer : public ChessPlayer {
 public:
  RandomPlayer();
  void think(const ChessGameState & cgs);
};

class TestPlayer : public ChessPlayer {
 public:
  TestPlayer() {}
  void think(const ChessGameState & cgs);
};

#ifndef WIN32

/**
 * XboardPlayer is a ChessPlayer that can be used to interface with
 * chess engines that support the Xboard protocol.
 */
class XboardPlayer : public ChessPlayer {
 public:
  XboardPlayer(
    std::string name,
    std::string executablename
  );

  ~XboardPlayer();

  /**
   * Create a new game of chess using an XboardPlayer. This function 
   * starts the gnuchess process.
   */
  void newGame();

  /**
   * Load a saved game ofchess into the XboardPlayer. This function
   * starts the gnuchess process using the saved game.
   */
  void loadGame(const ChessGameState& cgs);

  /**
   * Notify the XboardPlayer that the game is starting
   */
  void startGame();

  /**
   * Gets a move from the XboardPlayer
   */
  void think(const ChessGameState & cgs);

  /**
   * Sends the opponents move to the XboardPlayer
   */
  void opponentMove(const BoardMove & move, const ChessGameState & cgs);

  void undoMove();

  void runChessEngine();
};

/**
 * UciPlayer is a ChessPlayer that can be used to interface with
 * chess engines that support the Uci protocol.
 */
class UciPlayer : public ChessPlayer {
 public:
  UciPlayer(
    std::string name,
    std::string executablename
  );

  ~UciPlayer();

  /**
   * Create a new game of chess using an UciPlayer. This function 
   * starts the gnuchess process.
   */
  void newGame();

  /**
   * Load a saved game ofchess into the UciPlayer. This function
   * starts the gnuchess process using the saved game.
   */
  void loadGame(const ChessGameState& cgs);

  /**
   * Notify the UciPlayer that the game is starting
   */
  void startGame();

  /**
   * Gets a move from the UciPlayer
   */
  void think(const ChessGameState & cgs);

  /**
   * Sends the opponents move to the UciPlayer
   */
  void opponentMove(const BoardMove & move, const ChessGameState & cgs);

  void undoMove();

  void runChessEngine();
};

#endif // #ifndef WIN32

#endif

// end of file chessplayers.h
