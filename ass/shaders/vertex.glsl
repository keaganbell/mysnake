#version 300 es
precision mediump float;

in vec3 _position;
in vec4 _color;
in vec2 _uv;
in float _texid;

uniform mat4 proj;

out vec4 color;
out vec2 uv;
out float texid;

void main() {
    color = _color;
    uv = _uv;
    texid = _texid;
    gl_Position = proj * vec4(_position, 1.0);
}
