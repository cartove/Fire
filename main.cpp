#include "include/Angel.h"
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;
#include "drawable.h"
#include "cube.h"
#include "sphere.h"
#include "mesh.h"
#include "material.h"
#include "light_source.h"
const GLfloat WindowWidth = 1366;
const GLfloat WindowHeight = 768;

// SHADER Variables

GLuint model_view_loc;
GLuint projection_loc;

GLuint light_position_loc;
GLuint eye_position_loc;
GLuint ambient_product_loc;
GLuint diffuse_product_loc;
GLuint specular_product_loc;
GLuint shininess_loc;

// Viewing Variables

//default values

GLfloat leftDefault = -400, rightDefault = 400;
GLfloat bottomDefault = -400, topDefault = 400;
GLfloat zNearDefault = 1, zFarDefault = 600;
float defaultTheta = 0, defaultPhi = 0;
bool jumping=false;

// current variables
float theta = defaultTheta;
float phi = defaultPhi;
int lastX=WindowWidth/2,lastY=WindowHeight/2;

enum ViewingMode {
    ORTHO, PERSPECTIVE_WITH_FRUSTUM, PERSPECTIVE_WITH_FOV
} viewMode;

// viewing volume
GLfloat lefty = leftDefault, righty = rightDefault;
GLfloat bottom = bottomDefault, top =  topDefault;
GLfloat zNear = zNearDefault, zFar = zFarDefault;
float zoomFactor = 1.0;
float fovy = 45; // field of view in angles with respect to y axis

vec4 eye;
vec4 at;
std::vector<Drawable*> shapes;
std::vector<LightSource*> lights;
//----------------------------------------------------------------------------

void build_model(){

    //LIGHTS
    lights.push_back(new LightSource(
                    vec4(400, 300, 400, 1.0),
//                    vec4(0, 100, 0, 1.0),
                    color4(0.5, 0.5, 0.5, 1.0),
                    color4(1.0, 1.0, 1.0, 1.0),
                    color4(1.0, 1.0, 1.0, 1.0)
                ));
    lights.push_back(new LightSource(
                    vec4(0, 0, 10, 1.0),
                    color4(1.0, 0.5, 0.5, 1.0),
                    color4(1.0, 1.0, 1.0, 1.0),
                    color4(1.0, 1.0, 1.0, 1.0)
                ));


    //cube has a default material of red plastic
    FilledCube* cube1 = new FilledCube(vec4(0,-2,0,1),100000,.1,100000);
    //change cube material to gold
    cube1->material = Material(
            color4(0.24725,0.1995,0.0745,1.0),
            color4(0.75164,0.60648,0.22648,1.0),
            color4(0.628281,0.55802,0.266065,1.0),
            51.2f
            );
    shapes.push_back(cube1);
    FilledCube* cube2= new FilledCube(vec4(0,400,0,1),100000,1,100000);
    shapes.push_back(cube2);
    cube2->material = Material(
            color4(0.8,0.8,.9,1.0),
            color4(0.01,0.1,.5,1.0),
            color4(0,0,0,1.0),
            10000.2f
            );

    Sphere* s = new FilledSphere(vec4(0, 50, 0, 1), 25, 4);
    shapes.push_back(s);
//        Mesh * mm = new Mesh("suzanne.obj",vec4(0,0,0,1));
//    shapes.push_back(mm);
}

//-----------------------------------------------------------

void setup_view() {
    //eye and at are points
    static bool first=true;
    if(first){
        eye.x = 0;
        eye.y = 10;
        eye.z = 400;
        eye.w = 1.0;
        at.x=0;
        at.y=10;
        at.z=0;
        at.w=1;
        first=false;
    }

    //std::cout << "looking from" << eye << std::endl;
    vec4 up( 0.0, 1.0, 0.0, 0.0 );

    mat4 mv;
    mat4 mp;

    mv = LookAt((eye), (at) ,(up));

    float zf = zoomFactor;

    if (viewMode == PERSPECTIVE_WITH_FOV)
        mp = Perspective(fovy, WindowWidth/WindowHeight, zNear, zFar);
    else if (viewMode == PERSPECTIVE_WITH_FRUSTUM)
        mp = Frustum(lefty/zf, righty/zf, bottom/zf, top/zf, zNear, zFar);
    else
        mp = Ortho(lefty/zf, righty/zf, bottom/zf, top/zf, -zNear, -zFar);

    glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, mv);
    glUniformMatrix4fv(projection_loc, 1, GL_TRUE, mp);
    /*
    std::cout << "left : " << left << " , right : " << right << std::endl;
    std::cout << "top : " << top << " , bottom : " << bottom << std::endl;
    std::cout << "near : " << zNear << " , far : " << zFar << std::endl;
    std::cout << "theta " << theta << std::endl;
    std::cout << "phi " << phi << std::endl;
    std::cout << "zoom Factor " << zoomFactor << std::endl;
    std::cout << "--------------" << std::endl;
    */
}

void reset_view() {


    if (viewMode == ORTHO){

        zoomFactor = 1;
    }else if (viewMode == PERSPECTIVE_WITH_FRUSTUM) {
        zoomFactor = 100;
    } else {
        fovy = 60;
    }

    lefty = leftDefault; righty = rightDefault;
    bottom = bottomDefault; top =  topDefault;
    zNear = zNearDefault; zFar = zFarDefault;

}

//-----------------------------------------------------------
void init( void )
{
    // Load shaders and use the resulting shader program
    Drawable::shaderProgram = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( Drawable::shaderProgram );

    //init locations
    model_view_loc = glGetUniformLocation( Drawable::shaderProgram, "model_view" );
    projection_loc = glGetUniformLocation( Drawable::shaderProgram, "projection" );

    light_position_loc = glGetUniformLocation( Drawable::shaderProgram, "light_position" );
    eye_position_loc = glGetUniformLocation( Drawable::shaderProgram, "eye_position" );

    ambient_product_loc = glGetUniformLocation( Drawable::shaderProgram, "ambient_product" );
    diffuse_product_loc = glGetUniformLocation( Drawable::shaderProgram, "diffuse_product" );
    specular_product_loc = glGetUniformLocation( Drawable::shaderProgram, "specular_product" );
    shininess_loc = glGetUniformLocation( Drawable::shaderProgram, "shininess" );

    // enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    build_model();
    viewMode = PERSPECTIVE_WITH_FOV;
    reset_view();
    setup_view();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glClearColor(0.45, 0.45, 0.45, 1.0);
}

//-----------------------------------------------------------


void display( void ){

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    for (unsigned int i=0;i<shapes.size();i++){
        //send data of shape[i] to shader
        glUniform4fv(light_position_loc, 1, lights[0]->position);
        glUniform4fv(eye_position_loc, 1, eye);
        glUniform4fv(ambient_product_loc, 1, lights[0]->ambient * shapes[i]->material.ambientK);
        glUniform4fv(diffuse_product_loc, 1, lights[0]->diffuse * shapes[i]->material.diffuseK);
        glUniform4fv(specular_product_loc, 1, lights[0]->specular * shapes[i]->material.specularK);
        glUniform1f(shininess_loc, shapes[i]->material.shininess);
        shapes[i]->render();
    }

    glutSwapBuffers();
}

//----------------------------------------------------------------------------


void jump(int x){
    static  int t=10;
    static bool up=true;
    if(up){
        at.y+=3; eye.y+=3;
        if(--t==0){
            up=false;
            t=10;
        }
    }else{
        at.y-=3; eye.y-=3;
        if(--t==0){
            up=true;
            t=10;
            jumping=false;
            return;
        }
    }
    setup_view();
    glutPostRedisplay();
    glutTimerFunc( 30, jump, 0 );
}
void keyboard( unsigned char key, int x, int y )
{
    float deltaAng = 10/180.0 * M_PI;

    switch( key ) {
        case 033: // Escape Key
        case 'q': case 'Q':
            exit( EXIT_SUCCESS );
            break;
             case 'w':case 'W':
            eye.z-=cos(theta)*3; eye.x+=sin(theta)*3;
            at.z=eye.z-200*cos(theta);
            at.x=eye.x+200*sin(theta);
            break;
        case 's': case'S':
            eye.z+=cos(theta)*3; eye.x-=sin(theta)*3;
            at.z=eye.z-200*cos(theta);
            at.x=eye.x+200*sin(theta);
            break;
        case 'a':case'A':
            eye.z-=sin(theta)*3; eye.x-=cos(theta)*3;
            at.z=eye.z-200*cos(theta);
            at.x=eye.x+200*sin(theta);
            break;
        case 'd':case'D':
            eye.z+=sin(theta)*3; eye.x+=cos(theta)*3;
            at.z=eye.z-200*cos(theta);
            at.x=eye.x+200*sin(theta);
            break;

        case 32://SPACE BAR
            if(!jumping){
                glutTimerFunc( 1, jump, 0 );
                jumping=true;
            }
            break;
       /*
        case 'x': left *= 1.1; right *= 1.1; break;
        case 'X': left *= 0.9; right *= 0.9; break;
        case 'y': bottom *= 1.1; top *= 1.1; break;
        case 'Y': bottom *= 0.9; top *= 0.9; break;
        case 'z': zNear  *= 1.1; zFar *= 1.1; break;
        case 'Z': zNear *= 0.9; zFar *= 0.9; break;
        case 'r': radius *= 1.1; break;
        case 'R': radius *= 0.9; break;

        case 'a': theta += deltaAng; break;
        case 'd': theta -= deltaAng; break;
        case 'w': phi += deltaAng; break;
        case 's': phi -= deltaAng; break;

        case 'p':
                  viewMode = PERSPECTIVE_WITH_FRUSTUM;
                  reset_view();
                  break;
        case 'P':
                  viewMode = PERSPECTIVE_WITH_FOV;
                  reset_view();
                  break;
        case 'o':
                  viewMode = ORTHO;
                  reset_view();
                  break;
        case 'f':
                  fovy+=5;
                  break;
        case 'F':
                  fovy-=5;
                  break;
        case 'v':
                  zoomFactor*=1.1;
                  break;
        case 'V':
                  zoomFactor*= 0.9;
                  break;
        case ' ':  // reset values to their defaults
                  reset_view();
                  */
    }


}

void mouseMotion(int x,int y) {
    int delta_x = x-lastX;
    int delta_y = y-lastY;
    at.y-=delta_y/2.0;
    theta+=delta_x/50.0;
    if(theta>2*M_PI)theta-=2*M_PI;
    std::cout<<theta<<std::endl;
    at.z=eye.z-200*cos(theta);
    at.x=eye.x+200*sin(theta);
    //std::cout<<x<<"   "<<y<<std::endl;
    lastX = x;
    lastY = y;
    if(x<=0){
     lastX=WindowWidth/2;
     glutWarpPointer(WindowWidth/2.0,y);
    }
    if(x>=WindowWidth-1){
     lastX=WindowWidth/2;
     glutWarpPointer(WindowWidth/2.0,y);
    }
}

//----------------------------------------------------------------------------
void cleanup() {
    //destroy shaders
    glUseProgram(0);
    glDeleteProgram(Drawable::shaderProgram);
    //destroy shapes and its vertices
    for (unsigned int i=0;i<shapes.size();i++){
        delete shapes[i];
    }
}
//----------------------------------------------------------------------------
void idle(){
    setup_view();
    glutPostRedisplay();
}
int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA  | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize( WindowWidth, WindowHeight);
    viewMode = PERSPECTIVE_WITH_FOV;
    glutCreateWindow( "lab7: Shading" );

    glewInit();

    init();
    glutSetCursor(GLUT_CURSOR_NONE);
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutIdleFunc(idle);
    glutCloseFunc( cleanup );
    glutPassiveMotionFunc( mouseMotion );
    glLineWidth(1);
    //glEnable(GL_LINE_SMOOTH);
    glutWarpPointer(WindowWidth/2.0,WindowHeight/2.0);
    glutFullScreen();
    glutMainLoop();
    return 0;
}
