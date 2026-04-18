#!/bin/bash -e

rm -rf _d/SDL_mixer
git clone --depth=1 https://github.com/libsdl-org/SDL_mixer _d/SDL_mixer/s
cmake -S _d/SDL_mixer/s -B _d/SDL_mixer/b \
  -D CMAKE_PREFIX_PATH=id \
  -D CMAKE_INSTALL_PREFIX=id \
  -D CMAKE_CXX_STANDARD=23 \
  -D BUILD_SHARED_LIBS=OFF \
  -D SDLMIXER_TESTS=OFF \
  -D SDLMIXER_EXAMPLES=OFF \
  -D SDLMIXER_FLAC=OFF \
  -D SDLMIXER_GME=OFF \
  -D SDLMIXER_MOD=OFF \
  -D SDLMIXER_MOD_XMP=OFF \
  -D SDLMIXER_MP3=OFF \
  -D SDLMIXER_MIDI=OFF \
  -D SDLMIXER_MIDI_FLUIDSYNTH=OFF \
  -D SDLMIXER_MIDI_TIMIDITY=OFF \
  -D SDLMIXER_OPUS=OFF \
  -D SDLMIXER_VORBIS_STB=OFF \
  -D SDLMIXER_VORBIS_VORBISFILE=OFF \
  -D SDLMIXER_WAVPACK=OFF \
  -D CMAKE_BUILD_TYPE=Debug
cmake --build _d/SDL_mixer/b --target install -j
cmake _d/SDL_mixer/b -D CMAKE_BUILD_TYPE=Release
cmake --build _d/SDL_mixer/b --target install -j


