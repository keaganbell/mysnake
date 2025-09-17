#version 300 es
precision mediump float;

in vec4 color;
in vec2 uv;
in float texid;

uniform sampler2D tex;

out vec4 frag_color;

void main() {
    if (texid == 0.0) {
        frag_color = color;
    }
    else {
        frag_color = texture(tex, uv);
    }
}
