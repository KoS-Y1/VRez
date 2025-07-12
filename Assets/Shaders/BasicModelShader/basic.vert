#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexcoord;

layout (location = 0) out vec3 vWorldPositon;
layout (location = 1) out vec3 vWorldNormal;
layout (location = 2) out vec2 vTexcoord;


layout (set = 0, binding = 0) uniform UniformViewData
{
    mat4 uView;
    mat4 uProjecton;
};

layout (push_constant) uniform PC_PushConstantData
{
    mat4 PC_inModel;
};

void main()
{

    vWorldPositon = (PC_inModel * vec4(inPosition, 1.0f)).xyz;
    vWorldNormal = (PC_inModel * vec4(inNormal, 1.0f)).xyz;
    vTexcoord = inTexcoord;

//    gl_Position = uView * vec4(vWorldPositon, 1.0f);
    gl_Position = vec4(vWorldPositon, 1.0f);

}
