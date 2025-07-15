#version 450

layout(location = 0) out vec3 frag_color;
layout(location = 0) in vec2 position;

vec3 colors[3] = vec3[](
    vec3(1.0,0.0,0.0),
    vec3(0.0,1.0,0.0),
    vec3(0.0,0.0,1.0)
);

void main(){
    gl_Position = vec4(position,0.0,1.0); // gl_Position是一个内键的变量，表示顶点位置，传递给下一个环节
    frag_color = colors[gl_VertexIndex];
}