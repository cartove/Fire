#version 130

in vec3 N;
in vec3 L;
in vec3 E;
in vec2 UV;

uniform vec4 ambient_product;
uniform vec4 diffuse_product;
uniform vec4 specular_product;
uniform vec4 light_position;
uniform vec4 eye_position;

uniform float shininess;
uniform bool will_it_text;
uniform sampler2D myTextureSampler;

out vec3 color;

void main()
{
    if(will_it_text == true){
        gl_FragColor = texture2D( myTextureSampler, UV );
    }else{
    // we need to normalize again (as N is the interpolated value of 3 normals)
    vec3 NN = normalize(N);
    vec3 EE = normalize(E);
    vec3 LL = normalize(L);

    vec4 ambient, diffuse, specular;
    ambient = ambient_product;

    //diffuse calculation
    float Kd = max(dot(LL, NN), 0.0);;
    diffuse = Kd * diffuse_product;

    //specular calulation
    vec3 H   = normalize(LL+NN);
    float Ks = pow(max(dot(N, H), 0.0), shininess);
    specular = Ks * specular_product;

    //final shading
    gl_FragColor = vec4((ambient + diffuse + specular).xyz, 1.0);
    }
}
