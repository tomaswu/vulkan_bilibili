#version 450
//指定着色器语言的版本 glsl .vert顶点着色器

// 声明了一个数组
vec2 positions[3] = vec2[](
    vec2(0.0,-0.5),
    vec2(0.5,0.5),
    vec2(-0.5,0.5)
);

void main(){ 
    // gl_Position是一个内置变量，当c++端调用drawArrays()时，gpu会并行执行三次这个函数，其中gl_VertexIndex从0开始依次递增
    // gl_Position是一个4维的齐次坐标，其中最后一维等于1时，可以视为一个三维的点
    gl_Position = vec4(positions[gl_VertexIndex],0.0,1.0);
}