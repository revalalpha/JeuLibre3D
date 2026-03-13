struct VSInput {
    float2 inPosition;
    float2 inTexCoord;
};

struct Transform
{
    float3x3 model;
}

struct PushData
{
    float4 inColor;
    float3 raw1;
    float width;
    float3 raw2;
    float height;
    float4 raw3;
}

[[vk::binding(0, 0)]]
ConstantBuffer<Sampler2D> texture;

[[vk::push_constant()]]
ConstantBuffer<PushData> pushConstantData;

struct VSOutput
{
    float4 pos : SV_Position;
    float2 fragTexCoord;
    float4 fragColor;
};


[shader("vertex")]
VSOutput vertMain(VSInput input) {  
    VSOutput output;

    float3x3 ortho = float3x3(
        2.0 / pushConstantData.width, 0.0, -1.0, 
        0.0, 2.0 / pushConstantData.height, -1.0, 
        0.0, 0.0, 1.0
);
    float3x3 transform3x3 = float3x3(pushConstantData.raw1.xyz, pushConstantData.raw2.xyz, pushConstantData.raw3.xyz);
    float3 wpos = mul(ortho,mul(transform3x3,float3(input.inPosition, 1.0f)));
    output.pos = float4(wpos,1.0f);
    output.fragColor = pushConstantData.inColor;
    output.fragTexCoord = input.inTexCoord;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    float4 color = texture.Sample(vertIn.fragTexCoord).rgba;
    return color*vertIn.fragColor;
}