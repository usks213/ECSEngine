// �s�N�Z���V�F�[�_
#include "Common.hlsli"
#include "PBR.hlsli"
#include "DeferredCommon.hlsli"

// �p�����[�^
struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};


float4 PS(VS_OUTPUT input) : SV_Target0
{
	float4 rt0 = _RT0.Sample(_MainSampler, input.TexCoord);
	float4 rt1 = _RT1.Sample(_MainSampler, input.TexCoord);
	float depth = _Depth.Sample(_ShadowSampler, input.TexCoord);
	
	//float3 pos = DepthToWorldPosition(depth, input.TexCoord, _mViewInv, _mProjInv);
	float3 pos = _RT2.Sample(_MainSampler, input.TexCoord).xyz;
	float3 color = rt0.rgb;
	float metallic = rt0.w;
	float3 normal = rt1.xyz;
	float roughness = rt1.w;
	
	float4 Out = float4(color, 1);
	
	if (_Flg & LIGHT_FLG)
	{
		float3 L = normalize(-_directionalLit.direction.xyz); // �����̃x�N�g��
		//float3 L = normalize(float3(1,1,1)); // �����ւ̃x�N�g��
		float3 V = normalize(_viewPos.xyz - pos); // ���_�ւ̃x�N�g��
		float3 N = normalize(normal); // �@���x�N�g��
		float3 H = normalize(L + V); // �n�[�t�x�N�g��
		
		float3 tangent = normalize(cross(N, float3(1,1,1)));
		float3 bitangent = normalize(cross(N, tangent));
		tangent = normalize(cross(bitangent, N));
		
		Out.rgb = shade(color, metallic, roughness, N, V, L,
		_directionalLit.color.rgb, _directionalLit.ambient.rgb, _SkyTexture, _SkySampler);
		
		//float3 Diff = _directionalLit.color.rgb.rgb * color * saturate(dot(L, N)); // �g�U�F
		//Diff += _directionalLit.ambient.rgb * color; // ����
		//float3 Spec = float3(1,1,1) *
		//	pow(saturate(dot(N, H)), 100.0f); // ���ʔ��ːF
		
		//Out.rgb = Diff + Spec;
	}
	
	return Out;
}