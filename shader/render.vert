#version 450

vec2 position[3] = vec2[](
    vec2(0.0,-0.5),
    vec2(0.5,0.5),
    vec2(-0.5,0.5)
);

void main(){
    gl_Position = vec4(position[gl_VertexIndex],0.0,1.0); // gl_Position是一个内键的变量，表示顶点位置，传递给下一个环节
}