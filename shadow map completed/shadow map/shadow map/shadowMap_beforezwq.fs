varying vec3 vertex_position;
varying vec3 vertex_light_vector;
varying vec3 vertex_light_half_vector;
varying vec3 vertex_normal;
varying vec4 color;
 
const vec3 eye_position = vec3(0.0,0.0,0.0);
 
//uniform sampler2DShadow myTexture;
//uniform sampler2D myTexture;
varying vec4 vertex;
 
void main()
{
    vec3 E = normalize(vec3(eye_position - vertex_position));
    vec3 R = normalize(vec3(-reflect(vertex_light_vector, vertex_normal)));
 
    // Defining The Material Colors
    const vec4 AmbientColor = vec4(0.0, 0.0, 0.0, 1.0);
    const vec4 DiffuseColor = vec4(0.0, 0.0, 0.0, 1.0);
    const vec4 SpecularColor = vec4(1.0, 1.0, 1.0, 1.0);
 
    // Calculate the ambient term
    vec4 ambient_color = AmbientColor * gl_LightSource[0].ambient + gl_LightModel.ambient * gl_FrontMaterial.ambient;
 
    // Calculate the diffuse term
    vec4 diffuse_color = color * gl_LightSource[0].diffuse;
    //vec4 diffuse_color = DiffuseColor * gl_LightSource[0].diffuse;
 
    // Calculate the specular value
    //vec4 specular_color = SpecularColor * gl_LightSource[0].specular * pow(max(dot(E, R), 0.0) , 100.0);
    vec4 specular_color = SpecularColor * gl_LightSource[0].specular * pow(max(dot(normalize(vertex_normal), vertex_light_half_vector), 0.0) , 100.0);
 
    // Set the diffuse value (darkness). This is done with a dot product between the normal and the light
    // and the maths behind it is explained in the maths section of the site.
    float diffuse_value = max(dot(normalize(vertex_normal), vertex_light_vector), 0.0);
 
    vec4 texel, clip;
    vec4 scolor =color;
 
    clip = gl_TextureMatrix[0] * vertex;
 
    // Do the projection divide-by-w and shift to the [0,1] range (from the [-1, 1] range).
    clip.xyz = clip.xyz / clip.w * 0.5 + 0.5;
 
    //texel = shadow2D(myTexture, clip.xyz);
 
    if (texel.z < clip.z)
        scolor.rgb *= 0.4;
 
    gl_FragColor = scolor + ambient_color + diffuse_color * diffuse_value + specular_color;
}