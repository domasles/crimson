#version 300 es
precision mediump float;

in vec2 v_TexCoord;
in vec4 v_Color;

out vec4 FragColor;

uniform sampler2D u_Texture;
uniform bool u_UseTexture;

void main() {
    if (u_UseTexture) { FragColor = texture(u_Texture, v_TexCoord) * v_Color; }
    else { FragColor = v_Color; }
}
