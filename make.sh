#!/bin/sh

scriptDir=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)

programName="nicechess"
programVersion="1.0"
artDir="art"
modelsDir="models"
whiteSqauresImage="whitesquares.png"
blackSqauresImage="blacksquares.png"
fontFileName="/usr/share/fonts/liberation-sans/LiberationSans-Regular.ttf"

installMainDir="/usr"
#installMainDir="/opt/chess"

installBinDir="$installMainDir/bin"
installShareDir="$installMainDir/share/$programName"
installArtDir="$installShareDir/$artDir"
installModelsDir="$installShareDir/$modelsDir"

outDir="$scriptDir/out"
exe="$outDir/$programName"

debugParameters="-g -O0"

parameters=""
parameters=$parameters" -DNICECHESS_VERSION=\"$programVersion\""
parameters=$parameters" -DWHITE_SQUARES_IMAGE=\"$installArtDir/$whiteSqauresImage\""
parameters=$parameters" -DBLACK_SQUARES_IMAGE=\"$installArtDir/$blackSqauresImage\""
parameters=$parameters" -DMODELS_DIR=\"$installModelsDir/\""
parameters=$parameters" -DFONT_FILENAME=\"$fontFileName\""
parameters=$parameters" -D_GNU_SOURCE=1"
parameters=$parameters" -D_REENTRANT"
parameters=$parameters" $debugParameters"
parameters=$parameters" -I."
parameters=$parameters" -I/usr/include/SDL2"
parameters=$parameters" -I/usr/include/freetype2"
parameters=$parameters" -I/usr/include/libpng16"
parameters=$parameters" -I/usr/include/harfbuzz"
parameters=$parameters" -I/usr/include/glib-2.0"
parameters=$parameters" -I/usr/lib64/glib-2.0/include"
parameters=$parameters" -I/usr/include/sysprof-4"
parameters=$parameters" -pthread"

srcs=""
srcs=$srcs" ""basicset"
srcs=$srcs" ""bitboard"
srcs=$srcs" ""board"
srcs=$srcs" ""boardmove"
srcs=$srcs" ""boardposition"
srcs=$srcs" ""boardtheme"
srcs=$srcs" ""chessgame"
srcs=$srcs" ""chessgamestate"
srcs=$srcs" ""chessplayer"
srcs=$srcs" ""debugset"
srcs=$srcs" ""fontloader"
srcs=$srcs" ""gamecore"
srcs=$srcs" ""granitetheme"
srcs=$srcs" ""humanplayer"
#srcs=$srcs" ""md3model"
srcs=$srcs" ""menu"
srcs=$srcs" ""menuitem"
srcs=$srcs" ""nicechess"
srcs=$srcs" ""niceplayer"
srcs=$srcs" ""objfile"
srcs=$srcs" ""options"
srcs=$srcs" ""piece"
srcs=$srcs" ""pieceset"
#srcs=$srcs" ""q3charmodel"
#srcs=$srcs" ""q3set"
srcs=$srcs" ""randomplayer"
srcs=$srcs" ""texture"
srcs=$srcs" ""timer"
srcs=$srcs" ""uciplayer"
srcs=$srcs" ""utils"
srcs=$srcs" ""xboardplayer"
srcs=$srcs" ""vector"

isDependencyChanged()
{
  local class
  local deps
  local src
  local header
  local obj
  local i

  class=$1
  classObj="../out/$class.o"

#  echo "isdep: "$class

  deps=""

  case $class in
    "basicset")        deps="gamecore options pieceset" ;;
    "bitboard")        deps="board boardposition" ;;
    "board")           deps="bitboard boardmove" ;;
    "boardmove")       deps="boardposition piece" ;;
    "boardposition")   deps="board" ;;
    "boardtheme")      deps="boardmove boardposition chessgamestate options" ;;
    "chessgame")       deps="board boardmove chessplayer chessgamestate piece menu" ;;
    "chessgamestate")  deps="board" ;;
    "chessplayer")     deps="boardmove chessgamestate chessplayers options" ;;
    "debugset")        deps="pieceset" ;;
    "fontloader")      deps="" ;;
    "gamecore")        deps="boardtheme chessgame chessplayer fontloader menu menuitem objfile options pieceset texture" ;;
    "granitetheme")    deps="boardtheme gamecore options texture utils" ;;
    "humanplayer")     deps="boardmove chessplayer" ;;
#    "md3model")        deps="" ;;
    "menu")            deps="fontloader gamecore menuitem options timer" ;;
    "menuitem")        deps="fontloader gamecore menu timer" ;;
    "nicechess")       deps="boardtheme chessgame chessplayer fontloader gamecore pieceset options utils" ;;
    "niceplayer")      deps="board chessplayer options" ;;
    "objfile")         deps="" ;;
    "options")         deps="" ;;
    "piece")           deps="statsnapshot" ;;
    "pieceset")        deps="board boardposition objfile chessgamestate   piecesets piece texture timer" ;;
#    "q3charmodel")     deps="" ;;
#    "q3set")           deps="" ;;
    "randomplayer")    deps="board chessplayer" ;;
    "statsnapshot")    deps="boardposition" ;;
    "texture")         deps="" ;;
    "timer")           deps="" ;;
    "uciplayer")       deps="board chessgamestate chessplayer options" ;;
    "utils")           deps="boardtheme chessplayer pieceset options" ;;
    "xboardplayer")    deps="board chessgamestate chessplayer options" ;;
    "vector")          deps="" ;;
  esac

  for i in $deps; do
#    echo "dep: "$i

    src="$i.cpp"
    header="$i.h"
    obj="../out/$i.o"

    if [ $src -nt $classObj ]; then
      return 1
    elif [ $header -nt $classObj ]; then
      return 1
    fi
  done
  
  return 0
}

makeAll()
{
  local exitCode

  exitCode=0

  cd src

  mkdir -p $outDir

  objs=""

  for i in $srcs; do
    obj="../out/$i.o"
    objs=$objs" ""$obj"
  done

  bIsNeedLink=0

  while [ 1 ]; do
    bHasNewObject=0

    for i in $srcs; do
      src="$i.cpp"
      header="$i.h"
      obj="../out/$i.o"

      bIsNeedCompile=0

      if [ ! -f $obj ]; then
        bIsNeedCompile=1
      elif [ $src -nt $obj ]; then
        bIsNeedCompile=1
      elif [ $header -nt $obj ]; then
        bIsNeedCompile=1
      else
        isDependencyChanged $i
    
        bIsNeedCompile=$?

#        echo $bIsNeedCompile
#    
#        if [ $bIsNeedCompile == 1 ]; then
#          echo "need CC $i"
#        else
#          echo "no need CC $i"
#        fi
      fi

      if [ $bIsNeedCompile == 1 ]; then
        echo "CC $i"

        if [ ! -f $src ]; then
          echo "no such file: $src" >&2
          exit 1
        fi

        if [ "$bIsVerbose" -eq 1 ]; then
          set -x
        fi

        g++ $parameters -c -o $obj $src

        exitCode=$?

        set +x

        if [ $exitCode -ne 0 ]; then
          break
        fi

        bHasNewObject=1
      fi
    done

    if [ $exitCode -ne 0 ]; then
      break
    fi

    if [ $bHasNewObject == 1 ]; then
      bIsNeedLink=1
    else
      break
    fi
  done

  if [ $exitCode -eq 0 ]; then
    if [ ! -f $exe ] || [ $bIsNeedLink == 1 ]; then
      echo "LD $exe"

      if [ "$bIsVerbose" -eq 1 ]; then
        set -x
      fi

      g++  -g -O0  \
        -I/usr/include/SDL2 \
        -D_GNU_SOURCE=1 -D_REENTRANT \
        -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 \
        -I/usr/lib64/glib-2.0/include -I/usr/include/sysprof-4 \
        -pthread \
        -o $exe \
        $objs \
        -lGLU -lGL  -lSDL2 -lfreetype -lSDL2_image

      exitCode=$?

      set +x
    fi
  fi

  cd ..

  return $exitCode
}

makeClean()
{
  local exitCode

  exitCode=0

  if [ "$bIsVerbose" -eq 1 ]; then
    set -x
  fi

  rm -f $outDir/*

  exitCode=$?

  set +x

  return $exitCode
}

makeRun()
{
  local exitCode

  exitCode=0

  makeAll

  exitCode=$?

  if [ $exitCode -ne 0 ]; then
    return $exitCode
  fi

  if [ "$bIsVerbose" -eq 1 ]; then
    set -x
  fi

  $exe

  exitCode=$?

  set +x

  return $exitCode
}

makeInstall()
{
  local exitCode

  exitCode=0

  makeAll

  exitCode=$?

  if [ $exitCode -ne 0 ]; then
    return $exitCode
  fi

  if [ "$bIsVerbose" -eq 1 ]; then
    set -x
  fi

  if [ $exitCode -eq 0 ]; then
    install -d -m 755 $installBinDir
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ]; then
    install -d -m 755 $installShareDir
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ]; then
    install -d -m 755 $installArtDir
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ]; then
    install -d -m 755 $installModelsDir
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ]; then
    install -m 755 $exe $installBinDir
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ]; then
    install -m 644 $modelsDir/*.obj $installModelsDir
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ]; then
    install -m 644 $artDir/$whiteSqauresImage $installArtDir
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ]; then
    install -m 644 $artDir/$blackSqauresImage $installArtDir
    exitCode=$?
  fi

  set +x

  return $exitCode
}

makeUninstall()
{
  local exitCode

  exitCode=0

  if [ "$bIsVerbose" -eq 1 ]; then
    set -x
  fi

  if [ $exitCode -eq 0 ]; then
    rm -f $installArtDir/$blackSqauresImage
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ]; then
    rm -f $installArtDir/$whiteSqauresImage
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ]; then
    rm -f $installModelsDir/*.obj
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ]; then
    rm -f $installBinDir/$programName
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ] && [ -d $installModelsDir ]; then
    rmdir $installModelsDir
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ] && [ -d $installArtDir ]; then
    rmdir $installArtDir
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ] && [ -d $installShareDir ]; then
    rmdir $installShareDir
    exitCode=$?
  fi

  if [ $exitCode -eq 0 ] && [ -d $installBinDir ]; then
    rmdir $installBinDir
    exitCode=$?
  fi

  set +x

  return $exitCode
}

makeRunInstalled()
{
  local exitCode

  exitCode=0

  if [ "$bIsVerbose" -eq 1 ]; then
    set -x
  fi

  $installBinDir/$programName

  exitCode=$?

  set +x

  return $exitCode
}

printTargets()
{
  echo "Usage: $0 [V=1] [all|run|install|clean]"

  return 0
}

main()
{
  local exitCode

  exitCode=0

  bIsVerbose=0

  if [ "$1" == "V=1" ]; then
    bIsVerbose=1
    PS4=
    shift
  elif [ "$1" == "V=0" ] || [ "$1" == "V=" ]; then
    shift
  fi

  target=$1

  if [ "$target" == "" ]; then
    target="all"
  fi

  case $target in
    "all")          makeAll ;;
    "clean")        makeClean ;;
    "run")          makeRun ;;
    "install")      makeInstall ;;
    "uninstall")    makeUninstall ;;
    "runInstalled") makeRunInstalled ;;
    *)              printTargets ;;
  esac

  exitCode=$?

  return $exitCode
}

main $*

exit $?
