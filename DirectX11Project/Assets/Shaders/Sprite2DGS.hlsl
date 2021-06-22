cbuffer MatrixBuffer : register(b0)
{
    column_major matrix matproj;
    column_major matrix matview;
    column_major matrix matworld;
}

struct gsInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};

struct psInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL; // �ʖ@��
};

[maxvertexcount(3)]
void main(triangle gsInput input[3], inout TriangleStream<psInput> tristream)
{
    psInput output = (psInput) 0;
	// �ʖ@���̎擾
    float3 edge1 = input[1].pos.xyz - input[0].pos.xyz;
    float3 edge2 = input[2].pos.xyz - input[0].pos.xyz;
    output.normal = normalize(cross(edge1, edge2));
    for (int i = 0; i < 3; ++i)
    {
        output.pos = mul(input[i].pos, matworld); // ���[�J�����W���烏�[���h���W�֕ϊ�
        output.pos = mul(output.pos, matview); // ���[���h���W����r���[���W(�J�������W)�֕ϊ�
        output.pos = mul(output.pos, matproj); // �r���[���W����v���W�F�N�V�������W�֕ϊ�
        output.color = input[i].color; // �F���͓��͒l�����̂܂ܓn��
        output.tex = input[i].tex; // �F���͓��͒l�����̂܂ܓn��
        tristream.Append(output); // �O�p�`�|���S���P�ʂŃf�[�^���i�[����
    }
    tristream.RestartStrip();
}