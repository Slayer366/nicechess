/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : nicechess.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
 **************************************************************************/
 
#ifdef WIN32
  #include <windows.h>
#endif

#include <iostream>
#include <math.h>
#include <string>
#include <vector>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_opengl.h"
#include "SDL_thread.h"

#include "boardtheme.h"
#include "chessgame.h"
#include "chessplayer.h"
#include "fontloader.h"
#include "gamecore.h"
#include "pieceset.h"
#include "options.h"
#include "utils.h"

#define d1printf(...)
//#define d1printf(...) printf("%s:%d:", __FUNCTION__, __LINE__); printf(__VA_ARGS__);

using namespace std;

SDL_Window * window = NULL;
SDL_GLContext glcontext = NULL;

void setIcon()
{
  SDL_Surface * surface;

  // convert 1.png 1.rgb;
  // k=1; for j in $((for i in $(od -v -A none -t x1 -w48 1.rgb); do echo -n $i; done; echo) | sed 's/\(......\)/0x\1, /g'); do echo -n $j; if [ $k -eq 16 ]; then echo ""; k=0; fi; ((k++)); done

  unsigned int iconpixels[16*16] = {
    0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,
    0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,
    0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,
    0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,
    0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,
    0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,
    0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,
    0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,
    0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,
    0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,
    0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,
    0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,
    0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,
    0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,
    0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,
    0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0x4ea2c7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,0xf2f3f7,
  };

  surface =
    SDL_CreateRGBSurfaceFrom(
      iconpixels,
      16,
      16,
      32,
      16*4,
      0x00ff0000,
      0x0000ff00,
      0x000000ff,
      0x00000000
    );

  SDL_SetWindowIcon(window, surface);

  SDL_FreeSurface(surface);
}

// Replacement for gluPerspective
void glPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
    // Convert fovy from degrees to radians
    GLfloat f = 1.0f / tan(fovy * 0.5f * M_PI / 180.0f);

    GLfloat m[16] = {0};

    m[0]  = f / aspect;  // [0][0]
    m[5]  = f;           // [1][1]
    m[10] = (zFar + zNear) / (zNear - zFar);  // [2][2]
    m[11] = -1.0f;                       // [2][3]
    m[14] = (2.0f * zFar * zNear) / (zNear - zFar);  // [3][2]
    m[15] = 0.0f;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(m);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Resizes the game window, handles updating all OpenGL stuff
 * @param width - new width of the window
 * @param height - new height of the window
 */
bool resizeWindow(int width, int height)
{
  Options* opts = Options::getInstance();

  if (height == 0) { height = 1; }  // Protect against devide by zero
  
  GLfloat aspectRatio;
  aspectRatio = (float)width/(float)height;
  
  glViewport(0, 0, width, height);  // Set up our viewport.
  glMatrixMode(GL_PROJECTION);    // Change to the projection matrix and set our viewing volume.
  glLoadIdentity();
  glPerspective(30.0f, aspectRatio, 0.1f, 151.0f);
  glMatrixMode(GL_MODELVIEW);  // Change to the modelview matrix and set out viewing volume.
  glLoadIdentity();

  opts->windowwidth = width;
  opts->windowheight = height;
  
  return true;
}

/**
 * Initializes all the appropriate OpenGL settings
 */
bool initGL()
{
  Options* opts = Options::getInstance();

  glShadeModel(GL_SMOOTH);        // Enable smooth shading. 
  glClearColor(
    opts->backgroundcolorred,
    opts->backgroundcolorgreen,
    opts->backgroundcolorblue,
    1.0
  );
  glClearDepth(1.0f);          // Depth buffer setup. 
  glEnable(GL_DEPTH_TEST);        // Enables Depth Testing 
  glDepthFunc(GL_LEQUAL);          // The Type Of Depth Test To Do
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations 
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable( GL_COLOR_MATERIAL );
  GLfloat light_position[]       = { 3.0,   3.0,  5.0,  0.0 };
  const GLfloat light_ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
  const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
  const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  const GLfloat specularmat[]    = { 1.0f, 1.0f, 1.0f, 1.0f };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  glMaterialfv(GL_FRONT, GL_SPECULAR, specularmat);
  glMaterialf(GL_FRONT, GL_SHININESS, 128);
  glEnable(GL_NORMALIZE);
  glEnable(GL_CULL_FACE);

  return true;
}

/*
 * Handles the drawing of the frame at the highest level. Calculates FPS at 
 * five second intervals.
 * @param core - Current state of the game.
 */
bool drawGLScene(GameCore * core)
{
  // These are to calculate our fps. 
  static int T0     = 0;
  static int Frames = 0;

  // Clear The Screen And The Depth Buffer.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  core->draw();
  
  // Draw it to the screen. 
  SDL_GL_SwapWindow(window);

  // Gather our frames per second. 
  Frames++;
  int t = SDL_GetTicks();
  if (t - T0 >= 5000) {
    float sec = (float)((t-T0)/1000.0);
    float fps = Frames / sec;
//    cout << Frames << " frames in "<< sec << " seconds = "<< fps << " FPS\n";
    T0 = t;
    Frames = 0;
  }

  return true;
}

/*
 * Needed for loading the GameCore due to limitations of SDL_Thread
 * @param gc - GameCore that needs to be loaded
 */
int callLoad(void * gc)
{
  GameCore* core = (GameCore*)gc;
  core->load();
  return 0;
}

/*
 * Come on, its main.
 */
int main(int argc, char *argv[])
{
  bool isneedrepaint;
  bool iswindowfullscreen;
  bool iswindowmaximized;
  bool isrefreshwindowsize;

#ifdef WIN32
  // Seems this was removed in SDL 1.2.10
  freopen("stdout.txt", "w", stdout);
  freopen("stderr.txt", "w", stderr);
#endif

  // Options singleton
  Options* opts = Options::getInstance();

  // Parse command line options before doing any screen initialization
  parseCommandLine(argc, argv);
  
  // Screen Settings
  const int WINDOW_WIDTH = opts->windowwidth;
  const int WINDOW_HEIGHT = opts->windowheight;

  // Used to collect events. 
  SDL_Event event;
    
  // Whether or not the window is active.
  bool isActive = true;

  // Initialize SDL.
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cerr << "Video initialization failed: " << SDL_GetError() << endl;
    Quit(1);
  }

  // Initialize SDL.
  if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) < 0) {
    cerr << "Image initialization failed: " << SDL_GetError() << endl;
    Quit(1);
  }

  // Sets up OpenGL double buffering.
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  
  // Need some a bit for the stencil buffer
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

  window = SDL_CreateWindow(
    "NiceChess",
    opts->windowx,
    opts->windowy,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    SDL_WINDOW_OPENGL
    | SDL_WINDOW_RESIZABLE
    | (opts->fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0)
    | (opts->maximized ? SDL_WINDOW_MAXIMIZED : 0)
  );

  if (! window) {
    cerr << "CreateWindow failed: " << SDL_GetError() << endl;
    Quit(1);
  }

  setIcon();

  glcontext = SDL_GL_CreateContext(window);

  if (! glcontext) {
    cerr << "CreateContext failed: " << SDL_GetError() << endl;
    Quit(1);
  }

  // Initialize all the static board functions
  Board::init();

  ChessPlayer* player1 = toPlayer(opts->player1type);
  ChessPlayer* player2 = toPlayer(opts->player2type);

  player1->setPly(opts->player1ply);
  player2->setPly(opts->player2ply);

  BoardTheme* boardTheme = toBoard(opts->board);
  PieceSet* pieceSet = toPieces(opts->pieces);

  // Initialize OpenGL.
  initGL();

  // Resize the initial window.
  resizeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

  // Create a chess game object here.
  ChessGame game(player1, player2);

  // We delete game core after the player quits the game, the players are
  // deleted in the ChessGame destructor, and the BoardTheme and PieceSets
  // are deleted by the GameCore's destructor.
  GameCore * core = GameCore::getInstance();
  core->init(game, boardTheme, pieceSet);

  core->load();

  core->requestRepaint();

  bool quit = false;
  int width = WINDOW_WIDTH, height = WINDOW_HEIGHT;

  bool goodfullscreen = opts->fullscreen;
  int goodwidth = width, goodheight = height;

  SDL_Event stNextSdlEvent;
  int iRet;

  // Main event loop
  while (!quit) {

    iRet = SDL_PollEvent(&event);

    if (iRet == 1)
    {
      if (event.type == SDL_MOUSEMOTION)
      {
        while (true)
        {
          iRet = SDL_PeepEvents(
            &stNextSdlEvent,
            1,
            SDL_PEEKEVENT,
            SDL_FIRSTEVENT,
            SDL_LASTEVENT
          );

          if (iRet != 1)
          {
            break;
          }

          if (stNextSdlEvent.type != SDL_MOUSEMOTION)
          {
            break;
          }

          iRet = SDL_PeepEvents(
            &event,
            1,
            SDL_GETEVENT,
            SDL_FIRSTEVENT,
            SDL_LASTEVENT
          );

          d1printf("filtered\n");
        }
      }

      isrefreshwindowsize = false;
    
      if(event.type == SDL_QUIT) {
        quit = true;
      }
      else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_f) {
          SDL_SetWindowFullscreen(
            window,
            (iswindowfullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP)
          );

          isrefreshwindowsize = true;

          iswindowfullscreen = ! iswindowfullscreen;
        }
        else if (event.key.keysym.sym == SDLK_m) {
          if (iswindowmaximized)
          {
            SDL_RestoreWindow(window);
          }
          else
          {
            SDL_MaximizeWindow(window);
          }

          isrefreshwindowsize = true;

          iswindowmaximized = ! iswindowmaximized;
        }
      } else if(event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
          isrefreshwindowsize = true;
        }
      
        core->requestRepaint();
      }

      if (isrefreshwindowsize)
      {
        int iSdlWindowWidth;
        int iSdlWindowHeight;
      
        SDL_GetWindowSize(
          window,
          &iSdlWindowWidth,
          &iSdlWindowHeight
        );
        resizeWindow(iSdlWindowWidth, iSdlWindowHeight);
      }

      core->handleEvent(event);
    }

    isneedrepaint = core->isNeedRepaint();

    if (isneedrepaint)
    {
      static int iPrevTimeMs = 0;
      int iTimeMs;

      iTimeMs = SDL_GetTicks();

      if (iTimeMs - iPrevTimeMs >= 15)
      {
        d1printf("%d,paint\n", iTimeMs - iPrevTimeMs);

        iPrevTimeMs = iTimeMs;

        core->setNeedRepaint(false);

        drawGLScene(core);
      }
    }

    iRet = SDL_PeepEvents(
      &event,
      1,
      SDL_PEEKEVENT,
      SDL_FIRSTEVENT,
      SDL_LASTEVENT
    );

    if (iRet != 1)
    {
      SDL_Delay((isneedrepaint ? 5 : 15));
    }
  }

  core->destroy();
  
  // Clean ourselves up and exit.
  Quit(0);

  // Should never get here.
  cerr << "Shouldn't have reached this... End of nicechess.cpp.\n";
  return 0;
}

// End of file nicechess.cpp
