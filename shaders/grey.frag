uniform sampler2D texture;

void main()
{
	float intensity = dot(vec3(0.2989, 0.5870, 0.1140), gl_Color.rgb);
	vec4 greyweights = vec4(intensity, intensity, intensity, gl_Color.a);
	
	gl_FragColor = mix(gl_Color, greyweights, 0.95);
}
