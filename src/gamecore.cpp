/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : gamecore.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
 **************************************************************************/

#include "boardtheme.h"
#include "chessgame.h"
#include "chessplayer.h"
#include "fontloader.h"
#include "gamecore.h"
#include "menu.h"
#include "menuitem.h"
#include "objfile.h"
#include "options.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_thread.h"
#include "SDL_image.h"
#include "texture.h"
#include "utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <iostream>

#define d1printf(...)
//#define d1printf(...) printf("%s:%d:", __FUNCTION__, __LINE__); printf(__VA_ARGS__);

using std::cout;
using std::cerr;
using std::endl;

static const string PLAYER_NICE = "Nice";
static const string PLAYER_HUMAN = "Human";
static const string PLAYER_RANDOM = "Random";

GameCore * GameCore::m_instance = 0;

GameCore * GameCore::getInstance()
{
  if (m_instance == 0)
    m_instance = new GameCore;
  return m_instance;
}

void GameCore::destroy()
{
  if (m_thinkthread)
  {
    ChessGame* game = &m_game;
    game->getCurrentPlayer()->setStopThinking(true);
    SDL_WaitThread(m_thinkthread, NULL);
    m_thinkthread = NULL;
  }

  delete m_theme;
  m_theme = 0;
  delete m_set;
  m_set;
  delete this;
}

void GameCore::init(const ChessGame & cg, BoardTheme * bt, PieceSet * ps)
{
  m_game  = cg;
  m_theme  = bt;
  m_set  = ps;
}

bool GameCore::load()
{
  m_options = Options::getInstance();

  m_set->load();
  m_theme->load();

  m_game.newGame();
  m_game.startGame();

  if (m_game.getCurrentPlayer()->isHuman())
  {
    m_rotatey = 0.0;
  }
  else if (m_game.getInactivePlayer()->isHuman())
  {
    m_rotatey = 180.0;
  }

  GameCore::getInstance()->requestRepaint();

  m_isWaitingForPromotion = false;
  m_rotate = false;
  m_loaded = true;
  m_suggestedwhiteplayer = m_options->player1type;
  m_suggestedblackplayer = m_options->player2type;
  return true;
}

bool GameCore::loadGL()
{
  if(m_loadthread) {
    SDL_WaitThread(m_loadthread, NULL);
    m_loadthread = 0;
  }

  m_set->loadGL();
  m_theme->loadGL();

  m_defaultcur = SDL_GetCursor();
  char blank[64];
  for(int i = 0; i < 64; i++) {
    blank[i] = 0;
  }
  SDL_ShowCursor(SDL_DISABLE);

  m_blankcur = SDL_CreateCursor((Uint8*)blank, (Uint8*)blank, 8, 8, 0, 0);
  m_glloaded = true;

  if(!m_thinkthread) {
    spawnThinkThread();
  }

  preload();

  return true;
}

void GameCore::unloadGL()
{
  if(!m_glloaded) {
    return;
  }

  m_set->unloadGL();
  m_theme->unloadGL();

  SDL_FreeCursor(m_blankcur);

  m_glloaded = false;
}

// For custom mouse cursor
void GameCore::drawCursorOverlay()
{
  if (m_drawCursor == true) {
    float x = (float)m_mousex;
    float y = (float)m_mousey;
    int w = m_cursorWidth;
    int h = m_cursorHeight;

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT | GL_TEXTURE_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, m_options->windowwidth, m_options->windowheight, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, m_cursorTex);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(x,     y);
        glTexCoord2f(1, 0); glVertex2f(x + w, y);
        glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
        glTexCoord2f(0, 1); glVertex2f(x,     y + h);
    glEnd();

//    glBindTexture(GL_TEXTURE_2D, 0);
//    glDisable(GL_BLEND);
//    glDisable(GL_TEXTURE_2D);
//    glEnable(GL_CULL_FACE);
//    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
  }
}

void GameCore::draw()
{
  if (m_loaded) {

    if (!m_glloaded) {
      loadGL();
    }
    
    toBoardSpace();

    // Fix the lighting so it rotates with the board (same as shadow light)
    GLfloat light_position[4];
    light_position[0] = 10;
    light_position[1] = 12;
    light_position[2] = 6;
    light_position[3] = 1;
//    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    if(m_options->reflections) {
      drawReflections();
    }

    // Blend the draw reflections with the board.
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    m_theme->draw(m_game.getState());
    glDisable( GL_BLEND );
    
    if(m_options->shadows) {
      projectShadows();
    }

    m_set->draw(m_game.getState());

    // Draw a spinning pawn (like the loading screen) if we are waiting on the player
    if (
      ! m_game.getCurrentPlayer()->isHuman()
      && m_game.getCurrentPlayer()->isThinking()
    )
    {
      glLoadIdentity();

      double height = static_cast<double>(m_options->windowheight);
      double width = static_cast<double>(m_options->windowwidth);
      double aspectRatioDiff = (height/width)  - 0.75;
      glTranslated(33.5 - 50.0*aspectRatioDiff, -23.5, -106);

      glRotatef(SDL_GetTicks()/3.5,0,1,0);
      glRotatef(30,0,0,1);
      glTranslatef(0, -3.5, 0);

      // Make sure we can see this above the board
      glDisable(GL_DEPTH_TEST);
      Piece p(m_game.getCurrentPlayer()->getColor(), Piece::PAWN);
      glScalef(7.0, 7.0, 7.0);
      m_set->drawPiece(&p, 1.0, false);
      glScalef(1/7.0, 1/7.0, 1/7.0);
      glEnable(GL_DEPTH_TEST);

      GameCore::getInstance()->requestRepaint();
    }

    if (m_menu.isActive()) {
      m_menu.draw();
    }

    // See if the game has ended, if it has increment a timer that runs
    // to allow the player to see what happened before displaying the
    // end game menu.
    if (m_endgametimer.started()) {
      m_endgametimer++;

      GameCore::getInstance()->requestRepaint();

    } else if (m_endgametimer.done()) {
      // If a piece is picked up, drop it before showing the menu
      if(m_firstclick.isValid()) {
        deselectMoveStartPosition();
      }

      buildMenu();
      m_menu.pushOptionsSet("Game Over");
      string condition;
      if (m_game.getBoard().isCheckMate(Piece::BLACK)) {
        condition = "Checkmate - White Wins";
      }
      else if (m_game.getBoard().isCheckMate(Piece::WHITE)) {
        condition = "Checkmate - Black Wins";
      }
      else {
        condition = "Stalemate";
      }
      m_menu.setHeader(condition);
      m_menu.activate();
      m_endgametimer.resetDone();

      GameCore::getInstance()->requestRepaint();
    }
  } else {
    glLoadIdentity();
    drawLoadingScreen();
    // Extra delay during the loading screen to avoid choking the CPU
    SDL_Delay(25);
  }
    // Draw custom cursor in screen space (2D overlay)
    drawCursorOverlay();
}

void GameCore::deselectMoveStartPosition()
{
  //SDL_SetCursor(m_defaultcur);
  m_theme->setMoveStartPosition(BoardPosition());
  m_set->deselectPosition();
  m_firstclick.invalidate();
  m_drawCursor = true;
}

bool GameCore::handleEvent(SDL_Event& e)
{
  if(!m_loaded)
    return false;

  // Needs to handle mouse, keyboard, and some application events
  // Most window level tasks will be handled in main
  if(m_menu.handleEvent(e)) {
    return true;
  }

  d1printf("%d\n", e.type);
  
  if (e.type == SDL_MOUSEMOTION) {
    if(m_rotate) {
      GLfloat viewport[4];
      glGetFloatv(GL_VIEWPORT, viewport);
      m_rotatey += 360*(e.motion.xrel / viewport[2]);
      m_rotatex += 360*(e.motion.yrel / viewport[3]);
      if(m_rotatex < -42)
        m_rotatex = -42;
      if(m_rotatex > 48)
        m_rotatex = 48;

      GameCore::getInstance()->requestRepaint();

      return true;
    }

    m_mousex = e.motion.x;
    m_mousey = e.motion.y;
    updateMouseBoardPos();
    m_theme->hoverPosition(m_mousepos);
    m_set->mousePosition(m_mouseboardx, m_mouseboardy);
    m_set->hoverPosition(m_game.getState(), m_mousepos);

    if (m_set->hasSelectedPosition())
    {
      GameCore::getInstance()->requestRepaint();
    }

    return true;
  }
  else if (e.type == SDL_MOUSEBUTTONDOWN) {
    updateMouseBoardPos();
    bool needMove = m_game.getCurrentPlayer()->needMove();
    if (e.button.button == SDL_BUTTON_RIGHT) {
      // If a piece has already been picked up, release the piece.
      if(m_firstclick.isValid() && needMove) {
        deselectMoveStartPosition();
      } else {
        m_rotate = true;
        m_mousex = e.button.x;
        m_mousey = e.button.y;
        m_theme->hoverPosition(BoardPosition());
        m_set->hoverPosition(m_game.getState(), BoardPosition());
        SDL_ShowCursor(SDL_DISABLE);
        m_drawCursor = false;
//        SDL_WM_GrabInput(SDL_GRAB_ON);
      }
    }
    else if (!needMove) {
    } else if (e.button.button == SDL_BUTTON_LEFT) {
      if(m_rotate)
        return false;
      
      // Check to see if we're waiting for the player to select a piece to
      // promote his pawn to.
      if (this->isWaitingForPromotion()) {
        Piece::Type t = getPromotionSelection(m_mousepos);
        if (t == Piece::PAWN) {
          return false;
        }
        BoardMove bm(m_firstclick, m_secondclick, m_game.getBoard().getPiece(m_firstclick));
        bm.setPromotion(t);
        if (m_game.getCurrentPlayer()->needMove()) {
          m_game.getCurrentPlayer()->sendMove(bm);
        }
        m_set->drawPromotionSelector(false);
        this->setIsWaitingForPromotion(false);
        return true;
      }

      // Check to see if we've clicked yet
      if (m_firstclick.isValid()) {
        if(m_firstclick == m_mousepos) {
          // Clicked on the start position, unselect
          deselectMoveStartPosition();
        } 
        else {
          // We've already clicked once, so save this new click position and
          // make the move from m_firstclick to the new position
          BoardMove bm(m_firstclick, m_mousepos, m_game.getBoard().getPiece(m_firstclick));
          if (bm.needPromotion()) {
            m_secondclick = m_mousepos;
            if (m_game.getCurrentPlayer()->isHuman()) {
              m_set->drawPromotionSelector(true);
              this->setIsWaitingForPromotion(true);
              //SDL_SetCursor(m_defaultcur);
              m_drawCursor = true;
            }
            else {
              // TODO - Get promotion from AI
            }
          }
          else if (m_game.getCurrentPlayer()->needMove()) {
            if (m_mousepos.isValid())
            {
              if (m_game.getBoard().isMoveLegal(bm))
              {
                // The move doesn't need promotion, so simply send it off
                // to the current player.
                m_game.getCurrentPlayer()->sendMove(bm);
                m_theme->setMoveStartPosition(BoardPosition());
              }
            }
            else
            {
              deselectMoveStartPosition();
            }
          }
        }
      } 
      else {  
        // We haven't clicked yet so set m_firstclick to this click's position
        if (m_mousepos.isValid() && m_game.getBoard().isOccupied(m_mousepos)) {
          // Make sure player is selecting his own piece
          if(m_game.getTurn() == m_game.getBoard().getPiece(m_mousepos)->color()) {
            SDL_SetCursor(m_blankcur);
            m_drawCursor = false;
            m_firstclick = m_mousepos;
            m_theme->setMoveStartPosition(m_mousepos);
            m_set->selectPosition(m_mousepos);
          }
        }
      }
    }

    GameCore::getInstance()->requestRepaint();
  }
  else if (e.type == SDL_MOUSEBUTTONUP) {
    if (e.button.button == SDL_BUTTON_RIGHT && m_rotate) {
      m_rotate = false;
      //SDL_ShowCursor(SDL_ENABLE);
      SDL_ShowCursor(SDL_DISABLE); // Not using system cursor in favor of custom cursor
      m_drawCursor = true;
//      SDL_WM_GrabInput(SDL_GRAB_OFF);
//      SDL_WarpMouse(m_mousex, m_mousey);
    }

    GameCore::getInstance()->requestRepaint();
  }
  else if (e.type == SDL_KEYDOWN) {
    if (e.key.keysym.sym == SDLK_ESCAPE) {
      if (m_set->hasSelectedPosition())
      {
        deselectMoveStartPosition();
      }
      else
      {
        SDL_Event quitevent;
        quitevent.type = SDL_QUIT;
        SDL_PushEvent(&quitevent);
      }
    }
    else if (e.key.keysym.sym == SDLK_F1) {
      buildMenu();
      if (m_menu.isActive())
        m_menu.deactivate();
      else {
        // If a piece is picked up, drop it before showing the menu
        if(m_firstclick.isValid()) {
          deselectMoveStartPosition();
        }
        m_menu.activate();
      }
    }

    GameCore::getInstance()->requestRepaint();
  }
  else if (e.type == SDL_USEREVENT) {
    Options* opts = Options::getInstance();

    if(e.user.code == 0) {
      // Received notification that the player is done thinking
      ChessPlayer *player = (ChessPlayer*)e.user.data1;

      if (! player->isHuman())
      {
        m_set->animateMove(player->getMove());
      }

      if(m_game.tryMove(player->getMove())) {
        m_game.getCurrentPlayer()->opponentMove(player->getMove(), m_game.getState());
        deselectMoveStartPosition();
      }
      SDL_WaitThread(m_thinkthread, NULL);
      m_thinkthread = NULL;

      // Only temporary, really want to do this after animation is done
      if (!(m_game.getBoard().containsCheckMate() || m_game.getState().isDraw())) {
        spawnThinkThread();
      } else {
        m_endgametimer = Timer(Timer::LINEAR);
        m_endgametimer.setDuration(1.0);
        m_endgametimer.start();

        GameCore::getInstance()->requestRepaint();
      }
    }
    else if (e.user.code == Menu::eQUIT) {
      SDL_Event quitevent;
      quitevent.type = SDL_QUIT;
      SDL_PushEvent(&quitevent);
    }
    else if (e.user.code == Menu::eREFLECTTOG) {
      m_options->reflections = !m_options->reflections;
    }
    else if (e.user.code == Menu::eSHADOWTOG) {
      m_options->shadows = !m_options->shadows;
    }
    else if (e.user.code == Menu::eHIGHLIGHTSQUARETOG) {
      m_options->ishighlightsquare = !m_options->ishighlightsquare;
    }
    else if (e.user.code == Menu::eHIGHLIGHTPIECETOG) {
      m_options->ishighlightpiece = !m_options->ishighlightpiece;
    }
    else if (e.user.code == Menu::eHISTORYARROWSTOG) {
      m_theme->toggleHistoryArrows();
    }
    else if (e.user.code == Menu::eMOVEANIMATIONTOG) {
      m_options->animations = !m_options->animations;
    }
    else if (e.user.code == Menu::eMINCOMPUTERMOVETIMECHANGED) {
      opts->mincomputermovetimems = stoi(m_mincomputermovetimemschoices->getCurrentChoice());
    }
    else if (e.user.code == Menu::eBPLYCHANGED) {
      // Set the AI ply depth based on the ai difficulty string
      m_options->player2ply = stoi(m_blackplychoices->getCurrentChoice());
      m_game.getPlayer2()->setPly(m_options->player2ply);
    }
    else if (e.user.code == Menu::eBLACKPLAYERCHANGED) {
      m_suggestedblackplayer = m_blackplayerchoices->getCurrentChoice();
      m_blackplychoices->setCollapsed(
        m_suggestedblackplayer == PLAYER_HUMAN
      );
    }
    else if (e.user.code == Menu::eWPLYCHANGED) {
      // Set the AI ply depth based on the ai difficulty string
      m_options->player1ply = stoi(m_whiteplychoices->getCurrentChoice());
      m_game.getPlayer1()->setPly(m_options->player1ply);
    }
    else if (e.user.code == Menu::eWHITEPLAYERCHANGED) {
      m_suggestedwhiteplayer = m_whiteplayerchoices->getCurrentChoice();
      m_whiteplychoices->setCollapsed(
        m_suggestedwhiteplayer == PLAYER_HUMAN
      );
    }
    else if (e.user.code == Menu::eSTARTNEWGAME) {
      if (m_thinkthread)
      {
        ChessGame* game = &m_game;
        game->getCurrentPlayer()->setStopThinking(true);
        SDL_WaitThread(m_thinkthread, NULL);
        m_thinkthread = NULL;
      }

      ChessPlayer * whiteplayer = PlayerFactory(m_suggestedwhiteplayer);
      whiteplayer->setPly(m_options->player1ply);

      whiteplayer->setIsWhite(true);
      ChessPlayer * blackplayer = PlayerFactory(m_suggestedblackplayer);
      blackplayer->setPly(m_options->player2ply);

      blackplayer->setIsWhite(false);
      m_game.setPlayer1(whiteplayer);
      m_game.setPlayer2(blackplayer);
      m_game.newGame();
      m_game.startGame();

      if (m_game.getCurrentPlayer()->isHuman())
      {
        m_rotatey = 0.0;
      }
      else if (m_game.getInactivePlayer()->isHuman())
      {
        m_rotatey = 180.0;
      }

      GameCore::getInstance()->requestRepaint();

      spawnThinkThread();

      SDL_Event backEvent;
      backEvent.type = SDL_USEREVENT;
      backEvent.user.code = Menu::eBACK;
      SDL_PushEvent(&backEvent);
      SDL_Event backEvent2;
      backEvent2.type = SDL_USEREVENT;
      backEvent2.user.code = Menu::eBACK;
      SDL_PushEvent(&backEvent2);
      SDL_Event backEvent3;
      backEvent3.type = SDL_USEREVENT;
      backEvent3.user.code = Menu::eBACK;
      SDL_PushEvent(&backEvent3);
    }

    GameCore::getInstance()->requestRepaint();
  }
  return false;
}

bool GameCore::preload()
{
  Options* opts = Options::getInstance();

  m_loadpawn.findNorms();
  m_loadpawn.setScale(m_loadpawn.scale()*12);
  
  if(FontLoader::loadFont("sans", opts->fontfilename, 32)) {
    // add debugging
  } else {
    cerr << "Failed to load fonts." << endl;
    return false;
  }

//  if(m_logotexture.load("../art/nicechesslogo.png")) {
//    // add debugging
//  } else if(m_logotexture.load(ART_DIR + string("nicechesslogo.png"))) {
//    // add debugging
//  } else {
//    cerr << "Failed to load logo" << endl;
//    return false;
//  }
//
//  m_logotexture.loadGL();

/**
 * Custom cursor ("art/cursor.png")
 */
  SDL_ShowCursor(SDL_DISABLE);

    cursorsurface = IMG_Load("art/cursor.png");
    if (!cursorsurface) {
      cerr << "Failed to load custom mouse cursor: " << IMG_GetError() << endl;
    } else {
        m_cursorWidth  = cursorsurface->w;
        m_cursorHeight = cursorsurface->h;

        GLenum texture_format;
        GLint nOfColors = cursorsurface->format->BytesPerPixel;
        if (nOfColors == 4) {     // contains alpha channel
            texture_format = (cursorsurface->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
        } else if (nOfColors == 3) { // no alpha channel
            texture_format = (cursorsurface->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
        } else {
            cerr << "Unsupported image format for cursor" << endl;
            texture_format = GL_BGRA; // force fallback for glTexImage2D in case it takes a dump
            SDL_FreeSurface(cursorsurface);
        }

        glGenTextures(1, &m_cursorTex);
        glBindTexture(GL_TEXTURE_2D, m_cursorTex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, texture_format, m_cursorWidth, m_cursorHeight,
                 0, texture_format, GL_UNSIGNED_BYTE, cursorsurface->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        SDL_FreeSurface(cursorsurface);
    }

  m_preloaded = true;
  return true;
}

void GameCore::drawLoadingScreen()
{
  if(!m_preloaded)
    preload();
  glTranslated(0,0,-106);

//  // Drawing the NiceChess Logo
//  m_logotexture.use();
//  glEnable(GL_TEXTURE_2D);
//  glDisable(GL_LIGHTING);
//  glEnable(GL_BLEND);
//  glColor3f(1.0, 1.0, 1.0);
//  glBegin(GL_QUADS);
//    glNormal3d(0,0,-1);
//    glTexCoord2d(0.0,0.0);
//    glVertex3f(-35.0,25.0,0.0);
//    glTexCoord2d(0.0,1.0);
//    glVertex3f(-35.0,-10.0,0.0);
//    glTexCoord2d(1.0,1.0);
//    glVertex3f(35.0,-10.0,0.0);
//    glTexCoord2d(1.0,0.0);
//    glVertex3f(35.0,25.0,0.0);
//  glEnd();
//  glDisable(GL_BLEND);
//  glEnable(GL_LIGHTING);
//  glDisable(GL_TEXTURE_2D);

  // Alpha value to create a pulsing effect
  double c = 0.6+0.4*cos(0.15*SDL_GetTicks()*3.1415/180.0);

  // Drawing the Loading text  
  glEnable(GL_BLEND);
  glColor4f(1.0, 1.0, 1.0, c);
  glTranslated(28, -18, 0);
  glScaled(1/10.0, 1/10.0, 1/10.0);
  FontLoader::print(-28,-7,"Loading...");
  glScaled(10, 10, 10);
  glDisable(GL_BLEND);

  // Spinning pawn
  glRotatef(SDL_GetTicks()/2.5,0,1,0);
  glRotatef(30,0,0,1);

  m_loadpawn.draw();

  GameCore::getInstance()->requestRepaint();
}

void GameCore::projectShadows()
{
  glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  // Generate project matrix for planar shadows
  GLfloat shadowMat[4][4];
  GLfloat groundplane[4];

  // Specified in board space, y is up
  groundplane[0] = 0;
  groundplane[1] = 1;
  groundplane[2] = 0;
  groundplane[3] = 0;
  GLfloat lightpos[4];
  lightpos[0] = 6;
  lightpos[1] = 12;
  lightpos[2] = 10;
  // 1, since this is a point light
  lightpos[3] = 1;

  GLfloat mat[16];

  for(int i = 0; i < 16; i++)
    mat[i] = 0;

  mat[0] = lightpos[0];
  mat[4] = lightpos[1];
  mat[8] = lightpos[2];
  mat[12] = lightpos[3];

  glPushMatrix();
  glLoadIdentity();
  glMultMatrixf(mat);
  glRotated(m_rotatex, 1, 0, 0);
  glRotated(m_rotatey, 0, 1, 0);
  glGetFloatv(GL_MODELVIEW_MATRIX, mat);
  glPopMatrix();

  lightpos[0] = mat[0] + 4;
  lightpos[2] = mat[8] - 4;

  GLfloat dot;

  dot = groundplane[0] * lightpos[0] +
    groundplane[1] * lightpos[1] +
    groundplane[2] * lightpos[2] +
    groundplane[3] * lightpos[3];
  
  shadowMat[0][0] = dot - lightpos[0] * groundplane[0];
  shadowMat[1][0] = 0.f - lightpos[0] * groundplane[1];
  shadowMat[2][0] = 0.f - lightpos[0] * groundplane[2];
  shadowMat[3][0] = 0.f - lightpos[0] * groundplane[3];

  shadowMat[0][1] = 0.f - lightpos[1] * groundplane[0];
  shadowMat[1][1] = dot - lightpos[1] * groundplane[1];
  shadowMat[2][1] = 0.f - lightpos[1] * groundplane[2];
  shadowMat[3][1] = 0.f - lightpos[1] * groundplane[3];

  shadowMat[0][2] = 0.f - lightpos[2] * groundplane[0];
  shadowMat[1][2] = 0.f - lightpos[2] * groundplane[1];
  shadowMat[2][2] = dot - lightpos[2] * groundplane[2];
  shadowMat[3][2] = 0.f - lightpos[2] * groundplane[3];

  shadowMat[0][3] = 0.f - lightpos[3] * groundplane[0];
  shadowMat[1][3] = 0.f - lightpos[3] * groundplane[1];
  shadowMat[2][3] = 0.f - lightpos[3] * groundplane[2];
  shadowMat[3][3] = dot - lightpos[3] * groundplane[3];
  
  glPushMatrix();
    
  // Apply the project matrix
  glMultMatrixf((GLfloat*)shadowMat);

  glDisable(GL_LIGHTING);

  // Only want to write to the stencil buffer
  glDisable(GL_DEPTH_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  // Write a 1 to where the shadows should appear
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
  m_set->draw(m_game.getState());

  // Only want to draw where the stencil buffer is 1
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glStencilFunc(GL_EQUAL, 1, 0xffffffff);
  glPopMatrix();

  // Prevent depth buffer glitch
  glTranslatef(0, 0.01, 0);
  
  // Draw the actual shadow, a black 50% alpha polygon
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glColor4f(0, 0, 0, 0.5);
  glBegin( GL_QUADS );
  glNormal3d( 0, 7, 0 );
  glVertex3d( -0.1, 0,  0.1 );
  glVertex3d( 8.1, 0, 0.1 );
  glVertex3d( 8.1, 0, -8.1 );
  glVertex3d( -0.1, 0,  -8.1 );
  glEnd();
  
  // Restore OpenGL state
  glTranslatef(0, -0.01, 0);
  glPopAttrib();
}

void GameCore::drawReflections()
{
  // This plane is drawn and then erased. The purpose is to obtain the mouse
  // coordinates, and set the stencil buffer for the reflections
  // It is exactly the same size as the board, and is in exactly the same spot
  glDisable( GL_DEPTH_TEST );  
  glEnable( GL_STENCIL_TEST );
  glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
  glStencilFunc( GL_ALWAYS, 1, 0xffffffff );

  glBegin( GL_QUADS );
    glNormal3d( 0, 7, 0 );
    glVertex3d( 0.0, 0.0,  0.0 );
    glVertex3d( 8.0, 0.0,  0.0 );
    glVertex3d( 8.0, 0.0, -8.0 );
    glVertex3d( 0.0, 0.0, -8.0 );
  glEnd();

  glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  // Only draw if the stencil buffer has a 1 here
  glEnable( GL_DEPTH_TEST );
  glStencilFunc( GL_EQUAL, 1, 0xffffffff );
  glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
  
  GLfloat light_position[4];
  glGetLightfv(GL_LIGHT0, GL_POSITION, light_position);

  // Invert over the board plane
  glScalef( 1, -1, 1 );
  glCullFace(GL_FRONT);

  glLightfv(GL_LIGHT1, GL_POSITION, light_position);
  glEnable(GL_LIGHT1);
  glDisable(GL_LIGHT0);
  
  // Draw the reflected pieces
  m_set->draw(m_game.getState());
  
  // Return to normal (not inverted)
  glCullFace(GL_BACK);
  glScalef( 1, -1, 1 );
  glEnable(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glDisable( GL_STENCIL_TEST );
  glClear( GL_STENCIL_BUFFER_BIT );
}

void GameCore::buildMenu()
{
  static bool built = false;
  int mincomputermovetimems;

  if(!built) {
    Options* opts = Options::getInstance();
    int i;

    /* Main
      New game ->
        White player
          Difficulty
        Black player
          Difficulty
        Start New Game
      Options ->
        Reflections       On Off
        Shadows           On Off
        Highlight Piece   On Off
        Highlight Square  On Off
        Move Arrow        On Off
        Move Animation    On Off
        Min Move Time     1000
      Quit
    */
    m_menu.addMenuItem("Chess", new ChangeMenuItem("New Game", "New Game"));
    m_menu.addMenuItem("Chess", new ChangeMenuItem("Options", "Options"));
    m_menu.addMenuItem("Chess", new ActionItem("Quit", Menu::eQUIT));
    m_menu.addMenuItem("Chess", new SeparatorItem());
    m_menu.addMenuItem("Chess", new ActionItem("Back To Game", Menu::eBACK));

    // New Game Menu
    m_whiteplayerchoices = new ChoicesItem("White Player");
    m_whiteplayerchoices->addChoice(PLAYER_HUMAN, Menu::eWHITEPLAYERCHANGED);
    m_whiteplayerchoices->addChoice(PLAYER_NICE, Menu::eWHITEPLAYERCHANGED);

    for(i=0; i<opts->chessengines.size(); ++i)
    {
      m_whiteplayerchoices->addChoice(opts->chessengines[i].name, Menu::eWHITEPLAYERCHANGED);
    }

    m_whiteplayerchoices->setChoice(opts->player1type);

    m_whiteplychoices = new ChoicesItem("  Difficulty");

    for(i=1; i<=9; ++i)
    {
      m_whiteplychoices->addChoice(to_string(i), Menu::eWPLYCHANGED);
    }

    m_whiteplychoices->setChoice(std::to_string(opts->player1ply));

    m_blackplayerchoices = new ChoicesItem("Black Player");
    m_blackplayerchoices->addChoice(PLAYER_HUMAN, Menu::eBLACKPLAYERCHANGED);
    m_blackplayerchoices->addChoice(PLAYER_NICE, Menu::eBLACKPLAYERCHANGED);

    for(i=0; i<opts->chessengines.size(); ++i)
    {
      m_blackplayerchoices->addChoice(opts->chessengines[i].name, Menu::eBLACKPLAYERCHANGED);
    }

    m_blackplayerchoices->setChoice(opts->player2type);

    m_blackplychoices = new ChoicesItem("  Difficulty");

    for(i=1; i<=9; ++i)
    {
      m_blackplychoices->addChoice(to_string(i), Menu::eBPLYCHANGED);
    }

    m_blackplychoices->setChoice(std::to_string(opts->player2ply));

    m_menu.addMenuItem("New Game", m_whiteplayerchoices);
    m_menu.addMenuItem("New Game", m_whiteplychoices);
    m_menu.addMenuItem("New Game", m_blackplayerchoices);
    m_menu.addMenuItem("New Game", m_blackplychoices);
    m_menu.addMenuItem("New Game", new ActionItem("Start New Game", Menu::eSTARTNEWGAME));
    m_menu.addMenuItem("New Game", new SeparatorItem());
    m_menu.addMenuItem("New Game", new ActionItem("Back", Menu::eBACK));
    
    // Options
    m_menu.addMenuItem("Options", new ToggleItem("Reflections", Menu::eREFLECTTOG, m_options->reflections));
    m_menu.addMenuItem("Options", new ToggleItem("Shadows", Menu::eSHADOWTOG, m_options->shadows));
    m_menu.addMenuItem("Options", new ToggleItem("Highlight Piece", Menu::eHIGHLIGHTPIECETOG, m_options->ishighlightpiece));
    m_menu.addMenuItem("Options", new ToggleItem("Highlight Square", Menu::eHIGHLIGHTSQUARETOG, m_options->ishighlightsquare));
    m_menu.addMenuItem("Options", new ToggleItem("Move Arrow", Menu::eHISTORYARROWSTOG, m_options->historyarrows));
    m_menu.addMenuItem("Options", new ToggleItem("Move Animation", Menu::eMOVEANIMATIONTOG, m_options->animations));

    m_mincomputermovetimemschoices = new ChoicesItem("Min Move Time");

    for(i=0; i<=2000; i+=100)
    {
      m_mincomputermovetimemschoices->addChoice(to_string(i), Menu::eMINCOMPUTERMOVETIMECHANGED);
    }

    mincomputermovetimems = opts->mincomputermovetimems;

    if (mincomputermovetimems > 2000)
    {
      mincomputermovetimems = 2000;
    }

    m_mincomputermovetimemschoices->setChoice(to_string(mincomputermovetimems));

    m_menu.addMenuItem("Options", m_mincomputermovetimemschoices);
    m_menu.addMenuItem("Options", new SeparatorItem());
    m_menu.addMenuItem("Options", new ActionItem("Back", Menu::eBACK));

    m_menu.addMenuItem("Game Over", new ChangeMenuItem("New Game", "New Game"));
    m_menu.addMenuItem("Game Over", new ActionItem("Repeat Game", Menu::eSTARTNEWGAME));
    m_menu.addMenuItem("Game Over", new SeparatorItem());
    m_menu.addMenuItem("Game Over", new ActionItem("Main Menu", Menu::eBACK));
    m_menu.addMenuItem("Game Over", new ActionItem("Quit", Menu::eQUIT));

    m_menu.pushOptionsSet("Chess");
    built = true;
  }
}

void GameCore::toBoardSpace()
{
  // Setup board space coordinates
  glLoadIdentity();
  glTranslated(0, 0, -106);
  glRotated(42, 1, 0, 0);

  glRotated(m_rotatex, 1, 0, 0);
  glRotated(m_rotatey, 0, 1, 0);

  glScaled(7, 7, 7);
  glTranslated(-4.0, 0.0, 4.0);
}

void GameCore::updateMouseBoardPos()
{
  // Make sure the screen is cleared
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  toBoardSpace();

  // Draw a large quad to project the mouse position onto
  glBegin(GL_QUADS);
      glVertex3d(-13, 0,  9.5);
      glVertex3d( 20, 0,  9.5);
      glVertex3d( 20, 0, -13);
      glVertex3d(-13, 0, -13);
  glEnd();

  GLdouble mvmatrix[16];
  GLdouble pjmatrix[16];
  GLint viewport[4];

  // Get the current window size, modelview and projection matrices
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, pjmatrix);
  glGetIntegerv(GL_VIEWPORT, viewport);

  // Measure mouse y coordinate from the bottom of the window
  int mousey = viewport[3] - m_mousey;
//  GLfloat mousedepth;

  // Get the depth of the single pixel under the mouse
//  glReadPixels(m_mousex, mousey, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, 
//      &mousedepth);

//  GLdouble x, y, z;
  // Project the mouse position into board coordinates
//  gluUnProject(m_mousex, mousey, mousedepth, mvmatrix, pjmatrix, viewport, 
//      &x, &y, &z);

    glm::mat4 model = glm::make_mat4(mvmatrix);
    glm::mat4 proj  = glm::make_mat4(pjmatrix);
    glm::vec4 view  = glm::make_vec4(viewport);

    // Near and far points in NDC
    glm::vec3 screenNear(m_mousex, mousey, 0.0f);
    glm::vec3 screenFar (m_mousex, mousey, 1.0f);

    glm::vec3 nearPos = glm::unProject(screenNear, model, proj, view);
    glm::vec3 farPos  = glm::unProject(screenFar,  model, proj, view);

    // Ray direction
    glm::vec3 dir = glm::normalize(farPos - nearPos);

    // Intersection with y = 0 plane
    float t = -nearPos.y / dir.y;
    glm::vec3 world = nearPos + t * dir;

//    m_mouseboardx = x;
//    m_mouseboardy = z;
    m_mouseboardx = world.x;
    m_mouseboardy = world.z;

//    m_mousepos = BoardPosition((int)floor(x), -(int)ceil(z));
    m_mousepos = BoardPosition((int)floor(world.x), -(int)ceil(world.z));
}

int callThink(void *pt)
{
  ChessGame * game;
  int starttimems;
  int endtimems;

  Options* opts = Options::getInstance();

  game = (ChessGame*)pt;

  starttimems = SDL_GetTicks();

  // Give the player time to think
  game->getCurrentPlayer()->setStopThinking(false);
  game->getCurrentPlayer()->think(game->getState());

  if (game->getCurrentPlayer()->isStopThinking())
  {
    return 0;
  }

  endtimems = SDL_GetTicks();

  if (endtimems - starttimems < opts->mincomputermovetimems)
  {
    SDL_Delay(opts->mincomputermovetimems - (endtimems - starttimems));
  }

  game->getCurrentPlayer()->setThinking(false);

  // Finished thinking, let the main thread know
  SDL_Event thinkevent;
  thinkevent.type = SDL_USEREVENT;
  thinkevent.user.code = 0;
  thinkevent.user.data1 = game->getCurrentPlayer();
  thinkevent.user.data2 = NULL;
  SDL_PushEvent(&thinkevent);
  return 0;
}

void GameCore::spawnThinkThread()
{
  m_thinkthread = SDL_CreateThread(callThink, "think", &m_game);
  if(m_thinkthread == NULL) {
    cerr << "Unable to create think thread: " << SDL_GetError() << endl;
  }
}

Piece::Type GameCore::getPromotionSelection(const BoardPosition & bp)
{
  if (m_game.getCurrentPlayer()->isWhite()) {
    if (bp == BoardPosition('c', 9))
      return Piece::QUEEN;
    else if (bp == BoardPosition('d', 9))
      return Piece::KNIGHT;
    else if (bp == BoardPosition('e', 9))
      return Piece::BISHOP;
    else if (bp == BoardPosition('f', 9))
      return Piece::ROOK;
  }
  else {
    if (bp == BoardPosition('c', 0))
      return Piece::ROOK;
    else if (bp == BoardPosition('d', 0))
      return Piece::BISHOP;
    else if (bp == BoardPosition('e', 0))
      return Piece::KNIGHT;
    else if (bp == BoardPosition('f', 0))
      return Piece::QUEEN;
  }
  return Piece::PAWN;
}

void GameCore::setNeedRepaint(bool bIsNeedRepaint)
{
  this->bIsNeedRepaint = bIsNeedRepaint;
}

void GameCore::requestRepaint()
{
  this->bIsNeedRepaint = true;
}

bool GameCore::isNeedRepaint()
{
  return this->bIsNeedRepaint;
}

// End of file gamecore.cpp
