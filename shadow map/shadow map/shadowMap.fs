uniform sampler2DShadow shadowmap;

varying vec3 norm;
varying vec4 shadowTexcoord;
varying vec4 diffuse;


void main()
{
	const float epsilon = 0.4;

	float factor = 1.0;
	float depth = shadow2DProj(shadowmap, shadowTexcoord).r + epsilon;
	depth = clamp(depth, 0.0, 1.0);
        if(depth != 1.0)factor = 0.5;


	if(shadowTexcoord.x >= 0.0 && shadowTexcoord.y >= 0.0 
	&& shadowTexcoord.x <= 1.0 && shadowTexcoord.y <= 1.0 )
   	{
		gl_FragColor = vec4(gl_Color.rgb* diffuse.rgb * factor, 1.0);
	}
	else
	{
		gl_FragColor = vec4(gl_Color.rgb* diffuse.rgb, 1.0);
	}
}