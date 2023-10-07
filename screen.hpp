#ifndef SCREEN_HPP
#define SCREEN_HPP

//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>

/* A very simple class for rendering 2-D arrays to the screen.*/
class Screen
{
public:
    Screen();
    ~Screen();

    bool init(int windowWidth,int windowHeight,int resWidth=-1, int resHeight=-1);
    bool set_pixels(double* rChannel, double* gChannel, double* bChannel);
    bool render();

private:
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture;

    int mWindowHeight;
    int mWindowWidth;

    int mResWidth;
    int mResHeight;

    int mColorDepth;
    int mPitch;
};
#endif
