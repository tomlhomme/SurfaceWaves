#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "Numerica/numerica.hpp"
#include <omp.h>
#include <iostream>

namespace Waves
{
    class Surface
    {
    public:
        Surface(std::size_t h, std::size_t w, double dx);
        ~Surface();

        void compute_normal();
        void compute_diffuse(const Numerica::Vec1d& light_dir);
        void compute_specular(Numerica::Vec1d& light_dir);
        void compute_color(Numerica::Vec3d& color) const;

        Numerica::Vec1d mDiffuseColor;
        Numerica::Vec1d mSpecularColor;
        Numerica::Vec2d mHeight;
    protected:
        Numerica::Vec3d mUnitNormal;
        Numerica::Vec2d mDiffuse;
        Numerica::Vec2d mSpecular;
        double mGridSpacing;
    };

    class WaveSurface: public Surface
    {
    public:
        WaveSurface(std::size_t h, std::size_t w, double dx);
        ~WaveSurface();
        void update_height();
        void displace_sphere(double xpos, double ypos,double z, double rad);

        Numerica::Vec2d mHeightPrev;
        Numerica::Vec2d mHeightNext;
    private:
        double mPropagation;
        double mFriction;
        double mTimeStep;
    };
}


#endif