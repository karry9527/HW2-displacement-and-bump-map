uniform mat4 ShadowMatrix;

varying vec4 ShadowMapTexCoord;
varying vec3 Normal, LightDirection;

void main()
{
	vec4 Position = gl_ModelViewMatrix * gl_Vertex;
	ShadowMapTexCoord = ShadowMatrix * Position;
	Normal = gl_NormalMatrix * gl_Normal;
	LightDirection = gl_LightSource[0].position.xyz - Position.xyz;
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ProjectionMatrix * Position;
}

/*varying vec3 norm;
varying vec4 shadowTexcoord;
varying vec4 diffuse;
void main()
{
 
     
     vec4 Vpos = gl_ModelViewMatrix * gl_Vertex;
     vec3 pos = Vpos.xyz / Vpos.w;

     vec3 lightdir = normalize( vec3(gl_LightSource[0].position) - pos);
     norm = normalize(gl_NormalMatrix * gl_Normal);
     
     float NdotL = max(0.0, dot(lightdir, norm));
     diffuse = gl_LightSource[0].diffuse * NdotL;

      vec4 texcoord = gl_TextureMatrix[0] * gl_ModelViewMatrix * gl_Vertex;
   
     shadowTexcoord = texcoord / texcoord.w;
     shadowTexcoord = 0.5 * shadowTexcoord +0.5;

     gl_FrontColor  = gl_Color;
     gl_Position = ftransform(); 

}*/