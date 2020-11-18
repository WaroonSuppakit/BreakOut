#ifndef __CP_FUNCTIONS_H__
#define __CP_FUNCTIONS_H__

// SDL2 Headers
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#define True      1
#define False     0

#define QUIT      SDL_QUIT
#define KEYUP     SDL_KEYUP
#define KEYDOWN   SDL_KEYDOWN
#define K_ESCAPE  SDLK_ESCAPE
#define K_LEFT    SDLK_LEFT
#define K_RIGHT   SDLK_RIGHT

typedef TTF_Font  *Font;
typedef Mix_Music *Music;
typedef Mix_Chunk *Sound;
typedef SDL_Event Event;

typedef struct {
    unsigned int tex_id;
    int width, height;
} TextureStruct; 
typedef TextureStruct *Texture;

Texture cpLoadTexture(const char *filename);
Font cpLoadFont(const char *font_file, int font_size);
Music cpLoadMusic(const char *music_file);
Sound cpLoadSound(const char *sound_file);
void cpPlayMusic(Music music);
void cpPlaySound(Sound sound);
void cpFreeFont(Font font);
void cpFreeMusic(Music music);
void cpFreeSound(Sound sound);
int cpInit(const char *title, int win_width, int win_height);
void cpCleanUp();
void cpSwapBuffers();
void cpDelay(int millisecond);
int cbEventListener(Event *event);
void cpDrawTexture(int r, int g, int b, 
                   int x, int y, int width, int height, Texture texture);
void cpDrawText(int r, int g, int b,
                int x, int y, const char *text, Font font, int center);
void cpClearScreen();

#endif // __CP_FUNCTIONS_H__
