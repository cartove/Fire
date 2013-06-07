#version 130
in vec4 vPosition;
in vec3 vNormal;

//view matrices
uniform mat4 model_view;
uniform mat4 projection;

//shading properties
uniform vec4 light_position;
uniform vec4 eye_position;


uniform float shininess;

// send the normals to each vertex so the fragment shader get the interpolated value of the normals
out vec3 N;
//same for K and E vectors
out vec3 L;
out vec3 E;

void main()
{
    //vertex position after projection
    gl_Position = projection * model_view * vPosition;
    gl_Position = gl_Position / gl_Position.w;

    N = normalize(vNormal.xyz);
    L = normalize(light_position.xyz - vPosition.xyz);
    E = normalize(eye_position - vPosition).xyz;
}
