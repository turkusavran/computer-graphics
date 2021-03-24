#version 410

in vec4 vPosition;
in vec3 vNormal;
out vec4 color;
out vec3 fN;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 ColorChange;

void main()
{
    fN = vNormal;
    gl_Position = Projection * ModelView * vPosition;
    color = ColorChange;
}
