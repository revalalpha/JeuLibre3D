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

struct Transform
{
    float4x4 model;
}

struct Data
{
    UniformBuffer CamUbo;
    Sampler2D texture;
}
ParameterBlock<Data> data;

[[vk::push_constant()]]
ConstantBuffer<Transform> pushConstantData;

struct VSOutput
{
    float4 pos : SV_Position;
    float3 normal;
    float3 worldspacePosition;
    float3 eyePos;
    float2 fragTexCoord;
    float4 fragColor;
};

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;

    float4 wsPos = mul(pushConstantData.model, float4(input.inPosition, 1.0f));
    output.pos = mul(data.CamUbo.proj, mul(data.CamUbo.view, wsPos));
    output.worldspacePosition = wsPos.xyz;
    output.eyePos = mul(data.CamUbo.InverseView, float4(0.0f.xxx, 1.0f)).xyz;
    output.fragTexCoord = input.inTexCoord;
    output.fragColor = input.inColor;
    output.normal =  mul(pushConstantData.model, float4(input.normal, 0.0f)).xyz;
    return output;
}



[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {

    // light postition temporar
    float3 lightPos = float3(0.0f, 10.0f, 0.0f);
    // light color
    float3 lightColor = float3(1.0f, 1.0f, 1.0f);
    float3 L = normalize(lightPos - vertIn.worldspacePosition.xyz);
    float3 N = normalize(vertIn.normal);
    float3 V = normalize(vertIn.eyePos - vertIn.worldspacePosition.xyz);
    float3 H = normalize(L + V);

    // base color
    float3 Kd = data.texture.Sample(vertIn.fragTexCoord).rgb;
    // specular color
    float3 Ks = float3(1.0, 0.0, 0.0);
    // shiny
    float Ns = 100.0f;
    // Intensity
    float Intensity = 1000.0f;
    // distance betwen light and wsPos
    float r = length(lightPos - vertIn.worldspacePosition.xyz);

    float3 lightDir = normalize(float3(0.0f, -1.0f, 0.0f));
    float spotAngle = dot(normalize(-lightDir), L);
    float openAngle = radians(5.0f);
    float cosInner = cos(openAngle);
    float cosOuter = cos(openAngle + radians(5.0f));
    float spotAttenuation = smoothstep(cosOuter, cosInner, spotAngle);

    // beause its a shpere
    float spotNormalization = 1.0f / (2.0f * 3.14159265f * (1.0f - cosOuter));
    float attenuation = ((Intensity * spotNormalization )/ pow(r, 2.0f));
    float diffuse = max(dot(N, L), 0.0);
    float specular = ((Ns + 2.0f) / (2.0f * 3.14159265f)) * pow(max(dot(N, H), 0.0f), Ns);

    float3 Fr = Kd + Ks * specular;
    float Ej = diffuse * attenuation * spotAttenuation;

    float3 result = Fr * Ej * lightColor;
    return float4(result, 1.0f);
}