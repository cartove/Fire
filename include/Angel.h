//////////////////////////////////////////////////////////////////////////////
//
//  --- Angel.h ---
//
//   The main header file for all examples from Angel 6th Edition
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __ANGEL_H__
#define __ANGEL_H__

//----------------------------------------------------------------------------
//
// --- Include system headers ---
//

#include <cmath>
#include <iostream>
#include <stdio.h>

//  Define M_PI in the case it's not defined in the math header file
#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

//----------------------------------------------------------------------------
//
// --- Include OpenGL header files and helpers ---
//
//   The location of these files vary by operating system.  We've included
//     copies of open-soruce project headers in the "GL" directory local
//     this this "include" directory.
//

#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>


// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

//----------------------------------------------------------------------------
//
//  --- Include our class libraries and constants ---
//

namespace Angel {

//  Helper function to load vertex and fragment shader files
GLuint InitShader( const char* vertexShaderFile,
           const char* fragmentShaderFile );

//  Defined constant for when numbers are too small to be used in the
//    denominator of a division operation.  This is only used if the
//    DEBUG macro is defined.
const GLfloat  DivideByZeroTolerance = GLfloat(1.0e-07);

//  Degrees-to-radians constant
const GLfloat  DegreesToRadians = M_PI / 180.0;

}  // namespace Angel

#include "vec.h"
#include "mat.h"
#include "CheckError.h"

#define Print(x)  do { std::cerr << #x " = " << (x) << std::endl; } while(0)

//  Globally use our namespace in our example programs.
using namespace Angel;

typedef vec3 color3;
typedef vec4 color4;
typedef vec4 point4;



inline
mat4 Translate( const GLfloat x, const GLfloat y, const GLfloat z )
{
    mat4 c;
    c[0][3] = x;
    c[1][3] = y;
    c[2][3] = z;
    return c;
}

inline
mat4 Translate( const vec3& v )
{
    return Translate( v.x, v.y, v.z );
}

inline
mat4 Translate( const vec4& v )
{
    return Translate( v.x, v.y, v.z );
}
inline
mat4 Ortho( const GLfloat left, const GLfloat right,
        const GLfloat bottom, const GLfloat top,
        const GLfloat zNear, const GLfloat zFar )
{
    mat4 c;
    c[0][0] = 2.0/(right - left);
    c[1][1] = 2.0/(top - bottom);
    c[2][2] = 2.0/(zNear - zFar);
    c[3][3] = 1.0;
    c[0][3] = -(right + left)/(right - left);
    c[1][3] = -(top + bottom)/(top - bottom);
    c[2][3] = -(zFar + zNear)/(zFar - zNear);
    return c;
}

inline
mat4 Ortho2D( const GLfloat left, const GLfloat right,
          const GLfloat bottom, const GLfloat top )
{
    return Ortho( left, right, bottom, top, -1.0, 1.0 );
}

inline
mat4 Frustum( const GLfloat left, const GLfloat right,
          const GLfloat bottom, const GLfloat top,
          const GLfloat zNear, const GLfloat zFar )
{
    mat4 c;
    c[0][0] = 2.0*zNear/(right - left);
    c[0][2] = (right + left)/(right - left);
    c[1][1] = 2.0*zNear/(top - bottom);
    c[1][2] = (top + bottom)/(top - bottom);
    c[2][2] = -(zFar + zNear)/(zFar - zNear);
    c[2][3] = -2.0*zFar*zNear/(zFar - zNear);
    c[3][2] = -1.0;
    c[3][3] = 0.0;
    return c;
}

inline
mat4 Perspective( const GLfloat fovy, const GLfloat aspect,
          const GLfloat zNear, const GLfloat zFar)
{
    GLfloat top   = tan(fovy*DegreesToRadians/2) * zNear;
    GLfloat right = top * aspect;

    mat4 c;
    c[0][0] = zNear/right;
    c[1][1] = zNear/top;
    c[2][2] = -(zFar + zNear)/(zFar - zNear);
    c[2][3] = -2.0*zFar*zNear/(zFar - zNear);
    c[3][2] = -1.0;
    c[3][3] = 0.0;
    return c;
}

//----------------------------------------------------------------------------
//
//  Viewing transformation matrix generation
//

inline
mat4 LookAt( const vec4& eye, const vec4& at, const vec4& up )
{
    vec4 n = normalize(eye - at);
    vec4 u = vec4(normalize(cross(up,n)));
    vec4 v = vec4(normalize(cross(n,u)));
    vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
    mat4 c = mat4(u, v, n, t);
    return c * Translate( -eye );
}

//----------------------------------------------------------------------------
//
// Generates a Normal Matrix
//
inline
mat3 Normal( const mat4& c)
{
   mat3 d;
   GLfloat det;
   det = c[0][0]*c[1][1]*c[2][2]+c[0][1]*c[1][2]*c[2][1]
     -c[2][0]*c[1][1]*c[0][2]-c[1][0]*c[0][1]*c[2][2]-c[0][0]*c[1][2]*c[2][1];
   d[0][0] = (c[1][1]*c[2][2]-c[1][2]*c[2][1])/det;
   d[0][1] = -(c[0][1]*c[2][2]-c[0][2]*c[2][1])/det;
   d[0][2] = (c[0][1]*c[2][0]-c[2][1]*c[2][2])/det;
   d[1][0] = -(c[0][1]*c[2][2]-c[0][2]*c[2][1])/det;
   d[1][1] = (c[0][0]*c[2][2]-c[0][2]*c[2][0])/det;
   d[1][2] = -(c[0][0]*c[2][1]-c[2][0]*c[0][1])/det;
   d[2][0] = (c[0][1]*c[1][2]-c[1][1]*c[0][2])/det;
   d[2][1] = -(c[0][0]*c[1][2]-c[0][2]*c[1][0])/det;
   d[2][2] = (c[0][0]*c[1][1]-c[1][0]*c[0][1])/det;

  return d;
}
#endif // __ANGEL_H__
