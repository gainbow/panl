#version 100

// cheap fake marble noise, needs gradients

precision mediump float;

vec2 u_resolution = vec2(20,20);

// 2D random, https://thebookofshaders.com/11/
float rand2(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main()
{
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;

    vec2 pos = vec2(st*10.0);

    vec3 color = vec3(0.2);
    float rnd = rand2(pos);
    if(rnd < 0.07) {
        color = vec3(0.2 * rnd);
    } else if(rnd < 0.15) {
        color = vec3(0.1 * rnd);
    } else if(rand2(vec2(pos.x + 10.0, pos.y)) < 0.005) {
        color = vec3(0.6) - vec3(0.1 * rnd);
    }

    gl_FragColor = vec4(color,1.0);
}

// vim: set ft=glsl:
