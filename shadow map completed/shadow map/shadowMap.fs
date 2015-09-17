#version 120

uniform sampler2D ShadowMap;
varying vec3 normal, lightDir, eyeDir;
varying vec4 ShadowMapTexCoord;
varying vec3 Normal, LightDirection;

void main()
{

	float LightDistance2 = dot(LightDirection, LightDirection);
	float LightDistance = sqrt(LightDistance2);
	float NdotLD = max(dot(normalize(Normal), LightDirection / LightDistance), 0.0);
	
	// NdotLD *= shadow2DProj(ShadowMap, ShadowMapTexCoord).r;
	
	if(ShadowMapTexCoord.w > 0.0)
	{
		vec3 ShadowMapTexCoordProj = ShadowMapTexCoord.xyz / ShadowMapTexCoord.w;
		ShadowMapTexCoordProj.z *= 0.98f;
		
			if(texture2D(ShadowMap, ShadowMapTexCoordProj.xy).z <= ShadowMapTexCoordProj.z )
			{
				NdotLD = 0.3;
			}
	}


	vec4 final_color = 
	(gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + 
	(gl_LightSource[0].ambient * gl_FrontMaterial.ambient);
							
	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0)
	{
		final_color += gl_LightSource[0].diffuse * 
		               gl_FrontMaterial.diffuse * 
					   lambertTerm * NdotLD ;	
		
		vec3 E = normalize(eyeDir);
		vec3 R = reflect(-L, N);
		float specular = pow( max(dot(R, E), 0.0), 
		                 gl_FrontMaterial.shininess );
		final_color += gl_LightSource[0].specular * 
		               gl_FrontMaterial.specular * 
					   specular;	
	}


	

	gl_FragColor = final_color;

}