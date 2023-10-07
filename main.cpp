#include "screen.hpp"
#include <time.h>
#include <math.h>


const double RADIUS=0.5;
const double LX=1000.;
const double LY=1000.;
const double DT=0.02;
const double DX=0.1;
const long unsigned int NPARTICLES = 20000;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int wi=SCREEN_WIDTH;
const int he=SCREEN_HEIGHT;
Screen screen;

class Vec_2d
{
public:
    long unsigned int mShape[2]={0,0};
    double* mData;
    // Overloading () operator to access elements in array style
    double& operator() (long unsigned int, long unsigned int);

    Vec_2d(long unsigned int s1, long unsigned int s2);
    ~Vec_2d();
};

double& Vec_2d::operator() (long unsigned int xdim, long unsigned int ydim)
{
    //std::cout << "hi" << std::endl;
    return mData[xdim+ydim*mShape[0]];
}

Vec_2d::Vec_2d(long unsigned int s1, long unsigned int s2)
{
    mShape[0]=s1; mShape[1]=s2;
    mData=(double*)malloc(mShape[0]*mShape[1]*sizeof(double));
    /*
    for (int j=0;j<s1;j++)
    {
        for (int i=0;i<s2;i++)
        {
            mData[i+j*s2]=0;
        }
    }
    */
}

Vec_2d::~Vec_2d()
{
    free(mData);
}



class Vec_1d
{
public:
    long unsigned int mLength;
    double* mData;
    // Overloading [] operator to access elements in array style
    double& operator() (long unsigned int);

    Vec_1d(long unsigned int s);
    ~Vec_1d();
};

double& Vec_1d::operator() (long unsigned int xdim)
{
    //std::cout << "hi" << std::endl;
    return mData[xdim];
}

Vec_1d::Vec_1d(long unsigned int s)
{
    mLength=s;
    mData=(double*)malloc(s*sizeof(double));
}

Vec_1d::~Vec_1d()
{
    free(mData);
}

double a=5.;
double k=0.1;

void update(Vec_2d& h, Vec_2d& h_prev, Vec_2d& h_next)
{
    double adt2_dx2=a*DT*DT/(DX*DX);
    double kdt=k*DT;
    for (int j=0;j<SCREEN_HEIGHT;j++)
    {
        for (int i=0;i<SCREEN_WIDTH;i++)
        {
            h_next(i,j)=-h_prev(i,j)+2*h(i,j)
              + adt2_dx2*(-4*h(i,j) + h((i+1+wi)%wi,j) + h(i,(j+1+he)%he)
              + h((i-1+wi)%wi,j) + h(i,(j-1+he)%he))
              - kdt*(h(i,j) - h_prev(i,j));
            //std::cout << i << " " << j << " " << h(i,j)<<std::endl;
        }
    }
}

void displace_sphere(Vec_2d& h, double xpos, double ypos, double rad)
{
    int xmin,xmax,ymin,ymax;
    xmin=floor(xpos-rad);
    xmax=ceil(xpos+rad);
    ymin=floor(ypos-rad);
    ymax=ceil(ypos+rad);

    double mass_displaced=0;
    double area=0;
    //printf("l1\n");
    for (int j=ymin;j<ymax;j++)
    {
        for (int i=xmin;i<xmax;i++)
        {
            double id=(double)i; double jd=(double)j;
            double dist2=(id-xpos)*(id-xpos) + (jd-ypos)*(jd-ypos);
            double sheight = 0.1*(rad-sqrt(fmax(0,rad*rad-dist2)));
            //printf("%d %d\n",(i+wi)%wi, (j+he)%he);
            //printf("ss%d %d\n", i, j);
            if (h((i+wi)%wi,(j+he)%he)>sheight)
            {
                mass_displaced +=h((i+wi)%wi,(j+he)%he)-sheight;
                area+=1;
                h((i+wi)%wi,(j+he)%he)=sheight;
            }
        }
    }
    double addon=mass_displaced/(SCREEN_WIDTH*SCREEN_HEIGHT-area);
    //printf("l2\n");
    for (int j=0;j<SCREEN_HEIGHT;j++)
    {
        for (int i=0;i<SCREEN_WIDTH;i++)
        {
            double id=(double)i; double jd=(double)j;
            double dist2=(id-xpos)*(id-xpos) + (jd-ypos)*(jd-ypos);
            if (dist2>rad)
            {
                h(i,j)+=addon;
            }

        }
    }
}

void compute_unit_normal(Vec_2d& h, Vec_2d& nx,Vec_2d& ny, Vec_2d& nz)
{
    double ddx,ddy,mag;
    for (int j=0;j<SCREEN_HEIGHT;j++)
    {
        for (int i=0;i<SCREEN_WIDTH;i++)
        {
            ddx=(h((i+1)%wi,j)-h((i-1+wi)%wi,j))/(2.*DX);
            ddy=(h(i,(j+1)%he)-h(i,(j-1+he)%he))/(2.*DX);
            mag=sqrt(ddx*ddx+ddy*ddy+1);
            nx(i,j)=ddx/mag;
            ny(i,j)=ddy/mag;
            nz(i,j)=1/mag;
        }
    }
}

void compute_diffuse(Vec_1d& light,Vec_2d& nx,Vec_2d& ny, Vec_2d& nz, Vec_2d& diffuse )
{
    for (int j=0;j<SCREEN_HEIGHT;j++)
    {
        for (int i=0;i<SCREEN_WIDTH;i++)
        {
            //printf("%d %d\n", i, j);
            diffuse(i,j)=light(0)*nx(i,j)+light(1)*ny(i,j)+light(2)*nz(i,j);
        }
    }
    //printf("fin\n");
}


int main( int argc, char* args[] )
{
    //Vec_2d vec = Vec_2d(SCREEN_WIDTH,SCREEN_HEIGHT);
    //std::cout << vec(0,1) <<std::endl;
    Vec_2d h = Vec_2d(SCREEN_WIDTH,SCREEN_HEIGHT);
    Vec_2d h_prev = Vec_2d(SCREEN_WIDTH,SCREEN_HEIGHT);
    Vec_2d h_next = Vec_2d(SCREEN_WIDTH,SCREEN_HEIGHT);

    Vec_2d nx = Vec_2d(SCREEN_WIDTH,SCREEN_HEIGHT);
    Vec_2d ny = Vec_2d(SCREEN_WIDTH,SCREEN_HEIGHT);
    Vec_2d nz = Vec_2d(SCREEN_WIDTH,SCREEN_HEIGHT);

    Vec_2d diffuse = Vec_2d(SCREEN_WIDTH,SCREEN_HEIGHT);

    Vec_1d light=Vec_1d(3);
    light(0)=1./sqrt(102);
    light(1)=1./sqrt(102);
    light(2)=10./sqrt(102);


    screen.init(SCREEN_WIDTH,SCREEN_HEIGHT);

    for (int i=0;i<SCREEN_WIDTH;i++)
    {
        for (int j=0;j<SCREEN_HEIGHT;j++)
        {
            h(i,j)=0.5;//+0.5/(SCREEN_HEIGHT*SCREEN_WIDTH-1);
            h_prev(i,j)=0.5;

        }
    }

    //h(SCREEN_WIDTH/2,SCREEN_HEIGHT/2)=0;
    displace_sphere(h,SCREEN_WIDTH/2,SCREEN_HEIGHT/2,5.);

    //std::cout<<vec()
    int i=0;
    SDL_Event e; bool quit = false;

    double x=SCREEN_WIDTH/2;
    double y=SCREEN_HEIGHT/2;
    double u=0.;
    double v=0.;

    double vchange=0.1;

    while( quit == false ) {
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

        clock_t t2=clock();
        //printf("sph1\n");
        displace_sphere(h,x,y,5.);
        //printf("sph2\n");
        update(h,h_prev,h_next);
        //printf("sph3\n");
        displace_sphere(h,x,y,5.);
        //std::cout<<h(0,0)<<std::endl;
        double* tmp=h_prev.mData;
        h_prev.mData=h.mData;
        h.mData=h_next.mData;
        h_next.mData=tmp;

        //std::cout << "hi"<<std::endl;
        //std::cout<< h(0,0);
        if (i%3==0)
        {
            //printf("sph4\n");
            //printf("n\n");
            compute_unit_normal(h,nx,ny,nz);
            //printf("ii\n");
            compute_diffuse(light,nx,ny,nz,diffuse);

            double max,min;
            for (int j=0;j<SCREEN_HEIGHT;j++)
            {
                for (int i=0;i<SCREEN_WIDTH;i++)
                {
                    //printf("%lf\n",diffuse(i,j));
                    if (diffuse(i,j)<min)
                        min=diffuse(i,j);
                    if (diffuse(i,j)>max)
                        max=diffuse(i,j);
                }
            }
            //printf("%lf %lf", min,max);
            //printf("jj\n");
            screen.set_pixels(diffuse.mData,diffuse.mData,diffuse.mData);
            //printf("ff\n");
            //std::cout << "yes"<<std::endl;
            //printf("sph5\n");
            screen.render();

            //printf("sph6\n");
        }
        clock_t t3=clock();
        //std::cout<<"Render: "<<(double)((t3-t2)* 1000) / CLOCKS_PER_SEC<<std::endl;


    }
    return 0;
}
