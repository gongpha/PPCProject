static const char* SOLID_VSHADER = "#version 330 core\n\
layout(location = 0) in vec3 aPos;\
layout(location = 1) in vec4 color;\
out vec4 aColor;\
uniform mat4 projection;\
void main()\
{\
	gl_Position = projection * vec4(aPos, 1.0f);\
	aColor = color;\
}";


static const char* SOLID_FSHADER = "#version 330 core\n\
out vec4 FragColor;\
\
in vec4 aColor;\
\
void main()\
{\
	FragColor = aColor;\
}";

static const char* FONT_VSHADER = "#version 330 core\n\
layout(location = 0) in vec4 vertex;\
out vec2 TexCoords;\
uniform mat4 projection;\
void main()\
{\
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\
    TexCoords = vertex.zw;\
}\
";

static const char* FONT_FSHADER = "#version 330 core\n\
in vec2 TexCoords;\
out vec4 color;\
uniform sampler2D text;\
uniform vec3 textColor;\
void main()\
{\
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\
    color = vec4(textColor, 1.0) * sampled;\
}";

static const char* TEXTURE_VSHADER = "#version 330 core\n\
layout(location = 0) in vec2 vertex;\
out vec2 TexCoords;\
uniform mat4 model;\
uniform mat4 projection;\
void main()\
{\
    TexCoords = vertex.xy;\
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\
}";

static const char* TEXTURE_FSHADER = "#version 330 core\n\
in vec2 TexCoords;\
out vec4 color;\
\
uniform sampler2D tex;\
uniform vec4 modulate;\
\
void main()\
{\
    color = texture(tex, TexCoords) * modulate;\
}";

/////////////////////////////////////////////////////////////////////

static const char* WORLD_VSHADER = "#version 330 core\n\
layout(location = 0) in vec3 vertex;\
layout(location = 1) in vec3 normal;\
layout(location = 2) in vec2 uv_texture;\
layout(location = 3) in vec2 uv_lightmap;\
out vec2 TexCoords;\
out vec2 LightCoords;\
out vec3 Normal;\
out vec3 FragPos;\
out vec3 eyeview;\
uniform mat4 model;\
uniform mat4 projection;\
uniform mat4 view;\
\
void main()\
{\
	TexCoords = uv_texture;\
	LightCoords = uv_lightmap;\
	Normal = mat3(transpose(inverse(model))) * normal;\
	FragPos = vec3(model * vec4(vertex, 1.0));\
	gl_Position = projection * view * vec4(FragPos, 1.0);\
	vec3 cameraPos = vec3(inverse(view)[3]);\
	vec3 dir = vertex - cameraPos;\
	eyeview = dir;\
}";

static const char* WORLD_FSHADER = "#version 330 core\n\
in vec2 TexCoords;\
in vec2 LightCoords;\
in vec3 Normal;\
in vec3 FragPos;\
in vec3 eyeview;\
out vec4 FragColor;\
uniform sampler2D tex;\
uniform sampler2D lightmap;\
uniform sampler2D tex2;\
uniform bool is_sky;\
uniform mat4 view;\
uniform float time;\
\
void main()\
{\
	if (is_sky)\
	{\
		vec3 view3 = normalize(eyeview);\
		vec2 uv = ((view3 * (256.0f / view3.y)).xz + (time * vec2(-8.0f, 8.0f))) / 512.0f;\
		vec3 t2 = texture(tex2, uv * 2.0f).xyz;\
		uv += (time * 0.05f * vec2(-1.0f, 1.0f));\
		vec4 t1 = texture(tex, uv * 2.0f);\
		vec3 ALBEDO = mix(t2, t1.xyz, sign(t1.r + t1.g + t1.b));\
		FragColor = vec4(ALBEDO, 1.0f);\
		return;\
	}\
	/*vec3 result = texture(lightmap, LightCoords).r * texture(tex, TexCoords).rgb;*/\
	vec3 result = texture(tex, TexCoords).xyz * texture(lightmap, LightCoords).r * 1.0f;\
	FragColor = vec4(result, 1.0);\
}";

/*

void fragment()
{
	vec3 view = -(vec4(VIEW, 1.0f) * VIEW_MATRIX).xyz;
	vec2 uv = ((view * (256.0f / view.y)).xz + (TIME * vec2(-8.0f, 8.0f))) / 512.0f;
	vec3 t2 = texture(skytex_bg, uv * 2.0f).xyz;
	uv += (TIME * 0.05f * vec2(-1.0f, 1.0f));
	vec4 t1 = texture(skytex_fg, uv * 2.0f);
	ALBEDO = mix(t2, t1.xyz, t1.a);
}


*/