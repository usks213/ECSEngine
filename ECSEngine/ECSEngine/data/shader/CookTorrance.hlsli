// CookTorrance

//uniform float4 diffuse; // ägéUîΩéÀåWêî
//uniform float4 specular; // ãæñ îΩéÀåWêî
//uniform float4 ambient; // ä¬ã´åı
//uniform float microfacet; // ñ ÇÃëeÇ≥
 
// Beckmanï™ïz
float BechmannDistribution(float d, float m)
{
	float d2 = d * d;
	float m2 = m * m;
	return exp((d2 - 1.0) / (d2 * m2)) / (m2 * d2 * d2);
}
 

//float DistributionGGX(float3 N, float3 H, float a)
//{
//	float PI = 3.14159265f;
	
//	float a2 = a * a;

//	float NdotH = max(dot(N, H), 0.0);

//	float NdotH2 = NdotH * NdotH;

//	float nom = a2;

//	float denom = (NdotH2 * (a2 - 1.0) + 1.0);

//	denom = PI * denom * denom;

//	return nom / denom;
//}

// ÉtÉåÉlÉãçÄ
float Fresnel(float c, float f0)
{
	float sf = sqrt(f0);
	float N = (1.0 + sf) / (1.0 - sf);
	float g = sqrt(N * N + c * c - 1.0);
	float ga = (c * (g + c) - 1.0) * (c * (g + c) - 1.0);
	float gb = (c * (g - c) + 1.0) * (c * (g - c) + 1.0);
	return (g - c) * (g - c) / (2.0 * (g + c) + (g + c)) * (1.0 + ga / gb);
}
 
float SchlickFresnel(float u, float f0, float f90)
{
	return f0 + (f90 - f0) * pow(1.0 - u, 5.0);
}

float3 CookTorrance(float3 L, float3 N, float3 V, float3 baseColor, float metallic, float roughness)
{
	float3 H = normalize(L + V);
	float HdotN = saturate(dot(H, N));
	float LdotN = (dot(L, N));
	float LdotH = saturate(dot(L, H));
	float VdotN = saturate(dot(V, N));
	
	float PI = 3.14159265f;
	float f0 = 0.04f;
	float3 speColor = lerp(f0, baseColor, metallic);
	float ref = max(max(speColor.x, speColor.y), speColor.z);
	float ref90 = clamp(ref * 25.0f, 0.0f, 1.0f);
	
    // Cook-Torrance
	float f = SchlickFresnel(1.0, ref, ref90);
	//float3 f = float3(Fresnel(LdotH, speColor.x), Fresnel(LdotH, speColor.y), Fresnel(LdotH, speColor.z));
	float d = BechmannDistribution(HdotN, roughness);
	float t = 2.0 * HdotN / dot(V, H);
	float g = min(1.0, min(t * VdotN, t * LdotN));
	float m = 4.0 * VdotN * LdotN;
	float3 spe = max(f * d * g / m, 0.0);
	
	return spe;
}


float3 NormalizedDisneyDiffuse(float3 albedo, float3 N, float3 L, float3 V, float roughness)
{
	float PI = 3.14159265358979323846f;
	
	float3 H = normalize(L + V);
	float dotLH = saturate(dot(L, H));
	float dotNL = saturate(dot(N, L));
	float dotNV = saturate(dot(N, V));
	
	float energyBias = lerp(0.0, 0.5, roughness);
	float energyFactor = lerp(1.0, 1.0 / 1.51, roughness);
	float Fd90 = energyBias + 2.0 * dotLH * dotLH * roughness;
	float FL = SchlickFresnel(1.0, Fd90, dotNL);
	float FV = SchlickFresnel(1.0, Fd90, dotNV);
	return (albedo * FL * FV * energyFactor) / PI;
}