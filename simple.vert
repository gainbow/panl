#version 100

#define M_PI 3.1415926535897932384626433832795

// [0.0, 1.0]
uniform float vRotation;
attribute vec4 vPosition;

void main()
{
    float r = 2.0 * M_PI * vRotation;
    mat2 rmat = mat2(
            cos(r), sin(r),
            -sin(r), cos(r)
            );
    gl_Position = vec4(rmat * vPosition.xy, vPosition.zw);
    //gl_Position = vPosition;
}

// vim: set ft=glsl:
