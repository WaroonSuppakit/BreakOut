#include <stdlib.h>
#include "cp_functions.h"

SDL_Window *window = NULL;
SDL_GLContext context;
unsigned int message_texture;

Texture cpLoadTexture(const char *filename)
{   unsigned int tex_id;
    int type;
    Texture texture;

    SDL_Surface *image = IMG_Load(filename);
    if (!image) {
        fprintf(stderr, "IMG_Load %s failed!\n", filename);
        return NULL;
    }

    texture = (Texture)malloc(sizeof(TextureStruct));
    if (!texture) {
        fprintf(stderr, "%s allocation failed!\n", filename);
        SDL_FreeSurface(image);
        return NULL;
    }

    if (image->format->BytesPerPixel == 3)
        type = GL_RGB;
    else
        type = GL_RGBA;

    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, type, image->w, image->h, 0, type,
        GL_UNSIGNED_BYTE, image->pixels);

    texture->tex_id = tex_id;
    texture->width = image->w;
    texture->height = image->h;
    SDL_FreeSurface(image);

    return texture;
}

Font cpLoadFont(const char *font_file, int font_size)
{
    return TTF_OpenFont(font_file, font_size);
}

Music cpLoadMusic(const char *music_file)
{
    return Mix_LoadMUS(music_file);
}

Sound cpLoadSound(const char *sound_file)
{
    return Mix_LoadWAV(sound_file);
}

void cpPlayMusic(Music music)
{
    Mix_PlayMusic(music, -1);
}

void cpPlaySound(Sound sound)
{
    Mix_PlayChannel(-1, sound, 0);
}

void cpFreeFont(Font font)
{
    TTF_CloseFont(font);
}

void cpFreeMusic(Music music)
{
    Mix_FreeMusic(music);
}

void cpFreeSound(Sound sound)
{
    Mix_FreeChunk(sound);
}

void cpCheckSDLError(int line)
{  const char *error = SDL_GetError();

    if (error[0]) {
        fprintf(stderr, "SDL Error: %s\nLine: %d!\n", error, line);
        SDL_ClearError();
    }
}

int cpInit(const char *title, int win_width, int win_height)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
        return False;

    window = SDL_CreateWindow(title,
                              SDL_WINDOWPOS_CENTERED, 
                              SDL_WINDOWPOS_CENTERED,
                              win_width, win_height,
                              SDL_WINDOW_OPENGL);
    if (!window)
        return False;

    context = SDL_GL_CreateContext(window);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1);

    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (!(IMG_Init(flags) & flags)) {
        fprintf(stderr, "IMG_Init failed: %s!\n", IMG_GetError());
        return False;
    }

    if (TTF_Init() == -1)
        return False;

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
        return False;

    glClearColor(0, 0, 0, 1);
    glViewport(0, 0, win_width, win_height);
    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, win_width, win_height, 0, 0, 1);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGenTextures(1, &message_texture);

    return True;
}

void cpCleanUp()
{
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void cpSwapBuffers()
{
    SDL_GL_SwapWindow(window);
}

void cpDelay(int millisecond)
{
    SDL_Delay(millisecond);
}

int cbEventListener(Event *event)
{
    SDL_PollEvent(event);
}

void cpDrawTexture(int r, int g, int b,
    int x, int y, int width, int height, Texture texture)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture->tex_id);
    glColor4ub(r, g, b, 255);
    glBegin(GL_QUADS);
        glTexCoord2d(0, 0); glVertex2f(x, y);
        glTexCoord2d(1, 0); glVertex2f(x + width, y);
        glTexCoord2d(1, 1); glVertex2f(x + width, y + height);
        glTexCoord2d(0, 1); glVertex2f(x, y + height);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void cpDrawText(int r, int g, int b,
    int x, int y, const char *text, Font font, int center)
{   SDL_Surface *message;
    SDL_Color color = {r, g, b};
    SDL_Rect offset;
    int texture_format, xb, xe, yb, ye;

    message = TTF_RenderUTF8_Blended(font, text, color);
    if (!message)
        return;

    if (message->format->BytesPerPixel == 3)
        texture_format = GL_RGB;
    else
        texture_format = GL_RGBA;

    if (center) {
        xb = -message->w/2;
        xe = message->w/2;
        yb = -message->h/2;
        ye = message->h/2;
    }
    else {
        xb = 0;
        xe = message->w;
        yb = 0;
        ye = message->h;
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, message_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, message->w, message->h, 0,
        texture_format, GL_UNSIGNED_BYTE, message->pixels);
    glColor4ub(r, g, b, 255);
    glBegin(GL_QUADS);
        glTexCoord2d(0, 0); glVertex2f(x + xb, y + yb);
        glTexCoord2d(1, 0); glVertex2f(x + xe, y + yb);
        glTexCoord2d(1, 1); glVertex2f(x + xe, y + ye);
        glTexCoord2d(0, 1); glVertex2f(x + xb, y + ye);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    SDL_FreeSurface(message);
}

void cpClearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT);
}
