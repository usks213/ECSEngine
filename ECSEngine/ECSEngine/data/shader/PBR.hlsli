// PBR

#define _PI (3.14159265358979323846f)



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

float3 PBR(float3 L, float3 N, float3 V, float3 LitColor, float3 albedo, float metalness, float roughness)
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
	//float3 irradiance = irradianceTexture.Sample(defaultSampler, N).rgb;

	//F = fresnelSchlick(F0, NdotV);
	//kd = lerp(1.0 - F, 0.0, metalness);
	//float3 diffuseIBL = kd * albedo * irradiance;

	//uint specularTextureLevels = querySpecularTextureLevels();
	//float3 specularIrradiance = specularTexture.SampleLevel(defaultSampler, Lr, roughness * specularTextureLevels).rgb;

	//float2 specularBRDF = specularBRDF_LUT.Sample(spBRDF_Sampler, float2(NdotV, roughness)).rg;

	//float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

	//ambientLighting = diffuseIBL + specularIBL;
	
	
	return directLighting + ambientLighting;
}