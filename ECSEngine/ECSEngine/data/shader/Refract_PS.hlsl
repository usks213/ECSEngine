// ピクセルシェーダ
#include "Common.hlsli"
#include "PBR.hlsli"
//#include "CookTorrance.hlsli"


// パラメータ
struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float3	Normal	: NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float3 WorldPos : POSITION;
};

// PBRパラメータ
cbuffer PBRParam : register(b0)
{
	float _metallic = 0.0f;
	float _roughness = 0.0f;
	float _refraction = 1.33f;
	float _distance = 1.2f;
}


float4 PS(VS_OUTPUT input) : SV_Target0
{
	float4 Color = _Color;
	
	if (_Flg & TEXTURE_FLG)
	{
		Color *= _MainTexture.Sample(_MainSampler, input.TexCoord);
	}
	
	//if (_Flg & LIGHT_FLG)
	//{
	//	float3 L = normalize(-_directionalLit.direction.xyz); // 光源のベクトル
	//	//float3 L = normalize(float3(1,1,1)); // 光源へのベクトル
	//	float3 V = normalize(_viewPos.xyz - input.WorldPos); // 視点へのベクトル
	//	float3 N = normalize(input.Normal); // 法線ベクトル
	//	float3 H = normalize(L + V); // ハーフベクトル
		
	//	float3 tangent = normalize(cross(N, float3(1,1,1)));
	//	float3 bitangent = normalize(cross(N, tangent));
	//	tangent = normalize(cross(bitangent, N));
		
	//	float3 Diff = _directionalLit.color.rgb.rgb * Color.rgb * saturate(dot(L, N)); // 拡散色
	//	Diff += _directionalLit.ambient.rgb * Color.rgb; // 環境光
	//	float3 Spec = float3(1, 1, 1) *
	//		pow(saturate(dot(N, H)), 100.0f); // 鏡面反射色
		
	//	Color.rgb = Diff + Spec;
	//}
	
	float3 pos = input.WorldPos.xyz;
	float3 normal = normalize(input.Normal);
	float3 albedo = Color.rgb;
	float metallic = _metallic;
	float roughness = _roughness;
	float ao = 1.0f;
	
	if (_Flg & LIGHT_FLG)
	{
		float3 N = normalize(normal); // 法線ベクトル
		float3 V = normalize(_mainCamera._viewPos.xyz - pos); // 視点へのベクトル
		float3 R = reflect(-V, N);
		
		float3 F0 = Fresnel0(albedo, metallic);
		float3 Lit = float3(0, 0, 0);
		
		// DirectionalLight
		{
			float3 L = normalize(_directionalLit.direction.xyz); // 光源のベクトル
			float3 H = normalize(L + V); // ハーフベクトル
			float3 F = Fresnel(F0, V, H);
		
			float3 diffuse = DiffuseBRDF(albedo, metallic, N, L, F);
			float3 specular = SpeculerBRDF(roughness, N, V, L, H, F);
			
			Lit += (diffuse + specular) * DirectionalLightColor(_directionalLit);
		}
		// PointLight
		int i = 0;
		for (i = 0; i < _pointLightNum; ++i)
		{
			float3 L = normalize(_PointLights[i].position - pos); // 光源のベクトル
			float3 H = normalize(L + V); // ハーフベクトル
			float3 F = Fresnel(F0, V, H);
		
			float3 diffuse = DiffuseBRDF(albedo, metallic, N, L, F);
			float3 specular = SpeculerBRDF(roughness, N, V, L, H, F);
			
			Lit += (diffuse + specular) * PointLightColor(_PointLights[i], pos);
		}
		// SpotLight
		for (i = 0; i < _spotLightNum; ++i)
		{
			float3 L = normalize(_SpotLights[i].position - pos); // 光源のベクトル
			float3 H = normalize(L + V); // ハーフベクトル
			float3 F = Fresnel(F0, V, H);
		
			float3 diffuse = DiffuseBRDF(albedo, metallic, N, L, F);
			float3 specular = SpeculerBRDF(roughness, N, V, L, H, F);
		
			Lit += (diffuse + specular) * SpotLightColor(_SpotLights[i], pos);
		}
		// AmbientLight
		float3 ambient = AmbientBRDF(albedo, metallic, roughness, ao, N, V, R, F0,
			_directionalLit.ambient.rgb, _SkyTexture, _SkySampler);
		
		Color.rgb = ambient + Lit;
	}
	
	// 屈折
	float3 N = normalize(normal); // 法線ベクトル
	float3 V = normalize(_mainCamera._viewPos.xyz - pos); // 視点へのベクトル
	float3 F0 = Fresnel0(albedo, metallic);
	
	float3 reflactDir = refract(V, N, 1.0f / _refraction);
	float3 refractPos = pos + reflactDir * _distance;
	float4 refractScreenPos = mul(float4(refractPos, 1.0f), _mainCamera._mView);
	refractScreenPos = mul(refractScreenPos, _mainCamera._mProj);
	float2 screenUV = (refractScreenPos.xy / refractScreenPos.w) * 0.5f + 0.5f;
	screenUV.y = 1.0f - screenUV.y;
	
	float texWidth, texHeight, texMips;
	_GrabTexture.GetDimensions(0, texWidth, texHeight, texMips);
	float mip = texMips * roughness;
	float4 refractCol = _GrabTexture.SampleLevel(_SkySampler, screenUV, mip);
	
	float f = fresnelSchlickRoughness(dot(V, N), F0, _roughness);
	float3 c = (1.0 - f) * refractCol.rgb * _Color.rgb + f * Color.rgb;
	
	return float4(c, 1.0f);
}
