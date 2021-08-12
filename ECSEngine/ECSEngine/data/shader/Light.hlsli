// ライトシェーダー

struct DirectionalLightData
{
	float4 color;		// 色
	float4 ambient;		// 環境光
	float4 direction;	// 方向
};

struct PointLightData
{
	float4 color;		// 色
	float3 position;	// 位置
	float range;		// 範囲
	float4 attenuation; // 減衰
};

struct SpotLightData
{
	float4 color;		// 色
	float3 position;	// 位置
	float range;		// 範囲
	float4 attenuation; // 減衰
	float3 direction;	// 方向
	float spot;			// スポット
};