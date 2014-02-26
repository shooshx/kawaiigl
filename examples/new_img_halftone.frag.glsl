varying vec2 vTexCoord;
uniform sampler2D img; // 0


varying vec3 normal;
varying vec3 lightVec;
varying vec3 eyeVec;

uniform float space; // 17
uniform float colorRatio; //0.63
uniform float r_angle;

#define RATIO 1.0

#define OFFSET vec2(500,500)

float halftone(float v, float angle)
{		
	mat2 rotate = mat2(cos(angle), -sin(angle),
	                   sin(angle), cos(angle));
	mat2 inverse_rotate = mat2(cos(angle), sin(angle),
	                          -sin(angle), cos(angle));


	vec2 fg = vec2(gl_FragCoord) - OFFSET;

	fg = rotate * fg;
	
	vec2 sp = vec2( round(fg.x / space) * space,
				  round(fg.y / (space)) * space);
	sp = inverse_rotate * sp + OFFSET;

	float d = distance(vec2(gl_FragCoord), sp);
	
	float step = d - (v * space * colorRatio) + 1;
	float inten = smoothstep(1.0, 0.0, step);
	return inten;
}

void main(void)
{
	vec4 c = texture2D(img, vTexCoord);
	float r = halftone(c.r, r_angle);
	float g = halftone(c.g, -r_angle + 1);
	float b = halftone(c.b, r_angle/2 + 2);

	gl_FragColor = vec4(r, g, b, 1.0);
}

