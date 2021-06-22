//==========================================================================
// 構造体
//==========================================================================
struct psInput // 入力
{
    float4 pos : SV_POSITION;
    float4 color : COL;
};

float4 main(in psInput input) : SV_TARGET
{
    return input.color;
}