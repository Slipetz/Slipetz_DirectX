// mfeeney@fanshawec.ca
// The lighting functions are (modified) from Frank Luna's DirectX 10 book (chapter 6)

// This is used to specify what the material property of the light and objects are
struct MaterialInfo
{
    float4 diffuse;
	float4 ambient;
    float4 spec;
};

struct LightDesc
{
	float4 pos;			// Only used with point and spot
	float4 dir;			// Only used with directional(parallel) and spot
	MaterialInfo material;	// ambient, diffuse, spec
	float4 attenuation;
	// x = power, y = range, z = light type, w = not used
	// 0 = parallel, 1 = point, >1 = spot
	float4 lightPowerRangeType;	
};



// v = material description of the surface
// L = lighting description, including material
// pos = world space vertex position
// normal = world space vertex normal
// eyePos = camera (eye) world position
float4 PointLight(MaterialInfo material, LightDesc light, float4 pos, float4 normal, float4 eyePos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	normal = normalize( normal );

	// The vector from the surface to the light.
	float3 lightPos = light.pos.xyz;
	float3 vertexPos = pos.xyz;
	float3 lightVec = lightPos - vertexPos;
		//
	// The distance from surface to light.
	float d = length(lightVec);

	if( d > light.lightPowerRangeType.y )			// y element is range of spot or point light
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);		// If it's too far, set colour to black.
		// Note: since it's black, we don't need to to any more calculations.
	}

	// Normalize the light vector.
	lightVec /= d;			//normalize( lightVec );

	// Add the ambient light term.
	// 	litColor += v.diffuse.xyz * L.material.ambient.xyz;
	litColor += material.diffuse.rgb * light.material.ambient.rgb;			// Same as above: xyzw same as rgba

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal.xyz);
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(material.spec.a, 1.0f);
		float3 toEye     = normalize( eyePos.xyz - vertexPos.xyz );
		float3 R         = reflect( -lightVec, normal.xyz );
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);

		// diffuse and specular terms
		litColor += diffuseFactor * material.diffuse.rgb * light.material.diffuse.rgb;
		litColor += specFactor * material.spec.rgb * light.material.spec.rgb;
	}
	
	// attenuate
	litColor =  litColor / dot( light.attenuation.rgb, float3(1.0f, d, d*d));

	return float4( litColor.xyz, 1.0f );
}

//float3 ParallelLight( vSurfaceInfo, Light L, float3 eyePos)
float4 ParallelLight( MaterialInfo material, LightDesc light, float4 pos, float4 normal, float4 eyePos )
{
	normal = normalize( normal );

	float3 litColor = float3(0.0f, 0.0f, 0.0f);
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -light.dir.xyz;
	
	// Add the ambient term.
	litColor += material.diffuse.rgb * light.material.ambient.rgb;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot( lightVec, normal.xyz );

	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(material.spec.a, 1.0f);
		float3 toEye     = normalize( eyePos.xyz - pos.xyz );
		float3 R         = reflect( lightVec, normal.xyz );
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);
					
		

		// diffuse and specular terms
		litColor += diffuseFactor * material.diffuse.rgb * light.material.diffuse.rgb;
		litColor += specFactor * material.spec.rgb * light.material.spec.rgb;
	}
	
	return float4( litColor.rgb, 1.0f );
}

//float3 Spotlight(SurfaceInfo v, Light L, float3 eyePos)
float4 Spotlight( MaterialInfo material, LightDesc light, float4 pos, float4 normal, float4 eyePos )
{
	normal = normalize( normal );
	
	float4 litColor = PointLight(material, light, pos, normal, eyePos);
			
	// The vector from the surface to the light.
	float3 lightVec = normalize(light.pos.xyz - pos.xyz);
	
	float s = pow(max(dot(-lightVec, light.dir.xyz), 0.0f), light.lightPowerRangeType.x);
	
	// Scale color by spotlight factor.
	return litColor*s;
}


//Based on Frank Luna's book.
//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a point light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputePointLight(MaterialInfo mat, LightDesc L, float3 pos, float3 normal, float3 toEye,
				   out float4 ambient, out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.pos.xyz - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if( d > L.lightPowerRangeType.y )
		return;
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Ambient term.
	ambient = mat.ambient * L.material.ambient;
	//ambient = mat.Ambient * L.Ambient;	

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.spec.w);
					
		diffuse = diffuseFactor * mat.diffuse * L.material.diffuse;//L.Diffuse;
		spec    = specFactor * mat.spec * L.material.spec;
	}

	// Attenuate
	float att = 1.0f / dot(L.attenuation.xyz, float3(1.0f, d, d*d));

	diffuse *= att;
	spec    *= att;
}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeDirectionalLight(MaterialInfo mat, LightDesc L, 
                             float3 normal, float3 toEye,
					         out float4 ambient,
						     out float4 diffuse,
						     out float4 spec)
{

	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.dir.xyz;
	
	// Add ambient term.
	ambient = mat.ambient * L.material.ambient;	

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot( lightVec, normal );

	// Flatten to avoid dynamic branching.
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.spec.w);
					
		diffuse = diffuseFactor * mat.diffuse * L.material.diffuse;
		spec    = specFactor * mat.spec * L.material.spec;
	}
}


//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a spotlight.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeSpotLight(MaterialInfo mat, LightDesc L, float3 pos, float3 normal, float3 toEye,
				  out float4 ambient, out float4 diffuse, out float4 spec)
{

	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.pos.xyz - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if( d > L.lightPowerRangeType.x )
		return;
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Ambient term.
	ambient = mat.ambient * L.material.ambient;	

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.spec.w);
					
		diffuse = diffuseFactor * mat.diffuse * L.material.diffuse;
		spec    = specFactor * mat.spec * L.material.spec;
	}
	
	// Scale by spotlight factor and attenuate.
	float spot = pow(max(dot(-lightVec, L.dir.xyz), 0.0f), L.lightPowerRangeType.y);

	// Scale by spotlight factor and attenuate.
	float att = spot / dot(L.attenuation.xyz, float3(1.0f, d, d*d));

	ambient *= spot;
	diffuse *= att;
	spec    *= att;
}