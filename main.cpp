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


#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>

const GLfloat WindowWidth = 1366;
const GLfloat WindowHeight = 768;

// SHADER Variables

GLuint model_view_loc;
GLuint projection_loc;
GLboolean will_it_text=false;
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
bool birdcrap=false;
// current variables
float theta = defaultTheta;
float phi = defaultPhi;
int lastX=WindowWidth/2,lastY=WindowHeight/2;

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
std::vector<FilledCube *> walls;
//----------------------------------------------------------------------------
sf::Music jump_sound;
sf::Music hewon;
pair<sf::Music,sf::Music> music;
//----------------------------------------------------
void build_model(){
    jump_sound.OpenFromFile("jump.ogg");
    hewon.OpenFromFile("castleend.ogg");
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
    FilledCube* cube1 = new FilledCube(vec4(0,-2,0,1),100000,.1,100000);
    cube1->material = Material(
                color4(0.24725,0.1995,0.0745,1.0),
                color4(0.1,0.1,0.1,1.0),
                color4(0.1,0.1,0.266065,1.0),
                51.2f
                );
    shapes.push_back(cube1);
    fstream in("walls.txt");
    int n;
    in>>n;
    for(int i=0;i<n;i++){
        double cx,cz,w,d;
        in>>cx>>cz>>w>>d;
         FilledCube *temp=new FilledCube(vec4(cx,50,cz,1),w,100,d);
        //TODO material
        walls.push_back(temp);
    }

}

//-----------------------------------------------------------

void setup_view() {
    //eye and at are points
    static bool first=true;
    if(first){
        eye.x = 624;
        eye.y = 40;
        eye.z = 435;
        eye.w = 1.0;
        at.x=570;
        at.y=40;
        at.z=438;
        at.w=1;
        first=false;
        theta=-0.5*M_PI;
    }else if(!birdcrap){
        eye.y = 40;
        at.z=eye.z-200*cos(theta);
        at.x=eye.x+200*sin(theta);
    }else if(birdcrap){
        eye.y=400;
        at.x=eye.x-5;
        at.z=eye.z-5;
        at.y=20;
    }

    vec4 up( 0.0, 1.0, 0.0, 0.0 );
    mat4 mv= LookAt((eye), (at) ,(up));
    mat4 mp =Perspective(fovy, WindowWidth/WindowHeight, zNear, zFar*3);

    glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, mv);
    glUniformMatrix4fv(projection_loc, 1, GL_TRUE, mp);
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
    will_it_text = glGetUniformLocation( Drawable::shaderProgram, "will_it_text" );
    // enable depth test
    glEnable(GL_DEPTH_TEST);
    build_model();
    setup_view();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.88, 1, 1.0);}

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
        for (unsigned int i=0;i<walls.size();i++){
            //send data of shape[i] to shader
            glUniform4fv(light_position_loc, 1, lights[0]->position);
            glUniform4fv(eye_position_loc, 1, eye);
            glUniform4fv(ambient_product_loc, 1, lights[0]->ambient * walls[i]->material.ambientK);
            glUniform4fv(diffuse_product_loc, 1, lights[0]->diffuse * walls[i]->material.diffuseK);
            glUniform4fv(specular_product_loc, 1, lights[0]->specular * walls[i]->material.specularK);
            glUniform1f(shininess_loc, walls[i]->material.shininess);
            walls[i]->render();
        }
    if(birdcrap){
            FilledSphere* s=new FilledSphere(vec4(eye.x-5,25,eye.z-5,1),20,3);
            s->render();
            delete s;
    }
    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void jump(int x){
    static bool jumpwalalessa=true;
    if(jumpwalalessa){
    jump_sound.Play();
    jumpwalalessa=false;
    }
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
            jumpwalalessa=true;
            return;
        }
    }
    setup_view();
    glutPostRedisplay();
    glutTimerFunc( 30, jump, 0 );
}

void collide(double oldx,double oldz){
	for(int i=0;i<walls.size();i++){
        if(eye.x<=walls[i]->xmax+20&&eye.x>=walls[i]->xmin-10&&eye.z<=walls[i]->zmax+20&&eye.z>=walls[i]->zmin-10){
			eye.x=oldx;
			eye.z=oldz;
			break;
		}

	}
}


void keyboard( unsigned char key, int x, int y )
{
    double oldx=eye.x;
    double oldz=eye.z;
    if(birdcrap) theta=0;
    switch( key ) {
    case 033: // Escape Key
    case 'q': case 'Q':
        hewon.Play();
        while(hewon.GetStatus()==hewon.Playing){
            music.first.Stop();
            music.second.Stop();
            jump_sound.Stop();
        }
        exit(EXIT_SUCCESS);

        break;
    case 'w':case 'W':
        eye.z-=cos(theta)*3; eye.x+=sin(theta)*3;
        collide(oldx,oldz);
        break;
    case 's': case'S':
        eye.z+=cos(theta)*3; eye.x-=sin(theta)*3;
        collide(oldx,oldz);
        break;
    case 'a':case'A':
        eye.z-=sin(theta)*3; eye.x-=cos(theta)*3;
        collide(oldx,oldz);
        break;
    case 'd':case'D':
        eye.z+=sin(theta)*3; eye.x+=cos(theta)*3;
        collide(oldx,oldz);
        break;

    case 32://SPACE BAR
        if(!jumping){
            glutTimerFunc( 1, jump, 0 );
            jumping=true;
        }
        break;
    case 'n':
        std::cout << "looking from " << eye <<std::endl<<"at "<< at<<std::endl;
        break;
    case 'b':
        birdcrap=!birdcrap;
        if(birdcrap){
            music.first.Pause();
            music.second.Play();
        }else{
            music.second.Pause();
            music.first.Play();
        }
    }
}

void mouseMotion(int x,int y) {
    int delta_x = x-lastX;
    int delta_y = y-lastY;
    at.y-=delta_y/2.0;
    theta+=delta_x/50.0;
    if(theta>2*M_PI)theta-=2*M_PI;
    at.z=eye.z-200*cos(theta);
    at.x=eye.x+200*sin(theta);
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
    if(birdcrap) theta=0;
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
    if(eye.x>0&&eye.z>500&&eye.x<100&&eye.z<510){
        hewon.Play();
        while(hewon.GetStatus()==hewon.Playing){
            music.first.Stop();
            music.second.Stop();
            jump_sound.Stop();
        }
        exit( EXIT_SUCCESS );
    }
    setup_view();
    glutPostRedisplay();
}
int main( int argc, char **argv )
{
    music.first.OpenFromFile("overworld.ogg");
    music.second.OpenFromFile("underwater.ogg");
    music.first.SetLoop(true);
    music.second.SetLoop(true);
    music.first.Play();
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA  | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize( WindowWidth, WindowHeight);
    glutCreateWindow( "Project" );
    glewInit();
    init();

    glutSetCursor(GLUT_CURSOR_NONE);
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutIdleFunc(idle);
    glutCloseFunc( cleanup );
    glutPassiveMotionFunc( mouseMotion );
    glutWarpPointer(WindowWidth/2.0,WindowHeight/2.0);
    glutFullScreen();
    glutMainLoop();
    return 0;
}
