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


float4 PS(VS_OUTPUT input) : SV_Target0
{
	float4 Color = _Color;
	
	//if (_Flg & TEXTURE_FLG)
	//{
	//	Color *= _MainTexture.Sample(_MainSampler, input.TexCoord);
	//}
	
	if (_Flg & LIGHT_FLG)
	{
		float3 L = normalize(-_directionalLit.direction.xyz); // 光源のベクトル
		//float3 L = normalize(float3(1,1,1)); // 光源へのベクトル
		float3 V = normalize(_viewPos.xyz - input.WorldPos); // 視点へのベクトル
		float3 N = normalize(input.Normal); // 法線ベクトル
		float3 H = normalize(L + V); // ハーフベクトル
		
		float3 tangent = normalize(cross(N, float3(1,1,1)));
		float3 bitangent = normalize(cross(N, tangent));
		tangent = normalize(cross(bitangent, N));
		
		float roughness = 0.5f;
		float metallic = 1.0f;
		
		// 拡散反射BRDF
		//float3 dif = NormalizedDisneyDiffuse(Color.rgb, N, L, V, roughness);
		//Color.rgb = dif;
		//// ハーフランバート
		//float PI = 3.14159265359f;
		//float val = max(dot(N, L), 0.0f) * 0.5f + 0.5f;
		//val = val * val; // * (3.0f / (4.0f * PI));
		//Color.rgb = _directionalLit.diffuse.rgb * Diff * val; // 拡散色
        // ランバート
		//float3 dif = Color.rgb * _directionalLit.diffuse.rgb * saturate(dot(L, N)); // 拡散色

		// 
		//float3 spe = CookTorrance(L, N, V, Color.rgb, metallic, roughness);
		//Color.rgb += _directionalLit.specular.rgb *
		//	pow(saturate(dot(N, H)), power) * Diff; // 鏡面反射色
		
		//Color.rgb += _directionalLit.ambient.rgb * Diff; // 環境光
		
		// Diff += g_vKe.rgb * vTd.rgb; // エミッション
		
		//Color.rgb = PBR(L, N, V, _directionalLit.diffuse.rgb, Color.rgb, metallic, roughness,_SkyTexture, _SkySampler);
		//Color.rgb = _SkyTexture.Sample(_SkySampler, SkyMapEquirect(reflect(V,N))).rgb;
		Color.rgb = shade(Color.rgb, metallic, roughness, N, V, L, 
		_directionalLit.diffuse.rgb, _directionalLit.ambient.rgb, _SkyTexture, _SkySampler);
	}
	
	return Color;
}
