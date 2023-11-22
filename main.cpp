#include "screen.hpp"
#include <time.h>
#include <math.h>
#include "surface.hpp"


const double RADIUS=0.5;
const double LX=1000.;
const double LY=1000.;
const double DT=0.02;
const double DX=0.1;

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

Screen screen;


int main( int argc, char* args[] )
{
    Waves::WaveSurface ws(SCREEN_HEIGHT, SCREEN_WIDTH, DX);


    Numerica::Vec1d light(3);
    light(0)=1./sqrt(102);
    light(1)=1./sqrt(102);
    light(2)=10./sqrt(102);

    Numerica::Vec3d color(3,SCREEN_HEIGHT, SCREEN_WIDTH);


    screen.init(SCREEN_WIDTH,SCREEN_HEIGHT);

    for (std::size_t i=0;i<SCREEN_HEIGHT;i++)
    {
        for (std::size_t j=0;j<SCREEN_WIDTH;j++)
        {
            //std::cout << i << " " << j << std::endl;
            ws.mHeight(i,j)=0.5;//+0.5/(SCREEN_HEIGHT*SCREEN_WIDTH-1);
            ws.mHeightPrev(i,j)=0.5;
            //std::cout << "Set initial height" << std::endl;
        }
    }

    //h(SCREEN_WIDTH/2,SCREEN_HEIGHT/2)=0;
    ws.displace_sphere(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,0.5,5.);

    //std::cout<<vec()
    int i=0;
    SDL_Event e; bool quit = false;

    double x=SCREEN_WIDTH/2;
    double y=SCREEN_HEIGHT/2;
    double u=0.;
    double v=0.;

    double vchange=0.05;

    //std::cout << ws.mHeight(-1,40);
    while( quit==false ) {
        //printf("i\n");
        i++;
        while( SDL_PollEvent( &e ) ) {
          //User presses a key

          //User requests quit
          if( e.type == SDL_QUIT )
          {
              quit = true;
          }
          else if( e.type == SDL_KEYDOWN )
          {
              //Select surfaces based on key press
              //printf(" %lf\n",v);
              switch( e.key.keysym.sym )
              {
                  case SDLK_UP:
                  v-=vchange;
                  //printf("up %lf\n",v);
                  break;

                  case SDLK_DOWN:
                  v+=vchange;
                  break;

                  case SDLK_LEFT:
                  u-=vchange;
                  break;

                  case SDLK_RIGHT:
                  u+=vchange;
                  break;

                  case SDLK_SPACE:
                  u=0;
                  v=0;
                  break;

              }
          }
        }

        x+=u;
        y+=v;

        x=fmod(x+SCREEN_WIDTH,SCREEN_WIDTH);
        y=fmod(y+SCREEN_HEIGHT,SCREEN_HEIGHT);

        std::size_t xind = static_cast<std::size_t>(x);
        std::size_t yind = static_cast<std::size_t>(y);

        clock_t t1=clock();
        ws.displace_sphere(x,y, 0.5,5.);
        clock_t t2=clock();

        ws.update_height();
        clock_t t3=clock();

        ws.displace_sphere(x,y, 0.5,5.);


        if (i%1==0)
        {

            clock_t t4=clock();
            ws.compute_normal();
            clock_t t5=clock();

            ws.compute_diffuse(light);
            clock_t t6=clock();

            ws.compute_color(color);
            clock_t t7=clock();

            screen.set_pixels(color.data_ptr(),
                &(color.data_ptr()[SCREEN_HEIGHT*SCREEN_WIDTH]),
                &(color.data_ptr()[2*SCREEN_HEIGHT*SCREEN_WIDTH]));

            screen.render();

            std::cout << "disp " << t2-t1<<"\n";
            std::cout << "update " << t3-t2<<"\n";
            std::cout << "normal " << t5-t4<<"\n";
            std::cout << "diffuse " << t6-t5<<"\n";
            std::cout << "color " << t7-t6<<"\n";
        }
    }
    return 0;
}
