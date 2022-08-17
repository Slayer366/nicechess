bin_PROGRAMS = nicechess

libexec_PROGRAMS = md3view objview

nicechess_SOURCES =	basicset.cpp \
			bitboard.cpp \
			board.cpp \
			boardmove.cpp \
			boardposition.cpp \
			boardtheme.cpp \
			chessgame.cpp \
			chessgamestate.cpp \
			chessplayer.cpp \
			debugset.cpp \
			fontloader.cpp \
			gamecore.cpp \
			granitetheme.cpp \
			humanplayer.cpp \
			menu.cpp \
			menuitem.cpp \
			nicechess.cpp \
			niceplayer.cpp \
			objfile.cpp \
			options.cpp \
			piece.cpp \
			pieceset.cpp \
			randomplayer.cpp \
			texture.cpp \
			timer.cpp \
			uciplayer.cpp \
			utils.cpp \
			vector.cpp \
			xboardplayer.cpp

md3view_SOURCES = 	md3model.cpp \
			md3view.cpp \
			q3charmodel.cpp \
			texture.cpp \
			vector.cpp 

objview_SOURCES = 	objfile.cpp \
			objview.cpp \
			texture.cpp \
			vector.cpp

INCLUDES = -DPREFIX_DIR=\"$(bcdatadir)\" \
	  -DWHITE_SQUARES_IMAGE=\"$(artdir)/whitesquares.png\" \
	  -DBLACK_SQUARES_IMAGE=\"$(artdir)/blacksquares.png\" \
	  -DMODELS_DIR=\"$(modelsdir)\" \
	  -DFONT_FILENAME=\"/usr/share/fonts/liberation-sans/LiberationSans-Regular.ttf\""
