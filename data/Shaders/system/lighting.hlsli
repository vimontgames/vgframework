#pragma once

#include "system/lightsBuffer.hlsli"
#include "system/shared_consts.hlsli"
#include "system/pbr.hlsli"

//--------------------------------------------------------------------------------------
float3 heatmapGradient(float x, float _green, float _yellow, float _red)
{
    float3 color = 0;
	if (x <= _green)
		return float3(0,x/_green,0);
	else if (x <= _yellow)
		return lerp(float3(0,1,0), float3(1,1,0), saturate((x-_green)/(_yellow-_green)));
	else 
		return lerp(float3(1,1,0), float3(1,0,0), saturate((x-_yellow)/(_red-_yellow)));
	return color;
}

//--------------------------------------------------------------------------------------
struct LightingResult
{
	float3 envDiffuse;
	float3 envSpecular;
    float3 directLightDiffuse;
    float3 directLightSpecular;

	void addLightContribution(float3 Lo, float cosLo, float _cosLi, float3 Lr, float3 F0, float3 Li, float3 Lradiance, float3 _worldNormal, float roughness, float metalness)
	{
		// Half-vector between Li and Lo.
		float3 Lh = normalize(Li + Lo);

        // Calculate angles between surface normal and various light vectors.
		float cosLi = _cosLi;
		float cosLh = max(0.0f, dot(_worldNormal, Lh));

        // Calculate Fresnel term for direct lighting. 
		float3 F = fresnelSchlick(F0, max(0.0f, dot(Lh, Lo)));

		// Calculate normal distribution for specular BRDF.
		float D = ndfGGX(cosLh, roughness);
		//float D = GeometrySchlickGGX(cosLi, roughness);

		// Calculate geometric attenuation for specular BRDF.
		float G = gaSchlickGGX(cosLi, cosLo, roughness);
		//float G = GeometrySmith(roughness, cosLi, cosLo);

		// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
		// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
		// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
		float3 kd = lerp(float3(1.0f, 1.0f, 1.0f) - F, float3(0.0f, 0.0f, 0.0f), metalness);

		// Lambert diffuse BRDF.
		// We don't scale by 1/PI for lighting & material units to be more convenient.
		// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
		float3 diffuseBRDF = kd;

		// Cook-Torrance specular microfacet BRDF.
		float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		directLightDiffuse += diffuseBRDF * Lradiance * cosLi;
		directLightSpecular += specularBRDF * Lradiance * cosLi;
	}
};

//--------------------------------------------------------------------------------------
float getRangeAttenuation(float _dist, float _maxRange)
{
#if 1
	// inverse square dist + smooth max range
	float ratio = _dist / _maxRange;
	return max(min(1.0f - pow(ratio, 4), 1.0f), 0.0f) / pow(_dist, 2);
#else
	// inverse square dist 
	return 1.0f / ( _dist * _dist );
#endif
}

float SampleDirectionalShadowMap(Texture2D _shadowMap, float3 _shadowUV, float _bias)
{
	float shadow = 0;

	#if VG_GFX_REVERSE_DEPTH
	float depth = 1.0f - (_shadowUV.z-_bias);	
	#else
	float depth = _shadowUV.z-_bias;	
	#endif

	#if 0
	shadow = _shadowMap.SampleCmpLevelZero(shadowcmp, _shadowUV.xy,  depth, int2(0,0)).x;
	#else
	// PCF9
	shadow += _shadowMap.SampleCmpLevelZero(shadowcmp, _shadowUV.xy, depth, int2(0,-1)).x;	
	shadow += _shadowMap.SampleCmpLevelZero(shadowcmp, _shadowUV.xy, depth, int2(1,-1)).x;	
	shadow += _shadowMap.SampleCmpLevelZero(shadowcmp, _shadowUV.xy, depth, int2(2,-1)).x;	
	shadow += _shadowMap.SampleCmpLevelZero(shadowcmp, _shadowUV.xy, depth, int2(0,0)).x;	
	shadow += _shadowMap.SampleCmpLevelZero(shadowcmp, _shadowUV.xy, depth, int2(1,0)).x;	
	shadow += _shadowMap.SampleCmpLevelZero(shadowcmp, _shadowUV.xy, depth, int2(2,0)).x;	
	shadow += _shadowMap.SampleCmpLevelZero(shadowcmp, _shadowUV.xy, depth, int2(0,+1)).x;	
	shadow += _shadowMap.SampleCmpLevelZero(shadowcmp, _shadowUV.xy, depth, int2(1,+1)).x;	
	shadow += _shadowMap.SampleCmpLevelZero(shadowcmp, _shadowUV.xy, depth, int2(2,+1)).x;
	shadow /= 9.0f;	
	shadow = smoothstep(0.0,1.0,shadow);				
	#endif

	return shadow;
}

//--------------------------------------------------------------------------------------
LightingResult computeDirectLighting(ViewConstants _viewConstants, float3 _eyePos, float3 _worldPos, float3 _albedo, float3 _worldNormal, float4 _pbr)
{
    LightingResult output = (LightingResult)0;
    
    float occlusion = _pbr.r;
    float roughness = _pbr.g;
    float metalness = _pbr.b;

    // Outgoing light direction (vector from world-space fragment position to camera). 
	float3 Lo = normalize(_eyePos - _worldPos.xyz);

    // Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(_worldNormal, Lo));

    // Specular reflection vector.
	float3 Lr = 2.0 * cosLo * _worldNormal - Lo;

    // Fresnel reflectance at normal incidence (for metals use albedo color).
	float3 F0 = lerp(Fdielectric, _albedo, metalness);

	ByteAddressBuffer lights = getBuffer(RESERVEDSLOT_BUFSRV_LIGHTSCONSTANTS);
	LightsConstantsHeader lightsHeader = lights.Load<LightsConstantsHeader>(0);
    uint offset = sizeof(LightsConstantsHeader);	

	#ifdef _RAYTRACING
	RaytracingAccelerationStructure tlas = getTLAS(_viewConstants.getTLASHandle());
	#endif

	#ifdef _TOOLMODE
	uint rayCount = 0;
	#endif

	// IBL

	// environment diffuse
	uint irrandianceCubemapHandle = _viewConstants.getIrradianceCubemap();
	if (ReservedSlot::InvalidTextureCube != (ReservedSlot)irrandianceCubemapHandle)
	{
		TextureCube irradianceCubemap = getTextureCube(irrandianceCubemapHandle);
		{
			uint width, height, mipLevels;
			irradianceCubemap.GetDimensions(0, width, height, mipLevels);

			float3 F = fresnelSchlick(F0, cosLo);
			float3 kd = lerp(1.0 - F, 0.0, metalness);
			float diffuseEnvMip = mipLevels-1;
			output.envDiffuse = kd * irradianceCubemap.SampleLevel(linearClamp, normalize(_worldNormal.rgb), diffuseEnvMip).rgb;
		}
	}
	else
	{
		output.envDiffuse = _viewConstants.getEnvironmentColor();
	}

	// environment specular 
	uint specularReflectionCubemapHandle = _viewConstants.getSpecularReflectionCubemap();
	if (ReservedSlot::InvalidTextureCube != (ReservedSlot)specularReflectionCubemapHandle)
	{
		TextureCube specularReflectionCubemap = getTextureCube(specularReflectionCubemapHandle);
		uint width, height, mipLevels;
		specularReflectionCubemap.GetDimensions(0, width, height, mipLevels);

		Texture2D specularBRDF = getTexture2D(_viewConstants.getSpecularBRDF());
		float2 FG = specularBRDF.SampleLevel(linearClamp, float2(cosLo, 1-roughness), 0).rg;
		float specularEnvMip = roughness * (mipLevels - 1);
		output.envSpecular = (F0 * FG.x + FG.y) * specularReflectionCubemap.SampleLevel(linearClamp, Lr, specularEnvMip).rgb;

		//output.envSpecular = float3(FG.xxx);
	}
	else
	{
		output.envSpecular = _viewConstants.getEnvironmentColor();
	}

	output.envDiffuse *= occlusion * _viewConstants.getIrradianceIntensity();
	output.envSpecular *= occlusion * _viewConstants.getSpecularReflectionIntensity();

	for(uint i=0; i < lightsHeader.getDirectionalCount(); ++i)
	{
		DirectionalLightConstants directional = lights.Load<DirectionalLightConstants>(offset);
		offset += sizeof(DirectionalLightConstants);

		float3 lightDir = directional.getDirection();
		float3 Li = normalize(lightDir);
		float cosLi = max(0.0f, dot(_worldNormal, Li));

		if (cosLi > 0.0f)
		{
			float3 Lradiance = directional.getColor();
			float shadow = 1.0f;
			float si = directional.getShadowInstensity();

			if (si > 0.0f)
			{
				#ifdef _RAYTRACING
				shadow = 1;		

				RayDesc ray;
				ray.Origin    = _worldPos;
				ray.Direction = lightDir;
				ray.TMin      = 0.0325f;
				ray.TMax      = 100;

				RayQuery<RAY_FLAG_NONE> query;
				query.TraceRayInline(tlas, RAY_FLAG_NONE, 0xff, ray);

				#ifdef _TOOLMODE
				rayCount++;
				#endif

				// https://microsoft.github.io/DirectX-Specs/d3d/Raytracing.html#tracerayinline-example-2
				while(query.Proceed())
				{
					switch(query.CandidateType())
					{
						default:
						break;
				
						case CANDIDATE_NON_OPAQUE_TRIANGLE:
						shadow = 0.5f;
						break;
					}
				}

				switch(query.CommittedStatus())
				{
					default:
					case COMMITTED_NOTHING:
					break;

					case COMMITTED_TRIANGLE_HIT:
					shadow = 0.0f;
					break;
				}
				
				#else // _RAYTRACING

				float4 shadowUV = mul(directional.getShadowMatrix(), float4(_worldPos, 1.0f));
					   shadowUV.xyz /= shadowUV.w;
					   shadowUV.xy = shadowUV.xy*0.5f+0.5f;
					   shadowUV.y = 1-shadowUV.y;
		
				if (all(shadowUV.xy == saturate(shadowUV.xy) ))
				{			
					Texture2D shadowMap = getTexture2D(directional.getShadowMapTextureHandle());
					float bias = directional.getShadowBias();
					shadow = SampleDirectionalShadowMap(shadowMap, shadowUV.xyz, bias);
				}
				#endif		

				shadow = 1.0f - ((1.0f-shadow) * si);
			}

			output.addLightContribution(Lo, cosLo, cosLi, Lr, F0, Li, Lradiance * shadow, _worldNormal, roughness, metalness);
		}
	}	

	for(uint i=0; i < lightsHeader.getOmniCount(); ++i)
	{
		OmniLightConstants omni = lights.Load<OmniLightConstants>(offset + i * sizeof(OmniLightConstants)); 

		float3 lightDir = omni.getPosition() - _worldPos.xyz;
		float dist = length(lightDir);

		float att = getRangeAttenuation(dist, omni.getRadius());

		if (att > lightEps)
		{
			float3 color = omni.getColor();
			float3 Li = normalize(lightDir);
			float cosLi = max(0.0f, dot(_worldNormal, Li));

			if (cosLi > 0.0f)
			{
				float3 Lradiance = att * color;
				float shadow = 1.0f;
				float si = omni.getShadowInstensity();

				if (si > 0.0f)
				{
					#ifdef _RAYTRACING
					RayDesc ray;
					ray.Origin    = _worldPos;
					ray.Direction = lightDir;
					ray.TMin      = 0.0325f;
					ray.TMax      = 10;

					RayQuery<RAY_FLAG_FORCE_OPAQUE> query;
					query.TraceRayInline(tlas, 0, 0xff, ray);
					query.Proceed();

					#ifdef _TOOLMODE
					rayCount++;
					#endif

					switch(query.CommittedStatus())
					{
						case COMMITTED_TRIANGLE_HIT:
							shadow = 0;     
							break;
					}
					#endif // _RAYTRACING

					shadow = 1.0f - ((1.0f-shadow) * si);
				}

				output.addLightContribution(Lo, cosLo, cosLi, Lr, F0, Li, Lradiance * shadow, _worldNormal, roughness, metalness);
			}
		}
    }	

	#ifdef _TOOLMODE
	switch(_viewConstants.getDisplayMode())
	{
		default:
			break;
		
		case DisplayMode::Lighting_RayCount:
			output.directLightDiffuse = heatmapGradient(rayCount, 1.0f, 3.0f, 6.0f);
			break;
	}
	#endif

    return output;
}

//--------------------------------------------------------------------------------------
float3 applyLighting(float3 _albedo, LightingResult _lighting, DisplayMode _displayMode)
{
	#ifdef _TOOLMODE
	switch(_displayMode)
	{
		default:
			break;

		case DisplayMode::Lighting_EnvironmentDiffuse:
			return _lighting.envDiffuse.rgb;

		case DisplayMode::Lighting_EnvironmentSpecular:
			return _lighting.envSpecular.rgb;

		case DisplayMode::Lighting_DirectDiffuse:
			return _lighting.directLightDiffuse.rgb;

		case DisplayMode::Lighting_DirectSpecular:
			return _lighting.directLightSpecular.rgb;

		case DisplayMode::Lighting_Diffuse:
			return _lighting.envDiffuse.rgb + _lighting.directLightDiffuse.rgb;

		case DisplayMode::Lighting_Specular:
			return _lighting.envSpecular.rgb + _lighting.directLightSpecular.rgb;

		case DisplayMode::Lighting_RayCount:
			return _lighting.directLightDiffuse.rgb;
	}
	#endif

    return (_lighting.envDiffuse + _lighting.directLightDiffuse) * _albedo.rgb + (_lighting.envSpecular.rgb + _lighting.directLightSpecular.rgb);
}