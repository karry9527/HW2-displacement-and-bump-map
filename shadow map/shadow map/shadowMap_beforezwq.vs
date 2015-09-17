varying vec3 vertex_position;
varying vec3 vertex_light_vector;
varying vec3 vertex_light_half_vector;
varying vec3 vertex_normal;
varying vec4 color;
 
varying vec4 vertex;
 
void main()
{
    // Calculate the normal value for this vertex, in world coordinates (multiply by gl_NormalMatrix)
    vertex_normal = normalize(gl_NormalMatrix * gl_Normal);
    vertex_position = vec3(gl_ModelViewMatrix * gl_Vertex);
 
    // Calculate the light position for this vertex
    vec3 light_position = vec3(0, 0, 0);
    vertex_light_vector = normalize(light_position.xyz - vertex_position.xyz);
 
    // Calculate the light's half vector
    vertex_light_half_vector = normalize(gl_LightSource[0].halfVector.xyz);
 
    // Set the front color to the color passed through with glColor*f
    color = gl_Color;
 
    vertex = gl_Vertex;
 
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_Position = ftransform();
}