// ���C�g�V�F�[�_�[

struct DirectionalLightData
{
	float4 ambient; // ����
	float4 diffuse; // �g�U��
	float4 specular; // ���ˌ�
	
	float4 direction;	// ����
};

struct PointLightData
{
	float4 ambient; // ����
	float4 diffuse; // �g�U��
	float4 specular; // ���ˌ�
	
	float3 position;	// �ʒu
	float range;		// �͈�
	float4 attenuation; // ����
};

struct SpotLightData
{
	float4 ambient; // ����
	float4 diffuse; // �g�U��
	float4 specular; // ���ˌ�
	
	float3 position;	// �ʒu
	float range;		// �͈�
	float4 attenuation; // ����
	float3 direction;	// ����
	float spot;			// �X�|�b�g
};