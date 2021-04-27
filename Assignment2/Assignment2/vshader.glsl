#version 410

in vec4 vPosition;
in vec4 vColor;
out vec4 color;

uniform mat4 ModelView;
uniform mat4 Projection;

uniform int edge;
uniform int currentCube;
uniform int selectedCube;
uniform int selectedFace;
uniform mat4 vRotation[27];
uniform vec4 cubeColor[27];

void changePosition() {
    if (selectedFace == 1) {
        gl_Position = Projection * ModelView * vPosition;
    } else {
        gl_Position = Projection * ModelView * vRotation[currentCube] * vPosition;
    }
}

void changeColor() {
    if (selectedCube == 1) {
        color = cubeColor[currentCube];
    } else if (edge == 1) {
        color = vec4(0.0,0.0,0.0,1.0);
    } else {
        color = vColor;
    }
}

void main()  {
    changePosition();
    changeColor();
} 
