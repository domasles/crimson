#version 300 es
precision mediump float;

in vec2 v_TexCoord;
in vec4 v_Color;

out vec4 FragColor;

uniform sampler2D u_Texture;
uniform int u_UseTexture;

void main() {
    vec4 texColor = u_UseTexture == 1 ? texture(u_Texture, v_TexCoord) : vec4(1.0);
    FragColor = texColor * v_Color;
}
