#include "drawable.h"

class Mesh: public Drawable{
public:
    vector <vec4> shisnapoints;
    vector<GLuint> elements;
    Mesh(){
    }

    void calculateNormals(){
        vector<int> nb_seen;
        normals=new vec4[pointsNum];
        nb_seen.resize(pointsNum, 0);
        for (unsigned int i = 0; i < elements.size(); i+=3) {
            GLuint ia = elements[i];
            GLuint ib = elements[i+1];
            GLuint ic = elements[i+2];
            vec3 normal = normalize(cross(
                                        (points[ib]) - (points[ia]),
                                        (points[ic]) - (points[ia])));

            int v[3];  v[0] = ia;  v[1] = ib;  v[2] = ic;
            for (int j = 0; j < 3; j++) {
                GLuint cur_v = v[j];
                nb_seen[cur_v]++;
                if (nb_seen[cur_v] == 1) {
                    normals[cur_v] = normal;
                } else {
                    // average
                    normals[cur_v].x = normals[cur_v].x * (1.0 - 1.0/nb_seen[cur_v]) + normal.x * 1.0/nb_seen[cur_v];
                    normals[cur_v].y = normals[cur_v].y * (1.0 - 1.0/nb_seen[cur_v]) + normal.y * 1.0/nb_seen[cur_v];
                    normals[cur_v].z = normals[cur_v].z * (1.0 - 1.0/nb_seen[cur_v]) + normal.z * 1.0/nb_seen[cur_v];
                    normals[cur_v] = normalize(normals[cur_v]);
                }
            }
        }

        normalsCalculated=true;
    }

    Mesh(const char * filename,const vec4&center){
        load_obj(filename);
        normalsCalculated = false;
        for (int i=0;i<pointsNum;i++)
            points[i] += center;
        this->init();
    }


    void render() {
        glBindVertexArray( vao );
        glDrawArrays(GL_TRIANGLES,0,pointsNum);
    }
private:
    void load_obj(const char* filename) {
        ifstream in(filename, ios::in);
        if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }
        string line;
        while (getline(in, line)) {
            if (line.substr(0,2) == "v ") {
                istringstream s(line.substr(2));
                vec4 v; s>>v.x>>v.y >>v.z; v.w = 0.0;
                shisnapoints.push_back(v);
            }  else if (line.substr(0,2) == "f ") {
                istringstream s(line.substr(2));
                GLuint a,b,c;
                s>>a>>b>>c;
                a--; b--; c--;
                elements.push_back(a); elements.push_back(b); elements.push_back(c);
            }
        }
        points=new vec4[pointsNum];
        for(int i=0;i<shisnapoints.size();i++){
                    points[i]=shisnapoints[i]*50;

                    points[i].w=1;
                }
        pointsNum=shisnapoints.size();
    }



};
