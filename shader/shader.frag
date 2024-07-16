#version 450 
//指定着色器语言的版本 glsl .frag片段着色器

//layout out·表示是输入变量
//定义一个输出变量，类型是vec4，名字是outColor，layout(location = 0)表示这个变量在渲染管线中的位置是0
layout(location = 0) out vec4 outColor;

void main(){
    outColor = vec4(1.0,0.0,0.0,1.0);
}