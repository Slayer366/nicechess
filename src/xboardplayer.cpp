/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : gnuchessplayer.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
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

XboardPlayer::XboardPlayer(
  std::string name,
  std::string executablename
)
{
  m_name = name;
  m_executablename = executablename;
}

XboardPlayer::~XboardPlayer()
{
  if (m_initialized)
  {
    write(m_to[1], "quit\n", 5);
  }
}

void XboardPlayer::newGame()
{
//  runChessEngine();
}

void XboardPlayer::loadGame(const ChessGameState& cgs)
{
}

void XboardPlayer::startGame()
{
  if (m_is_white)
  {
    if (! m_initialized)
    {
      runChessEngine();
    }
    
    write(m_to[1], "go\n", 3);
  }
}

// Set up
void XboardPlayer::runChessEngine()
{
  string commandtext;

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

  write( m_to[1], "xboard\n", 7);
  write( m_to[1], "nopost\n", 7);
  write( m_to[1], "easy\n", 5);

  if (m_ply < 9)
  {
    commandtext = "depth " + to_string(m_ply) + "\n";
    write( m_to[1], commandtext.c_str(), commandtext.length());
  }

  m_initialized = true;
}

// Get a move from Xboard
void XboardPlayer::think(const ChessGameState & cgs)
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
  char c;
  while (true) {
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

    if (output.substr(0, 12) == "My move is :")
    {
      output = output.substr(12, 5);
      break;
    }
    else if (output.substr(0, 5) == "move ")
    {
      output = output.substr(5, 5);
      break;
    }
  }

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

// Send your move to Xboard
void XboardPlayer::opponentMove(const BoardMove & move, const ChessGameState & cgs)
{
  if (!m_initialized)
    runChessEngine();

  Board board = cgs.getBoard();
  
  Piece * piece = board.getPiece(move.dest());

  int characters = 5;
  string movestr = "";
  movestr += move.origin().filec();
  movestr += '0' + move.origin().rank();
  movestr += move.dest().filec();
  movestr += '0' + move.dest().rank();
  
  switch (move.getPromotion())
  {
    case Piece::ROOK:   { movestr += 'r'; characters++; break; }
    case Piece::KNIGHT: { movestr += 'n'; characters++; break; }
    case Piece::BISHOP: { movestr += 'b'; characters++; break; }
    case Piece::QUEEN:  { movestr += 'q'; characters++; break; }
    default: { break; }
  }
  movestr += '\n';

  cout << movestr << endl;

  write( m_to[1], movestr.c_str(), characters);
}

void XboardPlayer::undoMove()
{
  string undo = "undo\n";
  write(m_to[1], undo.c_str(), 5);
}

#endif

// end of file xboardplayer.cpp
