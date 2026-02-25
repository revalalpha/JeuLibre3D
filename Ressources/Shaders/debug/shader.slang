struct VSInput {
    float3 inPosition;
    float3 normal;
    float4 inColor;
    float2 inTexCoord;
};

struct UniformBuffer {
    float4x4 InverseView;
    float4x4 view;
    float4x4 proj;
};


struct PushConstant
{
    float4x4 model;
     float4 color;
}

struct Data
{
    UniformBuffer CamUbo;
}
ParameterBlock<Data> data;

[[vk::push_constant()]]
ConstantBuffer<PushConstant> pushConstantData;

struct VSOutput
{
    float4 pos : SV_Position;
    float4 fragColor;
};

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;

    float4 wsPos = mul(pushConstantData.model, float4(input.inPosition, 1.0f));
    output.pos = mul(data.CamUbo.proj, mul(data.CamUbo.view, wsPos));
    output.fragColor = pushConstantData.color;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    return float4(vertIn.fragColor);
}