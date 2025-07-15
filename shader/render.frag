#version 450

layout(location = 0) in vec3 frag_color; // out是输出变量 in表示输入变量
layout(location = 0) out vec4 out_color; // out是输出变量 in表示输入变量

void main(){
    out_color = vec4(frag_color,1.0);
}