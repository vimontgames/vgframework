#pragma once

static float lightEps = 0.01f;

struct LightsConstantsHeader
{
	#ifndef __cplusplus
	uint Load(ByteAddressBuffer _buffer, uint _offset = 0)
	{       
		m_counters   = _buffer.Load<uint4>(_offset);	_offset += sizeof(uint4);
		return _offset;
	}
	#endif

	uint4	m_counters;

	void	setDirectionalCount	(uint _count)		{ m_counters.x = _count; }
	uint	getDirectionalCount	()					{ return m_counters.x; }

	void	setOmniCount		(uint _count)		{ m_counters.y = _count; }
	uint	getOmniCount		()					{ return m_counters.y; }
};

struct OmniLightConstants 
{
	float4	m_color;
	float4	m_position;

	void	setColor			(float3 _color)		{ m_color.rgb = _color; }
	float3	getColor			()					{ return m_color.rgb;}
	
	void	setPosition			(float3 _position)	{ m_position.xyz = _position;}
	float3	getPosition			()					{ return m_position.xyz; }

	void	setRadius			(float _radius)		{ m_position.w = _radius; }
	float	getRadius			()					{ return m_position.w; }
};

struct DirectionalLightConstants 
{
	float4	m_color;
	float4 m_direction;

	void	setColor			(float3 _color)		{ m_color.rgb = _color; }
	float3	getColor			()					{ return m_color.rgb;}
	
	void	setDirection		(float3 _direction)	{ m_direction.xyz = _direction; }
	float3	getDirection		()					{ return m_direction.xyz; }
};