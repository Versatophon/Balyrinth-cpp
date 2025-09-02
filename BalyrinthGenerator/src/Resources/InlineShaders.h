#pragma once

//TODO: create a geometry shader to construct node geometry from single point

const char* sViewVertexShSource =
"#version 420\n\
\n\
in vec3 vPos;\n\
//in vec3 vCol;\n\
in int vColIndex;\n\
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
    color = c[vColIndex];//vec3(1,0,0);\n\
}\n";


const char* sViewFragmentShSource =
"#version 420\n\
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
