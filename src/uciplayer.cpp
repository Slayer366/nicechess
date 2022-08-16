/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : uciplayer.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/
#ifndef WIN32

#include "board.h"
#include "chessgamestate.h"
#include "chessplayer.h"
#include "options.h"

#include <cassert>
#include <sstream>
#include <string>
#include <unistd.h>

#define d1printf(...)
//#define d1printf(...) printf("%s:%d:", __FUNCTION__, __LINE__); printf(__VA_ARGS__);

using namespace std;

UciPlayer::UciPlayer(
  std::string name,
  std::string executablename
)
{
  m_name = name;
  m_executablename = executablename;
}

UciPlayer::~UciPlayer()
{
  if (m_initialized)
  {
    write(m_to[1], "quit\n", 5);
  }
}

void UciPlayer::newGame()
{
//  runChessEngine();
}

void UciPlayer::loadGame(const ChessGameState& cgs)
{
}

void UciPlayer::startGame()
{
  if (m_is_white)
  {
    if (! m_initialized)
    {
      runChessEngine();
    }
    
    write(m_to[1], "go depth 8\n", 11);
  }
}

// Set up stockfish
void UciPlayer::runChessEngine()
{
  string command;

  d1printf("%d\n", m_ply);

  pipe( m_to );
  pipe( m_from );

  if (fork() == 0) {
    // Child Process
    nice( 20 );
    dup2( m_to[0], 0 );
    dup2( m_from[1], 1 );
    close( m_to[0] );
    close( m_to[1] );
    close( m_from[0] );
    execlp( m_executablename.c_str(), NULL);
    cerr << "Couldn't run " + m_executablename << endl;

    write( m_from[1], "error\n", 6);
    exit(1);
  }

//  if (m_ply < 9)
//  {
//    command = "setoption name Skill Level ";
//    command += to_string(m_ply);
//    command += "\n";
//  }

  write(m_to[1], command.c_str(), command.length());

  m_initialized = true;
}

// Get a move from Uci
void UciPlayer::think(const ChessGameState & cgs)
{
  if (!m_initialized)
    runChessEngine();
  
  if (!m_initialized)
  {
    setStopThinking(true);
    return;
  }

  m_is_thinking = true;

  string output;
//  string outputPart;
  char c;
  while ( output.substr(0, 9) != "bestmove ") {
//    outputPart = output.substr(0, 12);
//    cout << "*" << outputPart << "*" << endl;

    output = "";
    while ( read( m_from[0], &c, 1 ) ) {
      if ( c == '\n' )
        break;
      output += c;
    }
    cout << output << endl;

    if (output == "error")
    {
      m_initialized = false;
      m_is_thinking = false;
      setStopThinking(true);
      return;
    }
  }
  output = output.substr(9, 5);

  cout << output << endl;
  
  // Construct a BoardMove from the move string.
  stringstream oss(output);
  int rank;
    
  oss >> c;
  oss >> rank;
  BoardPosition origin(c, rank);
  
  oss >> c;
  oss >> rank;
  BoardPosition dest(c, rank);

  Board b = cgs.getBoard();

  BoardMove move(origin, dest, b.getPiece(origin));

  if (
    rank == 1
    || rank == 8
  )
  {
    Piece * p = b.getPiece(origin);
    Piece::Type iPieceType;
    char cPromotionType;

    if (p->type() == Piece::PAWN)
    {
      oss >> cPromotionType;

      switch (cPromotionType)
      {
        case 'r': { iPieceType = Piece::ROOK; break; }
        case 'n': { iPieceType = Piece::KNIGHT; break; }
        case 'b': { iPieceType = Piece::BISHOP; break; }
        default:  { iPieceType = Piece::QUEEN; break; }
      }
      
      move.setPromotion(iPieceType);
    }
  }

  m_move = move;
}

// Send your move to Uci
void UciPlayer::opponentMove(const BoardMove & move, const ChessGameState & cgs)
{
  if (!m_initialized)
    runChessEngine();

  string command;
  string fen;

  fen = cgs.getFen();

  command = "";
  command += "position fen ";
  command += fen;
  command += "\n";

  command += "go";
  command += " depth ";
  command += to_string(m_ply);
  command += "\n";

  d1printf("'%s'\n", command.c_str());

  write(m_to[1], command.c_str(), command.length());
}

void UciPlayer::undoMove()
{
  string undo = "undo\n";
  write(m_to[1], undo.c_str(), 5);
}

#endif

// end of file stockfishplayer.cpp
