/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : utils.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
 **************************************************************************/

#include <iostream>
#include <string>
#include <vector>

#include "SDL.h"
#include "SDL_image.h"

#include "options.h"
#include "utils.h"

#define d1printf(...)
//#define d1printf(...) printf("%s:%d:", __FUNCTION__, __LINE__); printf(__VA_ARGS__);

using namespace std;

extern SDL_Window * window;
extern SDL_GLContext glcontext;
extern char ** environ;

void Quit(int returnCode)
{
  if (glcontext != NULL)
  {
    SDL_GL_DeleteContext(glcontext);
  }

  if (window != NULL)
  {
    SDL_DestroyWindow(window);
  }

  IMG_Quit();

  // Delete our singleton class here
  SDL_Quit();
  exit(returnCode);
}

void printUsage()
{
  Options* opts = Options::getInstance();

  cerr << "NiceChess " NICECHESS_VERSION << endl;
  cerr << "Usage: nicechess [options]" << endl;
  cerr << " -h   Print this help screen." << endl;
  cerr << " -g   Set window geometry WxH+X+Y. (Example: -g 800x600+0+0)" << endl;
  cerr << " -m   Start in maximized mode." << endl;
  cerr << " -f   Start in fullscreen mode." << endl;
  cerr << " -fn  FONT              (\"" FONT_FILENAME "\")" << endl;
  cerr << " -md  MODEL_DIRECTORY          (\"" MODELS_DIR "\")" << endl;
  cerr << " -bc  BACKGROUND_COLOR         (187 15 97)" << endl;
  cerr << " -wsc WHITE_SQUARE_COLOR       (228 2 97)" << endl;
  cerr << " -bsc BLACK_SQUARE_COLOR       (198 60 78)" << endl;
  cerr << " -wpc WHITE_PIECE_COLOR        (36 29 96)" << endl;
  cerr << " -bpc BLACK_PIECE_COLOR        (29 67 76)" << endl;
  cerr << " -wsi WHITE_SQUARES_IMAGE      (\"" WHITE_SQUARES_IMAGE "\")" << endl;
  cerr << " -bsi BLACK_SQUARES_IMAGE      (\"" BLACK_SQUARES_IMAGE "\")" << endl;
  cerr << " -wsm WHITE_MODIFY_COLOR       (Example: -wsm 228 8 100)" << endl;
  cerr << " -bsm BLACK_MODIFY_COLOR       (Example: -bsm 120 50 30)" << endl;
  cerr << " -r   Set reflections on." << endl;
  cerr << " -s   Set shadows on." << endl;
  cerr << " -hp  Set highlight piece on." << endl;
  cerr << " -hs  Set highlight square on." << endl;
  cerr << " -shc SQUARE_HIGHLIGHT_COLOR   (45 100 100)" << endl;
  cerr << " -ma  Set computer move animation on." << endl;
  cerr << " -mo  Set move arrow on." << endl;

#ifndef WIN32
  cerr << " -ce  NAME TYPE EXECUTABLE" << endl;
  cerr << "      Add chess engine. Type can be: xboard, uci." << endl;
  cerr << "      Examples: -ce Faile xboard /usr/bin/faile" << endl;
  cerr << "                -ce Gnuchess xboard /usr/bin/gnuchess" << endl;
  cerr << "                -ce Stockfish uci /usr/bin/stockfish" << endl;
#endif

  cerr << " -wpt WHITE_PLAYER_TYPE        (Human)" << endl;
  cerr << " -bpt BLACK_PLAYER_TYPE        (Nice)" << endl;
  cerr << "      Choices are: Human, Nice and added chess engine names." << endl;
  cerr << " -wpd WHITE_PLAYER_DIFFICULTY  (3) (Choices are: 1 - 9)" << endl;
  cerr << " -bpd BLACK_PLAYER_DIFFICULTY  (3) (Choices are: 1 - 9)" << endl;
  cerr << " -mt  TIME_MS  Set minimum computer move time.  (1000)" << endl;
  cerr << endl;
  cerr << "Colors are in HUE SAT. VALUE triplet or in 0xRRGGBB format." << endl;
  cerr << "Hue range:        0 - 360" << endl;
  cerr << "Saturation range: 0 - 100" << endl;
  cerr << "Value range:      0 - 100" << endl;
  cerr << "Texture images have to have power of two width and height." << endl;
  cerr << endl;
  cerr << "Keys:" << endl;
  cerr << "  F1  - Menu" << endl;
  cerr << "  f   - Toggle fullscreen" << endl;
  cerr << "  m   - Toggle maximized" << endl;
  cerr << "  Esc - Quit" << endl;
  cerr << endl;
  
  Quit(1);
}

void readColor(
  vector<string> & args,
  int * i,
  unsigned int * color,
  bool * isok
)
{
  float hue;
  float saturation;
  float value;

  if (numParams(args, *i) == 1)
  {
    *color = stoi(args[*i+1], NULL, 0);

    *i += 1;
  }
  else if (numParams(args, *i) == 3)
  {
    hue        = stoi(args[*i+1], NULL);
    saturation = stoi(args[*i+2], NULL);
    value      = stoi(args[*i+3], NULL);

    hsvToRgb32(
      hue,
      saturation / 100.0,
      value / 100.0,
      color
    );

    *i += 3;
  }
  else
  {
    *isok = false;
  }
}

void parseCommandLine(int argc, char* argv[])
{
  Options* opts = Options::getInstance();
  char * pcGeometry;
  unsigned int uiState;
  char cChar;
  bool isok;
  int j;
  float hue;
  float saturation;
  float value;

  vector<string> args;
  for(int i=1; i < argc; i++)
    args.push_back(string(argv[i]));

  isok = true;

  for(int i=0; i < args.size(); i++) {
    if(args[i] == "-h") {
      isok = false;
      break;
    } else if(args[i] == "-g" && numParams(args,i) == 1) {
      pcGeometry = argv[1+i+1];
      uiState = 0;

      opts->windowwidth = 0;

      while (*pcGeometry != 0)
      {
        cChar = *pcGeometry;

        switch (uiState)
        {
          case 0:
          {
            if (
              cChar >= '0'
              && cChar <= '9'
            )
            {
              opts->windowwidth *= 10;
              opts->windowwidth += cChar - '0';
            }
            else if (cChar == 'x')
            {
              opts->windowheight = 0;
              uiState++;
            }
            else
            {
              isok = false;
            }
            break;
          }
          case 1:
          {
            if (
              cChar >= '0'
              && cChar <= '9'
            )
            {
              opts->windowheight *= 10;
              opts->windowheight += cChar - '0';
            }
            else if (cChar == '+')
            {
              opts->windowx = 0;
              uiState++;
            }
            else
            {
              isok = false;
            }
            break;
          }
          case 2:
          {
            if (
              cChar >= '0'
              && cChar <= '9'
            )
            {
              opts->windowx *= 10;
              opts->windowx += cChar - '0';
            }
            else if (cChar == '+')
            {
              opts->windowy = 0;
              uiState++;
            }
            else
            {
              isok = false;
            }
            break;
          }
          case 3:
          {
            if (
              cChar >= '0'
              && cChar <= '9'
            )
            {
              opts->windowy *= 10;
              opts->windowy += cChar - '0';
            }
            else if (cChar == '+')
            {
              uiState++;
            }
            else
            {
              isok = false;
            }
            break;
          }
        }

        if (! isok)
        {
          break;
        }

        pcGeometry++;
      }
      i++;
    } else if(args[i] == "-m") {
      opts->maximized = true;
    } else if(args[i] == "-f") {
      opts->fullscreen = true;
    } else if(args[i] == "-fn" && numParams(args,i) == 1) {
      opts->fontfilename = args[i+1];
      i++;
    } else if(args[i] == "-md" && numParams(args,i) == 1) {
      opts->modeldirectory = args[i+1];
      i++;
    } else if(args[i] == "-bc") {
      readColor(args, &i, &(opts->backgroundcolor), &isok);
    } else if(args[i] == "-wsc") {
      readColor(args, &i, &(opts->whitesquarecolor), &isok);
      opts->isusewhitesquarecolor = true;
    } else if(args[i] == "-bsc") {
      readColor(args, &i, &(opts->blacksquarecolor), &isok);
      opts->isuseblacksquarecolor = true;
    } else if(args[i] == "-wsi" && numParams(args,i) == 1) {
      opts->whitesquareimagefilename = args[i+1];
      i++;
    } else if(args[i] == "-bsi" && numParams(args,i) == 1) {
      opts->blacksquareimagefilename = args[i+1];
      i++;
    } else if(args[i] == "-wsm") {
      readColor(args, &i, &(opts->whitesquaremodifycolor), &isok);
      opts->ismodifywhitesquare = true;
    } else if(args[i] == "-bsm") {
      readColor(args, &i, &(opts->blacksquaremodifycolor), &isok);
      opts->ismodifyblacksquare = true;
    } else if(args[i] == "-wpc") {
      readColor(args, &i, &(opts->whitepiececolor), &isok);
    } else if(args[i] == "-bpc") {
      readColor(args, &i, &(opts->blackpiececolor), &isok);
    } else if(args[i] == "-r") {
      opts->reflections = true;
    } else if(args[i] == "-s") {
      opts->shadows = true;
    } else if(args[i] == "-hp") {
      opts->ishighlightpiece = true;
    } else if(args[i] == "-hs") {
      opts->ishighlightsquare = true;
    } else if(args[i] == "-shc") {
      readColor(args, &i, &(opts->squarehighlightcolor), &isok);
    } else if(args[i] == "-ma") {
      opts->animations = true;
    } else if(args[i] == "-mo") {
      opts->historyarrows = true;

#ifndef WIN32
    } else if(args[i] == "-ce" && numParams(args,i) == 3) {
      ChessEngine chessengine;

      chessengine.name = args[i+1];
      chessengine.executablename = args[i+3];

      if(args[i+2] == "xboard") {
        chessengine.protocol = XBOARD;
      } else if(args[i+2] == "uci") {
        chessengine.protocol = UCI;
      } else {
        isok = false;
      }

      opts->chessengines.push_back(chessengine);

      i+=3;
#endif

    } else if(args[i] == "-wpt" && numParams(args,i) == 1) {
      if (
        args[i+1] == "Nice"
        || args[i+1] == "Human"
      ) {
        opts->player1type = args[i+1];
      } else {
        opts->player1type = "";

        for(j=0; j<opts->chessengines.size(); ++j)
        {
          if (args[i+1] == opts->chessengines[j].name)
          {
            opts->player1type = args[i+1];
            break;
          }
        }

        if (opts->player1type == "")
        {
          isok = false;
        }
      }
      i++;
    } else if(args[i] == "-bpt" && numParams(args,i) == 1) {
      if (
        args[i+1] == "Nice"
        || args[i+1] == "Human"
      ) {
        opts->player2type = args[i+1];
      } else {
        opts->player2type = "";

        for(j=0; j<opts->chessengines.size(); ++j)
        {
          if (args[i+1] == opts->chessengines[j].name)
          {
            opts->player2type = args[i+1];
            break;
          }
        }

        if (opts->player2type == "")
        {
          isok = false;
        }
      }
      i++;
    } else if(args[i] == "-wpd" && numParams(args,i) == 1) {
      opts->player1ply = stoi(args[i+1]);
      i++;
    } else if(args[i] == "-bpd" && numParams(args,i) == 1) {
      opts->player2ply = stoi(args[i+1]);
      i++;
    } else if(args[i] == "-mt" && numParams(args,i) == 1) {
      opts->mincomputermovetimems = stoi(args[i+1]);

      if (opts->mincomputermovetimems > 2000)
      {
        opts->mincomputermovetimems = 2000;
      }

      opts->mincomputermovetimems += 99;
      opts->mincomputermovetimems /= 100;
      opts->mincomputermovetimems *= 100;

      i++;
    } else {
      isok = false;
    }

    if (! isok)
    {
      break;
    }
  }

  if (opts->fontfilename == "")
  {
    cerr << "Error: Font is not set." << endl;
    isok = false;
  }

  if (opts->modeldirectory == "")
  {
    cerr << "Error: Model directory is not set." << endl;
    isok = false;
  }

  if (! isok)
  {
    printUsage();
  }

  opts->updateColors();
}

BoardTheme* toBoard(BoardType board)
{
  if(board == GRANITE) {
    return new GraniteTheme();
  }

  printUsage();

  return NULL;
}

ChessPlayer* toPlayer(std::string playertype)
{
  return PlayerFactory(playertype);
}

PieceSet* toPieces(PiecesType pieces)
{
  if(pieces == BASIC) {
    return new BasicSet();
  } else if(pieces == DEBUG) {
    return new DebugSet();
  } printUsage();

  return NULL;
}

int numParams(const vector<string>& args, int i)
{
  int num = 0;
  while(i+1 < args.size() && args[i+1][0] != '-') {
    i++;
    num++; 
  }
  return num;
}

void hsvToRgb(
  float fH,
  float fS,
  float fV,
  unsigned char * red,
  unsigned char * green,
  unsigned char * blue
)
{
  float fC;
  float fX;
  float fM;
  float fR0;
  float fG0;
  float fB0;

  fH = fmodf(fH, 360.0f);

  if (fV < 0.0f)
  {
    fV = 0.0f;
  }
  else if (fV > 1.0f)
  {
    fV = 1.0f;
  }

  if (fS < 0.0f)
  {
    fS = 0.0f;
  }
  else if (fS > 1.0f)
  {
    fS = 1.0f;
  }

  fH = floor(fH);

  fC = fV * fS;
  fX =
    fC
    *
    (
      1
      - fabs(
        (
          (
            (fH / 60)
            - floor(fH / 60 / 2) * 2
          ) - 1
        )
      )
    )
  ;
  fM = fV - fC;

  if (fH < 60)       { fR0 = fC; fG0 = fX; fB0 = 0;  }
  else if (fH < 120) { fR0 = fX; fG0 = fC; fB0 = 0;  }
  else if (fH < 180) { fR0 = 0;  fG0 = fC; fB0 = fX; }
  else if (fH < 240) { fR0 = 0;  fG0 = fX; fB0 = fC; }
  else if (fH < 300) { fR0 = fX; fG0 = 0;  fB0 = fC; }
  else               { fR0 = fC; fG0 = 0;  fB0 = fX; }

  *blue  = ((unsigned char)((fB0 + fM) * 255));
  *green = ((unsigned char)((fG0 + fM) * 255));
  *red   = ((unsigned char)((fR0 + fM) * 255));
}

void hsvToRgb32(
  float fH,
  float fS,
  float fV,
  unsigned int * color
)
{
  unsigned char red;
  unsigned char green;
  unsigned char blue;

  hsvToRgb(
    fH,
    fS,
    fV,
    &red,
    &green,
    &blue
  );
  
  *color =
    (255 << 24)
    | (red << 16)
    | (green << 8)
    | (blue)
  ;
}

void rgbToHsv(
  unsigned char red,
  unsigned char green,
  unsigned char blue,
  float * pfH,
  float * pfS,
  float * pfV
)
{
  float fR0;
  float fG0;
  float fB0;
  float fCMax;
  float fCMin;
  float fCDiff;
  float fH;
  float fS;
  float fV;

  fR0 = ((float)(red)) / 255;
  fG0 = ((float)(green)) / 255;
  fB0 = ((float)(blue)) / 255;

  fCMax = fR0;
  if (fCMax < fG0) { fCMax = fG0; }
  if (fCMax < fB0) { fCMax = fB0; }
  fCMin = fR0;
  if (fCMin > fG0) { fCMin = fG0; }
  if (fCMin > fB0) { fCMin = fB0; }

  fCDiff = fCMax - fCMin;

  if (fCDiff == 0.0)
  {
    fH = 0.0;
    fS = 0.0;
  }
  else
  {
    if (fCMax == fR0)
    {
      fH = (fG0 - fB0) / fCDiff;
      fH -= floor(fH / 6) * 6;
      fH = 60 * fH;
    }
    else if (fCMax == fG0)
    {
      fH = (fB0 - fR0) / fCDiff;
      fH += 2;
      fH = 60 * fH;
    }
    else
    {
      fH = (fR0 - fG0) / fCDiff;
      fH += 4;
      fH = 60 * fH;
    }

    fS = fCDiff / fCMax;
  }

  fV = fCMax;

  if (pfH != NULL)
  {
    *pfH = fH;
  }
  if (pfS != NULL)
  {
    *pfS = fS;
  }
  if (pfV != NULL)
  {
    *pfV = fV;
  }
}

void rgb32ToHsv(
  unsigned int color,
  float * pfH,
  float * pfS,
  float * pfV
)
{
  rgbToHsv(
    (color >> 16) & 0xff,
    (color >> 8) & 0xff,
    (color) & 0xff,
    pfH,
    pfS,
    pfV
  );
}

// end of file utils.cpp
