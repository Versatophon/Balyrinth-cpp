#pragma once

enum class BufferUsage
{
    Stream,//modified once and used at most few times
    Static,//modified once and used many times
    Dynamic,//modified repeatedly and used many times
};

enum class GeometryType
{
    Points = 0x0000,//GL_POINTS
    Lines = 0x0001,//GL_LINES
    LineLoop = 0x0002,//GL_LINE_LOOP
    LineStrip = 0x0003,//GL_LINE_STRIP
    Triangles = 0x0004,//GL_TRIANGLES
    TriangleStrip = 0x0005,//GL_TRIANGLE_STRIP
    TriangleFan = 0x0006,//GL_TRIANGLE_FAN
    Quads = 0x0007,//GL_QUADS
    QuadStrip = 0x0008,//GL_QUAD_STRIP
    Polygon = 0x0009,//GL_POLYGON
};

enum class ShaderType
{
    VertexShader,
    FragmentShader,
    GeometryShader
};

enum class AttributeType
{
    Float,
    Integer,
    Double
};
