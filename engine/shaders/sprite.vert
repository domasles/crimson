#version 300 es
precision mediump float;

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;

out vec2 v_TexCoord;
out vec4 v_Color;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main() {
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;

    gl_Position = u_Projection * u_View * vec4(a_Position, 0.0, 1.0);
}
