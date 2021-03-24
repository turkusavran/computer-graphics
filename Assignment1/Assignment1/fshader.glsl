#version 410

in vec4 color;
in vec3 fN;
out vec4 fragColor;

void main()
{
    vec3 N = normalize(fN);
    fragColor = color;
}

