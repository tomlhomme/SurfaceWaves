#include "screen.hpp"

Screen::Screen()
{
    mWindow=NULL;
    mRenderer=NULL;
    mTexture=NULL;

    mWindowWidth=0;
    mWindowHeight=0;

    mResWidth=0;
    mResHeight=0;

    mColorDepth=32;
    mPitch=mResWidth*mColorDepth/8;
}

Screen::~Screen()
{
    if (mTexture!=NULL)
    {
        SDL_DestroyTexture(mTexture);
    }
    if (mRenderer!=NULL)
    {
        SDL_DestroyRenderer(mRenderer);
    }
    if (mWindow!=NULL)
    {
        SDL_DestroyWindow(mWindow);
    }
}

bool Screen::init(int windowWidth,int windowHeight,int resWidth, int resHeight)
{
    mWindowWidth=windowWidth;
    mWindowHeight=windowHeight;

    if (resWidth==-1)
    {
        mResWidth=windowWidth;
    } else
    {
        mResWidth=resWidth;
    }
    if (resHeight==-1)
    {
        mResHeight=windowHeight;
    } else
    {
        mResHeight=resHeight;
    }

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        std::cout << "Error initialising SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    //Create window
    mWindow = SDL_CreateWindow( "", SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN );
    if( mWindow == NULL )
    {
        std::cout << "Error creating window: " << SDL_GetError() << std::endl;
        return false;
    }

    //Create renderer for window
    mRenderer = SDL_CreateRenderer( mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if( mRenderer == NULL )
    {
        std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_STREAMING, mResWidth, mResHeight);
    if( mTexture == NULL )
    {
        std::cout << "Error creating texture: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

Uint32 convert_color(double red, double green, double blue)
{
    // Convert the colours to unsigned integers.
    unsigned char r = static_cast<unsigned char>((red) * 255.0);
    unsigned char g = static_cast<unsigned char>((green) * 255.0);
    unsigned char b = static_cast<unsigned char>((blue) * 255.0);

    Uint32 pixelColor = (r << 24) + (g << 16) + (b << 8) + 255;

    return pixelColor;
}

bool Screen::set_pixels(double* rChannel, double* gChannel, double* bChannel)
{
    Uint32* px;
    int pitch;
    short rVal, gVal, bVal;
    //std::cout << "go"<< std::endl;
    SDL_LockTexture( mTexture, NULL, (void**)&px, &pitch );
    //std::cout << "go"<< std::endl;
    // set pixels to solid white
    for (int i=0;i<mResHeight;i++){
        for (int j = 0; j < mResWidth; j++) {
            int idx = j+mResWidth*i;
            //std::cout<<i << " " << j << " " <<rChannel[idx] <<std::endl;
            px[idx] = convert_color(rChannel[idx],gChannel[idx],bChannel[idx]);
            //std::cout<<i << " " << j << std::endl;
        }
    }
    SDL_UnlockTexture(mTexture);
    return true;
}

bool Screen::render()
{
    //printf("ren1\n");
    SDL_RenderCopy(mRenderer,mTexture,NULL,NULL);
    //printf("ren2\n");
    SDL_RenderPresent(mRenderer);
    //printf("ren\n");
    return true;
}
