#ifndef __CUBE_H__
#define __CUBE_H__

#include "drawable.h"
#include <vector>


class Particle : public Drawable {
    public:
        //constructors
        Particle(vec4 center, float width ,float hight, float depth);

        virtual void render();
        virtual void calculateNormals();

        //destructor
        ~Particle();
};

#endif
