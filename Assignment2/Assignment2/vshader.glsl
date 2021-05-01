#version 410

in vec4 vPosition, vColor;

uniform mat4 ModelView, Projection;
uniform mat4 vRotation[8];
uniform vec4 cubeColor[8];

uniform int currentCube;
uniform int edge;
uniform int selectFace;
uniform int selectCube;

out vec4 color;

void changePosition() {
    if(selectFace == 1) {
        gl_Position = Projection * ModelView * vPosition;
    } else {
        gl_Position = Projection * ModelView * vRotation[currentCube] * vPosition;
    }
}

void changeColor() {
    if (selectCube == 1) {
        color = cubeColor[currentCube];
    } else if(edge == 1) {
        color = vec4(0.0,0.0,0.0,1.0);
    } else {
        color = vColor;
    }
}

void main()  {
    changePosition();
    changeColor();
}
