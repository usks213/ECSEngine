// BRDF

#define _PI (3.14159265358979323846f)

float sqr(float x)
{
	return x * x;
}

float SchlickFresnel(float u)
{
	float m = clamp(1 - u, 0, 1);
	float m2 = m * m;
	return m2 * m2 * m; // pow(m,5)
}

float GTR1(float NdotH, float a)
{
	if (a >= 1)
		return 1 / _PI;
	float a2 = a * a;
	float t = 1 + (a2 - 1) * NdotH * NdotH;
	return (a2 - 1) / (_PI * log(a2) * t);
}

float GTR2(float NdotH, float a)
{
	float a2 = a * a;
	float t = 1 + (a2 - 1) * NdotH * NdotH;
	return a2 / (_PI * t * t);
}

float GTR2_aniso(float NdotH, float HdotX, float HdotY, float ax, float ay)
{
	return 1 / (_PI * ax * ay * sqr(sqr(HdotX / ax) + sqr(HdotY / ay) + NdotH * NdotH));
}

float smithG_GGX(float NdotV, float alphaG)
{
	float a = alphaG * alphaG;
	float b = NdotV * NdotV;
	return 1 / (NdotV + sqrt(a + b - a * b));
}

float smithG_GGX_aniso(float NdotV, float VdotX, float VdotY, float ax, float ay)
{
	return 1 / (NdotV + sqrt(sqr(VdotX * ax) + sqr(VdotY * ay) + sqr(NdotV)));
}

float3 mon2lin(float3 x)
{
	return float3(pow(x.x, 2.2f), pow(x.y, 2.2f), pow(x.z, 2.2f));
}

// X:tangent, Y:bitangent
float3 BRDF(float3 L, float3 V, float3 N, float3 X, float3 Y)
{
	// íËêî
	float3 baseColor	= float3(82, 67, 16);
	float metallic		= 1;
	float subsurface	= 0.0f;
	float specular		= 0.5f;
	float roughness		= 0.0f;
	float specularTint	= 0.0f;
	float anisotropic	= 0.0f;
	float sheen			= 0.0f;
	float sheenTint		= 0.5f;
	float clearcoat		= 0.0f;
	float clearcoatGloss = 1.0f;
	
	float NdotL = dot(N, L);
	float NdotV = dot(N, V);
	if (NdotL < 0 || NdotV < 0)
		return float3(0, 0, 0);

	float3 H = normalize(L + V);
	float NdotH = dot(N, H);
	float LdotH = dot(L, H);

	float3 Cdlin = mon2lin(baseColor);
	float Cdlum = .3 * Cdlin.x + .6 * Cdlin.y + .1 * Cdlin.z; // luminance approx.

	float3 Ctint = Cdlum > 0 ? Cdlin / Cdlum : float3(1, 1, 1); // normalize lum. to isolate hue+sat
	float3 Cspec0 = lerp(specular * 0.08f * lerp(float3(1, 1, 1), Ctint, specularTint), Cdlin, metallic);
	float3 Csheen = lerp(float3(1, 1, 1), Ctint, sheenTint);

    // Diffuse fresnel - go from 1 at normal incidence to .5 at grazing
    // and lerp in diffuse retro-reflection based on roughness
	float FL = SchlickFresnel(NdotL), FV = SchlickFresnel(NdotV);
	float Fd90 = 0.5f + 2 * LdotH * LdotH * roughness;
	float Fd = lerp(1.0f, Fd90, FL) * lerp(1.0f, Fd90, FV);

    // Based on Hanrahan-Krueger brdf approximation of isotropic bssrdf
    // 1.25 scale is used to (roughly) preserve albedo
    // Fss90 used to "flatten" retroreflection based on roughness
	float Fss90 = LdotH * LdotH * roughness;
	float Fss = lerp(1.0f, Fss90, FL) * lerp(1.0f, Fss90, FV);
	float ss = 1.25f * (Fss * (1 / (NdotL + NdotV) - 0.5f) + 0.5f);

    // specular
	float aspect = sqrt(1 - anisotropic * 0.9f);
	float ax = max(0.001f, sqr(roughness) / aspect);
	float ay = max(0.001f, sqr(roughness) * aspect);
	float Ds = GTR2_aniso(NdotH, dot(H, X), dot(H, Y), ax, ay);
	float FH = SchlickFresnel(LdotH);
	float3 Fs = lerp(Cspec0, float3(1, 1, 1), FH);
	float Gs;
	Gs = smithG_GGX_aniso(NdotL, dot(L, X), dot(L, Y), ax, ay);
	Gs *= smithG_GGX_aniso(NdotV, dot(V, X), dot(V, Y), ax, ay);

    // sheen
	float3 Fsheen = FH * sheen * Csheen;

    // clearcoat (ior = 1.5 -> F0 = 0.04)
	float Dr = GTR1(NdotH, lerp(0.1f, 0.001f, clearcoatGloss));
	float Fr = lerp(0.04f, 1.0f, FH);
	float Gr = smithG_GGX(NdotL, 0.25f) * smithG_GGX(NdotV, 0.25f);

	return ((1 / _PI) * lerp(Fd, ss, subsurface) * Cdlin + Fsheen)
        * (1 - metallic)
        + Gs * Fs * Ds + 0.25f * clearcoat * Gr * Fr * Dr;
}

