#pragma once

//TODO: create a geometry shader to construct node geometry from single point



#ifdef __EMSCRIPTEN__
#define SHADER_VERSION "#version 300 es\nprecision mediump float;\nprecision mediump int;\n"
#define COLOR_INDEX_INPUT "in float vColIndex;"
#else
#define SHADER_VERSION "#version 420"
#define COLOR_INDEX_INPUT "in int vColIndex;"
#endif

const char* sViewVertexShSource =
SHADER_VERSION"\n\
\n\
in vec3 vPos;\n"
COLOR_INDEX_INPUT"\n\
\n\
uniform uint model_index;\n\
\n\
layout(std140) uniform matrices\n\
{\n\
    mat4 p;\n\
    mat4 v;\n\
};\n\
\n\
layout(std140) uniform models\n\
{\n\
    mat4 m[256];\n\
};\n\
layout(std140) uniform colors\n\
{\n\
    vec4 c[256];\n\
};\n\
out vec4 color;\n\
void main()\n\
{\n\
    gl_Position = p * v * m[model_index] * vec4(vPos, 1);\n\
    color = c[int(vColIndex)];\n\
}\n";

const char* sViewVertexSingleColorShSource =
SHADER_VERSION"\n\
\n\
in vec3 vPos;\n\
\n\
uniform uint color_index;\n\
uniform uint model_index;\n\
\n\
layout(std140) uniform matrices\n\
{\n\
    mat4 p;\n\
    mat4 v;\n\
};\n\
\n\
layout(std140) uniform models\n\
{\n\
    mat4 m[256];\n\
};\n\
layout(std140) uniform colors\n\
{\n\
    vec4 c[256];\n\
};\n\
out vec4 color;\n\
void main()\n\
{\n\
    gl_Position = p * v * m[model_index] * vec4(vPos, 1);\n\
    color = c[color_index];\n\
}\n";

const char* sViewFragmentShSource =
SHADER_VERSION"\n\
\n\
/*uniform uint materialIndex = 0;\n\
\n\
struct Material\n\
{\n\
    vec4 ambient;\n\
    vec4 diffuse;\n\
    vec4 specular;\n\
    vec4 special;\n\
};\n\
\n\
layout(std140) uniform materials\n\
{\n\
    Material material[512];\n\
};*/\n\
\n\
in vec4 color;\n\
out vec4 outColor;\n\
\n\
void main()\n\
{\n\
    //if (!gl_FrontFacing)\n\
    //{\n\
    //    discard;\n\
    //}\n\
\n\
    outColor = color;//vec4(vec3(1,0,0),1);\n\
}";
