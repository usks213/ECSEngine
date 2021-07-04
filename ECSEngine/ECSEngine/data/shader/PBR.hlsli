// PBR

#define _PI (3.14159265358979323846f)


float3 lerp3(float3 x, float3 y, float s)
{
	return x + (y - x) * s;
}

float3 fresnelSchlick(float3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float ndfGGX(float NdotH, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (_PI * denom * denom);
}

float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaSchlickGGX(float NdotL, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(NdotL, k) * gaSchlickG1(NdotV, k);
}

float BechmannDistribution(float NdotH, float m)
{
	float d2 = NdotH * NdotH;
	float m2 = m * m;
	return exp((d2 - 1.0) / (d2 * m2)) / (m2 * d2 * d2);
}

uint querySpecularTextureLevels()
{
	uint width, height, levels;
	//specularTexture.GetDimensions(0, width, height, levels);
	return levels;
}

// à¯êîÇ…ÇÕê≥ãKâªÇ≥ÇÍÇΩîΩéÀÉxÉNÉgÉãÇë„ì¸
float2 SkyMapEquirect2(float3 reflectionVector)
{
	float PI = 3.14159265359f;
	float phi = acos(-reflectionVector.y);
	float theta = atan2(-1.0f * reflectionVector.x, reflectionVector.z) + PI;
	return float2(theta / (PI * 2.0f), phi / PI);
}

float3 PBR(float3 L, float3 N, float3 V, float3 LitColor, float3 albedo, float metalness, float roughness,
			Texture2D skyTex, SamplerState skySampler)
{
	float3 H = normalize(L + V);
	float NdotV = max(0.0, dot(N, V));
	float NdotL = max(0.0, dot(N, L));
	float NdotH = max(0.0, dot(N, H));
	float3 reflectionVec = 2.0 * NdotV * N - V;
	
	//NdotL = NdotL * 0.5f + 0.5f;
	//NdotL *= NdotL;
	
	float3 F0 = lerp(0.04f, albedo, metalness);
	
	// FDG
	float3 F = fresnelSchlick(F0, max(0.0, dot(H, V)));
	//float D = ndfGGX(NdotH, roughness);
	float D = BechmannDistribution(NdotH, roughness);
	//float D = ndfGGX(NdotH, roughness);
	float G = gaSchlickGGX(NdotL, NdotV, roughness);
	//float t = 2.0 * NdotH / dot(V, H);
	//float G = min(1.0, min(t * NdotV, t * NdotL));
	
	// Diffuse
	float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);
	float3 diffuseBRDF = kd * albedo;
	// Specular
	float3 specularBRDF = (F * D * G) / max(0.00001, 4.0 * NdotL * NdotV);
	
	float3 directLighting = (diffuseBRDF + specularBRDF) * LitColor * NdotL;
	
	// IBL
	float3 ambientLighting = float3(0, 0, 0);
	float3 irradiance = skyTex.Sample(skySampler, SkyMapEquirect2(reflect(V,N))).rgb;

	F0 = lerp(0.04f, irradiance, metalness);
	F = fresnelSchlick(F0, NdotV);
	kd = lerp(1.0 - F, 0.0, roughness);
	float3 diffuseIBL = kd * albedo * irradiance;

	//uint specularTextureLevels = querySpecularTextureLevels();
	//float3 specularIrradiance = specularTexture.SampleLevel(defaultSampler, Lr, roughness * specularTextureLevels).rgb;

	//float2 specularBRDF = specularBRDF_LUT.Sample(spBRDF_Sampler, float2(NdotV, roughness)).rg;

	//float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
	specularBRDF = (F * D * G) / max(0.00001, 4.0 * NdotL * NdotV);
	float3 specularIBL = specularBRDF;

	ambientLighting = diffuseIBL + specularIBL;
	
	
	return directLighting + ambientLighting;
}

float3 fresnelSchlick_roughness(float3 F0, float cosTheta, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 EnvBRDFApprox(float3 SpecularColor, float Roughness, float NoV)
{
	float4 c0 = float4(-1, -0.0275f, -0.572f, 0.022f);
	float4 c1 = float4(1, 0.0425f, 1.04f, -0.04f);
	float4 r = Roughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28f * NoV)) * r.x + r.y;
	float2 AB = float2(-1.04f, 1.04f) * a004 + r.zw;
	return SpecularColor * AB.x + AB.y;
}

float so(float NoV, float ao, float roughness)
{
	return clamp(pow(NoV + ao, exp2(-16.0 * roughness - 1.0)) - 1.0 + ao, 0.0, 1.0);
}


float3 shade(float3 albedo, float metalness, float roughness, float3 N, float3 V, float3 L, 
float3 LitDif, float3 LitAmb, Texture2D AmbiTex, SamplerState AmbiSampler)
{
	float3 H = normalize(L + V);
	float NdotV = max(0.0f, dot(N, V));
	float NdotL = max(0.0f, dot(N, L));
	float NdotH = max(0.0f, dot(N, H));
		
	float3 F0 = lerp3(float3(0.04f, 0.04f, 0.04f), albedo, metalness);

	float3 F = fresnelSchlick_roughness(F0, max(0.0f, dot(H, L)), roughness);
	float D = ndfGGX(NdotH, roughness);
	float G = gaSchlickGGX(NdotL, NdotV, roughness);

	float3 kd = lerp3(float3(1.0f, 1.0f, 1.0f) - F, float3(0.0f, 0.0f, 0.0f), metalness);

	float3 diffuseBRDF = kd * albedo / _PI;
	float3 specularBRDF = (F * D * G) / max(0.0001f, 4.0f * NdotL * NdotV);

	float3 direct = (diffuseBRDF + specularBRDF) * LitDif * NdotL;
	
	// Ambient
	float difAmbi = kd * albedo * LitAmb;
	
	float3 ref = EnvBRDFApprox(F0, roughness, NdotV) * (1.0f - roughness);
	//float3 speAmbi = AmbiTex.Sample(AmbiSampler, SkyMapEquirect2(reflect(V, N))).rgb * ref;
	float mip = 12 * roughness;
	float3 speAmbi = AmbiTex.SampleLevel(AmbiSampler, SkyMapEquirect2(reflect(V, N)), mip).rgb * ref;
	float3 ambi = difAmbi + speAmbi;
	
	return direct + ambi;
}