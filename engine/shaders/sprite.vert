#version 300 es
precision mediump float;

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;

out vec2 v_TexCoord;
out vec4 v_Color;

uniform mat4 u_Projection;
uniform vec2 u_Translation;

void main() {
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;

    gl_Position = u_Projection * vec4(a_Position + u_Translation, 0.0, 1.0);
}
