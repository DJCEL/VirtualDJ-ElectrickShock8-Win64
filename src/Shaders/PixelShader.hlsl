////////////////////////////////
// File: PixelShader.hlsl
////////////////////////////////

//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------
Texture2D g_Texture2D : register(t0);
SamplerState g_SamplerState : register(s0);
//--------------------------------------------------------------------------------------
// Constant Buffer
//--------------------------------------------------------------------------------------
cbuffer PS_CONSTANTBUFFER : register(b0)
{
    int g_FX_Select;
    int g_FX_Activate;
    float g_FX_Time;
};
//--------------------------------------------------------------------------------------
// Input structure
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 Position : SV_Position;
	float4 Color : COLOR0;
	float2 TexCoord : TEXCOORD0;
};
//--------------------------------------------------------------------------------------
// Output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 Color : SV_TARGET;
};
//--------------------------------------------------------------------------------------
// Additional Functions
//--------------------------------------------------------------------------------------
float SQ(float x)
{
    return x * x;
}
//--------------------------------------------------------------------------------------
float4 Negative(float4 color)
{
    color.rgb = 1 - color.rgb;
    
    return color;
}
//--------------------------------------------------------------------------------------
float Triangle(float2 texcoord)
{
    float x = texcoord.x;
    float y = texcoord.y;
    
    float value = 0.0f;
    float f1_x = -2.0f * x + 1.0f;
    float f2_x = 2.0f * x - 1.0f;

    if (x <= 0.5)
    {
        if (y >= f1_x) 
            value = 1.0f;
    }
    else if (x >= 0.5)
    {
        if (y >= f2_x)
            value = 1.0f;
    }
        
    return value;
}

//--------------------------------------------------------------------------------------
float Circle(float2 texcoord, float radius)
{
    float value = 0.0f;
    float x = texcoord.x;
    float y = texcoord.y;
    
    // translation to center the circle in (0,0)
    x -= 0.5f;
    y -= 0.5f;
    
    float f1_x = sqrt(SQ(radius) - SQ(x)); // half circle up
    float f2_x = f1_x * (-1.0f); // half circle down
    
    if (y > 0)
    {
        if (y <= f1_x)
            value = 1.0f;
    }
    else if (y <= 0)
    {
        if (y >= f2_x)
            value = 1.0f;
    }
    
    return value;
}
//--------------------------------------------------------------------------------------
float Bands(float x, int nb, bool inverted)
{
    float2 val = x * nb;
    uint2 band = uint2(val);
    bool isEvenBand = (bool) ((band % 2) == 0);
    return (isEvenBand ^ inverted) ? 1.0f : 0.0f;
}
//--------------------------------------------------------------------------------------
float4 FX_Negative(int FX_On, float4 color)
{
    if (FX_On == 1)
    {
        color = Negative(color);
    }
    
    return color;
}
//--------------------------------------------------------------------------------------
float4 FX_Triangle(int FX_On, float2 texcoord)
{
    float4 color = float4(0.0, 0.0, 0.0, 1.0);
    
    if (FX_On == 1)
    {
        float val = Triangle(texcoord);
        color = float4(val, val, val, 1.0);
    }
   
    return color;
}
//--------------------------------------------------------------------------------------
float4 FX_Circle(int FX_On, float2 texcoord, float radius)
{
    float4 color = float4(0.0, 0.0, 0.0, 1.0);
    
    if (FX_On == 1)
    {
        float val = Circle(texcoord, radius);
        color = float4(val, val, val, 1.0);
    }
   
    return color;
}
//--------------------------------------------------------------------------------------
float4 FX_Bands(int FX_On, float t, int nb, bool inverted)
{
    float4 color = float4(0.0, 0.0, 0.0, 1.0);
    
    if (FX_On == 1)
    {
        float val = Bands(t, nb, inverted);
        color = float4(val, val, val, 1.0);
    }
   
    return color;
}
//--------------------------------------------------------------------------------------
float4 FX_BiBands(int FX_On, float2 texcoord, int nb, bool inverted)
{
    float x = texcoord.x;
    float y = texcoord.y;
    
    float4 color = float4(0.0, 0.0, 0.0, 1.0);
    
    if (FX_On == 1)
    {
        float val1 = Bands(x, nb, inverted);
        float val2 = Bands(y, nb, inverted);
        float val = (val1 && val2) ? 1.0f : 0.0f;
        color = float4(val, val, val, 1.0);
    }
   
    return color;
}
//--------------------------------------------------------------------------------------
float4 FX_BiBandsInv(int FX_On, float2 texcoord, int nb, bool inverted)
{
    float x = texcoord.x;
    float y = texcoord.y;
    
    float4 color = float4(0.0, 0.0, 0.0, 1.0);
    
    if (FX_On == 1)
    {
        float val1 = Bands(x, nb, inverted);
        float val2 = Bands(y, nb, inverted);
        float val = (val1 || val2) ? 1.0f : 0.0f;
        color = float4(val, val, val, 1.0);
    }
   
    return color;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT ps_main(PS_INPUT input)
{
    PS_OUTPUT output;
    float2 texcoord = input.TexCoord;
    float4 color = g_Texture2D.Sample(g_SamplerState, texcoord);
    int FX_Select = g_FX_Select;
    int FX_Activate = g_FX_Activate;
    bool FX_Inverted = (g_FX_Time == 1.0) ? true : false;

    if (FX_Select == 1)
    {
        color = FX_Negative(FX_Activate, color);
    }
    else if (FX_Select == 2)
    {
        color = FX_Bands(FX_Activate, texcoord.x, 2, FX_Inverted);
    }
    else if (FX_Select == 3)
    {
        color = FX_Bands(FX_Activate, texcoord.x, 4, FX_Inverted);
    }
    else if (FX_Select == 4)
    {
        color = FX_Bands(FX_Activate, texcoord.x, 8, FX_Inverted);
    }
    else if (FX_Select == 5)
    {
        color = FX_Bands(FX_Activate, texcoord.x, 16, FX_Inverted);
    }
    else if (FX_Select == 6)
    {
        color = FX_Bands(FX_Activate, texcoord.x, 32, FX_Inverted);
    }
    else if (FX_Select == 7)
    {
        color = FX_Bands(FX_Activate, texcoord.y, 32, FX_Inverted);
    }
    else if (FX_Select == 8)
    {
        color = FX_Bands(FX_Activate, texcoord.y, 16, FX_Inverted);
    }
    else if (FX_Select == 9)
    {
        color = FX_Bands(FX_Activate, texcoord.y, 8, FX_Inverted);
    }
    else if (FX_Select == 10)
    {
        color = FX_Bands(FX_Activate, texcoord.y, 4, FX_Inverted);
    }
    else if (FX_Select == 11)
    {
        color = FX_Bands(FX_Activate, texcoord.y, 2, FX_Inverted);
    }
    else if (FX_Select == 12)
    {
        color = FX_BiBands(FX_Activate, texcoord, 2, FX_Inverted);
    }
    else if (FX_Select == 13)
    {
        color = FX_BiBands(FX_Activate, texcoord, 4, FX_Inverted);
    }
    else if (FX_Select == 14)
    {
        color = FX_BiBands(FX_Activate, texcoord, 8, FX_Inverted);
    }
    else if (FX_Select == 15)
    {
        color = FX_BiBands(FX_Activate, texcoord, 16, FX_Inverted);
    }
    else if (FX_Select == 16)
    {
        color = FX_BiBands(FX_Activate, texcoord, 32, FX_Inverted);
    }
    else if (FX_Select == 17)
    {
        color = FX_BiBandsInv(FX_Activate, texcoord, 32, FX_Inverted);
    }
    else if (FX_Select == 18)
    {
        color = FX_BiBandsInv(FX_Activate, texcoord, 16, FX_Inverted);
    }
    else if (FX_Select == 19)
    {
        color = FX_BiBandsInv(FX_Activate, texcoord, 8, FX_Inverted);
    }
    else if (FX_Select == 20)
    {
        color = FX_BiBandsInv(FX_Activate, texcoord, 4, FX_Inverted);
    }
    else if (FX_Select == 21)
    {
        color = FX_BiBandsInv(FX_Activate, texcoord, 2, FX_Inverted);
    }
    else if (FX_Select == 22)
    {
        color = FX_Triangle(FX_Activate, texcoord);
    }
    else if (FX_Select == 23)
    {
        color = FX_Circle(FX_Activate, texcoord, 0.4f);
    }
    
    output.Color = color * input.Color;
    return output;
}
