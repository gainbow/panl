#version 100

attribute vec4 vPosition;

varying vec3 vPos;
void main()
{
    vPos = vPosition.xyz;
    gl_Position = vPosition;
}

// vim: set ft=glsl:
