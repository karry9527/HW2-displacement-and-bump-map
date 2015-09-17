#version 120

uniform mat4 ShadowMatrix;

varying vec4 ShadowMapTexCoord;
varying vec3 Normal, LightDirection;

varying vec3 normal, lightDir, eyeDir;
void main()
{
	//phong shading test
	normal = gl_NormalMatrix * gl_Normal;

	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

	lightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
	eyeDir = -vVertex;
	//end
	vec4 Position = gl_ModelViewMatrix * gl_Vertex;
	ShadowMapTexCoord = ShadowMatrix * Position;
	Normal = gl_NormalMatrix * gl_Normal;
	LightDirection = gl_LightSource[0].position.xyz - Position.xyz;
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ProjectionMatrix * Position;
}