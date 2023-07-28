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