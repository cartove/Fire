#ifndef __LIGHT_SOURCE_H__
#define __LIGHT_SOURCE_H__

#include"include/Angel.h"

class LightSource {
    public:
        vec4 ambient;
        vec4 diffuse;
        vec4 specular;

        //a Point (w=1) on which light source is located
        vec4 position;

        LightSource(vec4 position, vec4 ambient, vec4 diffuse,
                vec4 specular);

        vec4 getDirection();
        vec4 getDirectionFromPoint(vec4 point);
};

#endif
