/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : chessplayer.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
 **************************************************************************/

#include "chessplayer.h"
#include "options.h"

#include <string>

ChessPlayer * PlayerFactory(const std::string & playertype)
{
  Options* opts = Options::getInstance();
  int i;

  if (playertype == "Nice") {
    return new NicePlayer();
  }
  else if (playertype == "Human") {
    return new HumanPlayer();
  }
  else if (playertype == "Random") {
    return new RandomPlayer();
  }

#ifndef WIN32
  else {
    for(i=0; i<opts->chessengines.size(); ++i)
    {
      if (playertype == opts->chessengines[i].name)
      {
        if (opts->chessengines[i].protocol == XBOARD)
        {
          return
            new XboardPlayer(
              opts->chessengines[i].name,
              opts->chessengines[i].executablename
            );
        }
        else
        {
          return
            new UciPlayer(
              opts->chessengines[i].name,
              opts->chessengines[i].executablename
            );
        }
        break;
      }
    }
  }
#endif

  return NULL;
}

// End of file chessplayer.cpp
