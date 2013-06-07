#include "light_source.h"

LightSource::LightSource(vec4 position, vec4 ambient,
         vec4 diffuse, vec4 specular) {
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular= specular;

    this->position = position;
}

vec4  LightSource::getDirection() {
    vec4 a = normalize(this->position);
    a[3] = 0.0;
    return a;
}

vec4 LightSource::getDirectionFromPoint(vec4 point) {
    return normalize(this->position - point);
}
