void main_vp( in float4   position  : POSITION,
			  in float2   uv	    : TEXCOORD0,             
			  in float4   color     : COLOR0,
		   
              out float4 oPosition : POSITION,
		      out float2 oUv : TEXCOORD0,
			  out float4 oColor : TEXCOORD1)
{
    oPosition = position; 
	oUv = uv;
	oColor = color;
}


float4 main_fp(float2 texCoord : TEXCOORD0,
			   float4 color : TEXCOORD1,
			   sampler2D atlas : register(s0)) : COLOR
{
	return tex2D(atlas, texCoord) * color;	
}