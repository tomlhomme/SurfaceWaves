#include "surface.hpp"

Waves::Surface::Surface(std::size_t h, std::size_t w, double dx):
    mDiffuseColor(3),
    mSpecularColor(3),
    mHeight(h,w),
    mUnitNormal(h,w,3),
    mDiffuse(h,w),
    mSpecular(h,w),
    mGridSpacing(dx)
{
    //std::cout << "Created surface." << std::endl;
}

Waves::Surface::~Surface()
{
}
 
void Waves::Surface::compute_normal()
{
    //std::cout << "compute normal" << std::endl;
    double ddx,ddy,recip_mag;
    const double recip_2dx=1/(2.*mGridSpacing);
    for (std::size_t i=0;i<mHeight.mSz0;i++)
    {
        const std::size_t indip1=(i+1)%mHeight.mSz0, indim1=(i-1+mHeight.mSz0)%mHeight.mSz0;
        for (std::size_t j=0;j<mHeight.mSz1;j++)
        {
            ddx=(mHeight(indip1,j)-mHeight(indim1,j))*recip_2dx;
            ddy=(mHeight(i,(j+1)%mHeight.mSz1)-mHeight(i,(j-1+mHeight.mSz1)%mHeight.mSz1))*recip_2dx;
            recip_mag=1./sqrt(ddx*ddx+ddy*ddy+1);
            mUnitNormal(i,j,0)=ddx*recip_mag;
            mUnitNormal(i,j,1)=ddy*recip_mag;
            mUnitNormal(i,j,2)=1.*recip_mag;
        }
    }
    //std::cout << "finished"<< std::endl;
}

void Waves::Surface::compute_diffuse(const Numerica::Vec1d& light_dir)
{
    //std::cout << "compute diffuse" << std::endl;
    //double max=0,min=1;
    for (std::size_t i=0;i<mHeight.mSz0;i++)
    {
        for (std::size_t j=0;j<mHeight.mSz1;j++)
        {
            //if (i==mHeight.axis_size(0)/2)
            //    std::cout<<mHeight(i,j) << " ";
            //printf("%d %d\n", i, j);
            mDiffuse(i,j)=light_dir(0)*mUnitNormal(i,j,0)+
                            light_dir(1)*mUnitNormal(i,j,1)+
                            light_dir(2)*mUnitNormal(i,j,2);
            //if (mDiffuse(i,j)>max)
            //    max=mDiffuse(i,j);
            //else if (mDiffuse(i,j)<min)   
            //    min=mDiffuse(i,j);
        }
    }
    //std::cout << std::endl;
    //std::cout<<std::endl;
    //std::cout<<max << " " << min<< std::endl;
    //printf("fin\n");
    //std::cout << "finished"<< std::endl;
}

void Waves::Surface::compute_specular(Numerica::Vec1d& light_dir)
{
;
}

void Waves::Surface::compute_color(Numerica::Vec3d& color) const
{
    //mDiffuse.print();
    //std::cout << "compute color" << std::endl;
    //std::cout << "Compute Color\n";
    for (std::size_t i=0; i< color.mSz0; i++)
    {
        for (std::size_t j=0; j<color.mSz1; j++)
        {

            for (std::size_t k=0; k<color.mSz2; k++)
            {
                if (i==0)
                    color(i,j,k)=fmax(mDiffuse(j,k),0.);
                else 
                    color(i,j,k)=0.;
            //if (j==mHeight.axis_size(0)/2)
            //    std::cout<<"col "<<color(0,j,k) << " \n";
            }
            //std::cout << i << " " << mHeight.axis_size(0)/2 << "\n";
        }
    }
    //std::cout << "finished"<< std::endl;
}

Waves::WaveSurface::WaveSurface(std::size_t h, std::size_t w, double dx): 
Surface(h,w, dx),
mHeightPrev(h,w),
mHeightNext(h,w),
mPropagation(1.),
mFriction(0.1),
mTimeStep(0.05)
{
}

Waves::WaveSurface::~WaveSurface()
{

}

void Waves::WaveSurface::update_height()
{
    const double adt2_dx2=mPropagation*mTimeStep*mTimeStep/(mGridSpacing*mGridSpacing);
    const double kdt=mFriction*mTimeStep;
    const std::size_t wi=mHeight.axis_size(1),he=mHeight.axis_size(0);
    for (std::size_t i=0;i<he;i++)
    {
        for (std::size_t j=0;j<wi;j++)
        {
            const std::size_t indip1 = (i+1+he)%he;
            const std::size_t indim1 = (i-1+he)%he;
            const double hij=mHeight(i,j);
            mHeightNext(i,j)=-mHeightPrev(i,j)+2*hij
              + adt2_dx2*(-4*hij + mHeight(indip1,j) + mHeight(i,(j+1+wi)%wi)
              + mHeight(indim1,j) + mHeight(i,(j-1+wi)%wi))
              - kdt*(hij - mHeightPrev(i,j));
        }
    }
    
    // Switch pointers.
    double* tmp=mHeightNext.data_ptr();
    mHeightNext.data_ptr()=mHeightPrev.data_ptr();
    mHeightPrev.data_ptr()=mHeight.data_ptr();
    mHeight.data_ptr()=tmp;
}

void Waves::WaveSurface::displace_sphere(double xpos, double ypos,double z, double rad)
{
    //std::cout << "displace" << std::endl;
    const int xmin=floor(xpos-rad),xmax=ceil(xpos+rad),
            ymin=floor(ypos-rad),ymax=ceil(ypos+rad);

    const std::size_t xind = static_cast<std::size_t>(xpos);
    const std::size_t yind = static_cast<std::size_t>(ypos);

    const double k=0.1;
    double mass_displaced=0;
    double area=0;

    const std::size_t wi=mHeight.axis_size(1);
    const std::size_t he=mHeight.axis_size(0);

    //double centre_height = mHeight((ymin+he)%he, (xmin+wi)%wi) + rad*k;
    //                        
    //std::cout << centre_height << std::endl;
    for (int i=ymin;i<ymax;i++)
    {
        const std::size_t indi=(i+he)%he;
        for (int j=xmin;j<xmax;j++)
        {
            const double id=(double)i, jd=(double)j;
            const double dist2=(jd-xpos)*(jd-xpos) + (id-ypos)*(id-ypos);
            double sheight;
            if (dist2>rad)
                sheight=1000.;
            else
                sheight = k*(z-sqrt(fmax(0,rad*rad-dist2)))+z;
            //std::cout << sheight << " sheight " << dist2<< " dist2 "<< id<< " id " << jd<< " jd "<< std::endl;
            //printf("%d %d\n",(i+wi)%wi, (j+he)%he);
            //printf("ss%d %d\n", i, j);
            const std::size_t indj=(j+wi)%wi;
            //std::cout << indi << " " << indj << std::endl;
            //std::cout << indi << " "<<indj << std::endl;
            if (mHeight(indi,indj)>sheight)
            {
                mass_displaced +=mHeight(indi,indj)-sheight;
                area+=1;
                mHeight(indi,indj)=sheight;
            }
            //if (i==mHeight.axis_size(0)/2)
            //    std::cout<<"disp "<<mHeight(i,j) << " ";
        }
    }
    const double addon=mass_displaced/(he*wi-area);
    //printf("l2\n");
    for (std::size_t i=0;i<he;i++)
    {
        for (std::size_t j=0;j<wi;j++)
        {
            const double id=(double)i, jd=(double)j;
            const double dist2=(jd-xpos)*(jd-xpos) + (id-ypos)*(id-ypos);
            if (dist2>rad)
            {
                
                mHeight(i,j)+=addon;
            }//std::cout << i << " " << j << std::endl;}

            //if (i==mHeight.axis_size(0)/2)
            //    std::cout<<"disp "<<mHeight(i,j) << " \n";

        }
    }
    //std::cout << "finished"<< std::endl;
}