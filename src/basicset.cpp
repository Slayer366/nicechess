/***************************************************************************
 * NiceChess
 * https://github.com/BencsikRoland/nicechess
 *
 * File : basicpieceset.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey, Bencsik Roland
 **************************************************************************/

#include "config.h"
#include "gamecore.h"
#include "options.h"
#include "pieceset.h"
#include "SDL_opengl.h"

#define d1printf(...)
//#define d1printf(...) printf("%s:%d:", __FUNCTION__, __LINE__); printf(__VA_ARGS__);

BasicSet::BasicSet()
{
  for(int i = 0; i < Board::BOARDSIZE; i++)
    for(int j = 0; j < Board::BOARDSIZE; j++)
      m_hoverheight[i][j] = 0.01;
}

bool BasicSet::load()
{
  Options* opts = Options::getInstance();

  // Make the base of the piece its center
  for(int i = 0; i <= Piece::LAST_TYPE; i++) {
    m_pieces[i].setRecenter(true, false, true);
#ifdef TEXTUREPIECES
    m_pieces[i].enableTexture();
#endif
  }

#ifdef TEXTUREPIECES
  m_textures[Piece::WHITE][Piece::PAWN].load(opts->texturedirectory + "/pawnw.png");
  m_textures[Piece::WHITE][Piece::ROOK].load(opts->texturedirectory + "/rookw.png");
  m_textures[Piece::WHITE][Piece::KNIGHT].load(opts->texturedirectory + "/knightw.png");
  m_textures[Piece::WHITE][Piece::BISHOP].load(opts->texturedirectory + "/bishopw.png");
  m_textures[Piece::WHITE][Piece::QUEEN].load(opts->texturedirectory + "/queenw.png");
  m_textures[Piece::WHITE][Piece::KING].load(opts->texturedirectory + "/kingw.png");

  m_textures[Piece::BLACK][Piece::PAWN].load(opts->texturedirectory + "/pawnb.png");
  m_textures[Piece::BLACK][Piece::ROOK].load(opts->texturedirectory + "/rookb.png");
  m_textures[Piece::BLACK][Piece::KNIGHT].load(opts->texturedirectory + "/knightb.png");
  m_textures[Piece::BLACK][Piece::BISHOP].load(opts->texturedirectory + "/bishopb.png");
  m_textures[Piece::BLACK][Piece::QUEEN].load(opts->texturedirectory + "/queenb.png");
  m_textures[Piece::BLACK][Piece::KING].load(opts->texturedirectory + "/kingb.png");
#endif

  // Load the piece models 
  if(m_pieces[Piece::PAWN].load(opts->modeldirectory + "/pawn.obj")) {
    // Add debugging
  } else {
    return false;
  }

  if(m_pieces[Piece::ROOK].load(opts->modeldirectory + "/rook.obj")) {
    // Add debugging
  } else {
    return false;
  }

  if(m_pieces[Piece::KNIGHT].load(opts->modeldirectory + "/knight.obj")) {
    // Add debugging
  } else {
    return false;
  }

  if(m_pieces[Piece::BISHOP].load(opts->modeldirectory + "/bishop.obj")) {
    // Add debugging
  } else {
    return false;
  }

  if(m_pieces[Piece::QUEEN].load(opts->modeldirectory + "/queen.obj")) {
    // Add debugging
  } else {
    return false;
  }

  if(m_pieces[Piece::KING].load(opts->modeldirectory + "/king.obj")) {
    // Add debugging
  } else {
    return false;
  }

  // Create smooth normals (Phong Shading)
  for(int i = 0; i <= Piece::LAST_TYPE; i++)
    m_pieces[i].findNorms();

  // Scale the pieces to the proper size
  m_pieces[0].setScale(m_pieces[0].scale()*7);
  for(int i = 1; i <= Piece::LAST_TYPE; i++)
    m_pieces[i].setScale(m_pieces[0].scale());
  
  return true;
}

bool BasicSet::unload()
{
  unloadGL();
  return true;
}

bool BasicSet::loadGL()
{
  // Create display lists for the pieces
  for(int i = 0; i <= Piece::LAST_TYPE; i++) {
    m_pieces[i].build();
  }

#ifdef TEXTUREPIECES
  for(int j = 0; j <= Piece::LAST_COLOR; j++) {
    for(int i = 0; i <= Piece::LAST_TYPE; i++) {
      m_textures[j][i].loadGL();
    }
  }
#endif
  return true;
}

bool BasicSet::unloadGL()
{
  // Delete display lists for the pieces
  for(int i = 0; i <= Piece::LAST_TYPE; i++) {
    m_pieces[i].unbuild();
  }

  for(int j = 0; j <= Piece::LAST_COLOR; j++) {
    for(int i = 0; i <= Piece::LAST_TYPE; i++) {
      m_textures[j][i].unloadGL();
    }
  }
  return true;
}


void BasicSet::draw(const ChessGameState& cgs)
{
  Options* opts = Options::getInstance();
  bool hasselectedposition;

  Board b = cgs.getBoard();

  hasselectedposition = (m_selected.isValid() && !m_drawPromotionSelector);

  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  
  // Move to center of bottom left square
  glPushMatrix();
  glTranslated(0.5,0,-0.5);

  for(BoardPosition bp('a', 1); !bp.outN(); bp.moveN())
  {
    for(bp.setFile('a'); !bp.outE(); bp.moveE())
    {
      if (opts->ishighlightpiece)
      {
        if(m_hovertimer[bp.x()][bp.y()].started()) {
          m_hovertimer[bp.x()][bp.y()]++;
          m_hoverheight[bp.x()][bp.y()] += 
            m_hovertimer[bp.x()][bp.y()].change();

          GameCore::getInstance()->requestRepaint();
        }
        if(m_hovertimer[bp.x()][bp.y()].done()) {
          m_hoverheight[bp.x()][bp.y()] = 0.01;
          m_hovertimer[bp.x()][bp.y()].resetDone();
        }
      }

      double offx = 0.0, offy = 0.0;
      if (m_movetimer[bp.x()][bp.y()].first.started()) {
        m_movetimer[bp.x()][bp.y()].first++;
        m_movetimer[bp.x()][bp.y()].second++;

        if (! m_movetimer[bp.x()][bp.y()].first.done()) {
          offx = m_movetimer[bp.x()][bp.y()].first.value();
          offy = m_movetimer[bp.x()][bp.y()].second.value();
          GameCore::getInstance()->requestRepaint();
        }
      }

      if(b.isOccupied(bp))
      {
        Piece* p = b.getPiece(bp);

        if (
          ! hasselectedposition
          || bp != m_selected
        )
        {
          bool alert = false;
          if(cgs.isCheck() && p->type() == Piece::KING && p->color() == cgs.getTurn())
            alert = true;
        
          if (opts->ishighlightpiece)
          {
            glTranslated(0, m_hoverheight[bp.x()][bp.y()], 0);
          }

          if (opts->animations)
          {
            glTranslated(offx, 0, -offy);
          }

          drawPiece(p, 1.0, alert);

          if (opts->animations)
          {
            glTranslated(-offx, 0, offy);
          }

          if (opts->ishighlightpiece)
          {
            glTranslated(0, -m_hoverheight[bp.x()][bp.y()], 0);
          }
        }
      }

      glTranslated(1,0,0);
    }

    // Move to the next rank
    glTranslated(-8,0,-1);
  }

  // Draw the pieces for pawn promotion selection
  if (m_drawPromotionSelector) {
    if (cgs.isWhiteTurn()) {
      // We're waiting for the white player to select which
      // piece to promote to, so draw the pieces at the black
      // end of the board.
      glTranslated(2,0,0);
      static Piece * q = new Piece(Piece::WHITE, Piece::QUEEN);
      drawPiece(q, 1.0, false);
      glTranslated(1,0,0);
      static Piece * k = new Piece(Piece::WHITE, Piece::KNIGHT);
      drawPiece(k, 1.0, false);
      glTranslated(1,0,0);
      static Piece * b = new Piece(Piece::WHITE, Piece::BISHOP);
      drawPiece(b, 1.0, false);
      glTranslated(1,0,0);
      static Piece * r = new Piece(Piece::WHITE, Piece::ROOK);
      drawPiece(r, 1.0, false);
      glTranslated(-5,0,0);
    }
    else {
      // We're waiting for the black player to select which
      // piece to promote to, so draw the pieces at the white
      // end of the board.
      glTranslated(5,0,9);
      static Piece * q = new Piece(Piece::BLACK, Piece::QUEEN);
      drawPiece(q, 1.0, false);
      glTranslated(-1,0,0);
      static Piece * k = new Piece(Piece::BLACK, Piece::KNIGHT);
      drawPiece(k, 1.0, false);
      glTranslated(-1,0,0);
      static Piece * b = new Piece(Piece::BLACK, Piece::BISHOP);
      drawPiece(b, 1.0, false);
      glTranslated(-1,0,0);
      static Piece * r = new Piece(Piece::BLACK, Piece::ROOK);
      drawPiece(r, 1.0, false);
      glTranslated(-2,0,-9);
    }
  }
  
  glPopMatrix();

  if(m_selected.isValid() && !m_drawPromotionSelector) {
    Piece* selected = b.getPiece(m_selected);
    bool alert = false;
    if(cgs.isCheck() && selected->type() == Piece::KING && selected->color() == cgs.getTurn())
      alert = true;

    glTranslated(m_mouseX, 0.2, m_mouseY);  
    drawPiece(b.getPiece(m_selected), 1.0, alert);
    glTranslated(-m_mouseX, -0.2, -m_mouseY);  
  }
}

void BasicSet::drawPiece(Piece* p, double alpha, bool alert)
{
  double rotation = 0;
  double color;
  Options* opts = Options::getInstance();

  if (p->type() == Piece::KNIGHT)
  {
    if (p->color() == Piece::WHITE)
    {
     rotation = -135.0;
    }
    else
    {
      rotation = 45.0;
    }
  }
  else if (p->type() == Piece::BISHOP)
  {
    if (p->color() == Piece::WHITE)
    {
     rotation = 0.0;
    }
    else
    {
      rotation = 180.0;
    }
  }

  if(alpha != 1.0)
    glEnable(GL_BLEND);

  glScalef(1/7.0, 1/7.0, 1/7.0);
  glRotated(rotation, 0.0, 1.0, 0.0);

#ifdef TEXTUREPIECES
  color = 0.8;
  glColor4f(color, color, color, alpha);
#else
  if(p->color() == Piece::BLACK)
  {
    glColor4f(
      opts->blackpiececolorred,
      opts->blackpiececolorgreen,
      opts->blackpiececolorblue,
      alpha
    );
  }
  else
  {
    glColor4f(
      opts->whitepiececolorred,
      opts->whitepiececolorgreen,
      opts->whitepiececolorblue,
      alpha
    );
  }
#endif

  if(alert)
    glColor4f(0.7, 0.15, 0.15, alpha);
  
#ifdef TEXTUREPIECES
  glEnable(GL_TEXTURE_2D);
  m_textures[p->color()][p->type()].use();
#endif
  m_pieces[p->type()].draw();
#ifdef TEXTUREPIECES
  glDisable(GL_TEXTURE_2D);
#endif

  glRotated(-rotation, 0.0, 1.0, 0.0);
  glScalef(7.0, 7.0, 7.0);
  
  if(alpha != 1.0)
    glDisable(GL_BLEND);
}

void BasicSet::hoverPosition(const ChessGameState & gs, const BoardPosition& bp)
{
  if(!m_selected.isValid()) {
    if(m_lasthoverpos.isValid()) {
      m_hovertimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
        .setType(Timer::LOGARITHMIC);
      m_hovertimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
        .setDuration(0.5);
      m_hovertimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
        .setRange(0.2, 0.01);
      m_hovertimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
        .start();
      m_lasthoverpos.invalidate();

      GameCore::getInstance()->requestRepaint();
    }
    if(gs.isPositionSelectable(bp)) {
      m_hoverheight[bp.x()][bp.y()] = 0.2;
      m_hovertimer[bp.x()][bp.y()].stop();
      m_lasthoverpos = bp;

      GameCore::getInstance()->requestRepaint();
    }
  }
}

void BasicSet::animateMove(const BoardMove & bm)
{
  Timer t;
  const Piece * p;
  BoardPosition rookorigin;
  BoardPosition rookdest;
  BoardMove rookmove;

  t.setRange(-bm.signedFileDiff(), 0);
  t.setDuration(0.5);
  m_movetimer[bm.dest().x()][bm.dest().y()].first = t;
  m_movetimer[bm.dest().x()][bm.dest().y()].first.start();

  t.setRange(-bm.signedRankDiff(), 0);
  t.setDuration(0.5);
  m_movetimer[bm.dest().x()][bm.dest().y()].second = t;
  m_movetimer[bm.dest().x()][bm.dest().y()].second.start();

  p = bm.getPiece();

  if (p->type() == Piece::KING)
  {
    if (bm.signedFileDiff() == -2)
    {
      // Castling to the left
      rookorigin = (p->color() == Piece::WHITE ? BoardPosition('a', 1) : BoardPosition('a', 8));
      rookdest   = (p->color() == Piece::WHITE ? BoardPosition('d', 1) : BoardPosition('d', 8));
    } else if (bm.signedFileDiff() == 2)
    {
      // Castling to the right
      rookorigin = (p->color() == Piece::WHITE ? BoardPosition('h', 1) : BoardPosition('h', 8));
      rookdest   = (p->color() == Piece::WHITE ? BoardPosition('f', 1) : BoardPosition('f', 8));
    }

    rookmove = BoardMove(rookorigin, rookdest, NULL, Piece::NOTYPE);

    t.setRange(-rookmove.signedFileDiff(), 0);
    t.setDuration(0.5);
    m_movetimer[rookmove.dest().x()][rookmove.dest().y()].first = t;
    m_movetimer[rookmove.dest().x()][rookmove.dest().y()].first.start();

    t.setRange(-rookmove.signedRankDiff(), 0);
    t.setDuration(0.5);
    m_movetimer[rookmove.dest().x()][rookmove.dest().y()].second = t;
    m_movetimer[rookmove.dest().x()][rookmove.dest().y()].second.start();
  }

  GameCore::getInstance()->requestRepaint();
}

// end of file basicpieceset.cpp
