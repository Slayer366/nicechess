# === Project Settings ===
PROGRAM_NAME := nicechess
PROGRAM_VERSION := 1.0

# Directories
SRC_DIR := src
OUT_DIR := out
ART_DIR := art
MODELS_DIR := models
FONT_FILE := fonts/LiberationSans-Regular.ttf

# === Compiler and Tools ===
CXX := g++
RM := rm -f
MKDIR := mkdir -p

# === Flags ===
DEBUG_FLAGS := -g -O0 -fsigned-char
RELEASE_FLAGS := -O2 -fsigned-char

CXXFLAGS := \
  -DNICECHESS_VERSION=\"$(PROGRAM_VERSION)\" \
  -DWHITE_SQUARES_IMAGE=\"$(ART_DIR)/whitesquares.png\" \
  -DBLACK_SQUARES_IMAGE=\"$(ART_DIR)/blacksquares.png\" \
  -DMODELS_DIR=\"$(MODELS_DIR)/\" \
  -DFONT_FILENAME=\"$(FONT_FILE)\" \
  -D_GNU_SOURCE=1 -D_REENTRANT \
  $(RELEASE_FLAGS) \
  -I. \
  -I/usr/include/SDL2 \
  -I/usr/include/freetype2 \
  -I/usr/include/libpng16 \
  -I/usr/include/harfbuzz \
  -I/usr/include/glib-2.0 \
  -I/usr/include/glm \
  -I/usr/lib64/glib-2.0/include \
  -I/usr/include/sysprof-4 \
#  $(shell pkg-config --cflags sdl2 SDL2_image freetype2) \
  -pthread

LDFLAGS := \
  $(shell pkg-config --libs sdl2 SDL2_image freetype2) \
  -lGL -lm -lpng -lz -pthread

# === Sources & Objects ===
SRC_FILES := \
  basicset bitboard board boardmove boardposition boardtheme \
  chessgame chessgamestate chessplayer debugset fontloader \
  gamecore granitetheme humanplayer menu menuitem nicechess \
  niceplayer objfile options piece pieceset randomplayer \
  texture timer uciplayer utils xboardplayer vector

SRCS := $(addprefix $(SRC_DIR)/,$(addsuffix .cpp,$(SRC_FILES)))
OBJS := $(addprefix $(OUT_DIR)/,$(addsuffix .o,$(SRC_FILES)))

# === Targets ===
EXE := $(PROGRAM_NAME)

# === Default Target ===
all: $(EXE)

# === Linking ===
$(EXE): $(OBJS)
	@echo "LD $@"
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

# === Compilation ===
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) $(OUT_DIR)
	@echo "CC $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Utility Targets ===
run: $(EXE)
	./$(EXE)

clean:
	@echo "Cleaning..."
	$(RM) $(OUT_DIR)/*.o $(EXE)

# === Phony Targets ===
.PHONY: all run clean
