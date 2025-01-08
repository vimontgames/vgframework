#include "core/Precomp.h"
#include "Math.h"

#if !VG_ENABLE_INLINE
#include "Math.inl"
#endif

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    float3x3 extractRotation(const float4x4 & _matrix)
    {
        float3x3 rot;
        rot[0] = float3(normalize(_matrix[0].xyz));
        rot[1] = float3(normalize(_matrix[1].xyz));
        rot[2] = float3(normalize(_matrix[2].xyz));
        return rot;
    }

    //--------------------------------------------------------------------------------------
    float smoothdampImpl(float _currentValue, float _targetValue, float & _currentVelocity, float _duration, float _dt)
    {
        VG_ASSERT(_duration > 0.0f);

        float omega = 2.0f / _duration;
        float x = omega * _dt;
        float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
        float delta = _currentValue - _targetValue;
        float originalTarget = _targetValue;

        _targetValue = _currentValue - delta;

        float temp = (_currentVelocity + omega * delta) * _dt;

        _currentVelocity = (_currentVelocity - omega * temp) * exp;

        float result = _targetValue + (delta + temp) * exp;

        if ((originalTarget - _currentValue > 0.0f) == (result > originalTarget))
        {
            result = originalTarget;
            _currentVelocity = (result - originalTarget) / _dt;
        }

        return result;
    }

    //--------------------------------------------------------------------------------------
    float smoothdamp(float _currentValue, float _targetValue, float & _currentVelocity, float _duration, float _dt)
    {
        return smoothdampImpl(_currentValue, _targetValue, _currentVelocity, _duration, _dt);
    }

    //--------------------------------------------------------------------------------------
    float2 smoothdamp(float2 _currentValue, float2 _targetValue, float2 & _currentVelocity, float _duration, float _dt)
    {
        float vX = (float)_currentVelocity.x;
        float vY = (float)_currentVelocity.y;

        float2 result;
        result.x = smoothdamp(_currentValue.x, _targetValue.x, vX, _duration, _dt);
        result.y = smoothdamp(_currentValue.y, _targetValue.y, vY, _duration, _dt);

        _currentValue = float2(vX, vY);
        return result;
    }

    //--------------------------------------------------------------------------------------
    float3 smoothdamp(float3 _currentValue, float3 _targetValue, float3 & _currentVelocity, float _duration, float _dt)
    {       
        float vX = (float)_currentVelocity.x;
        float vY = (float)_currentVelocity.y;
        float vZ = (float)_currentVelocity.z;
        
        float3 result;
        result.x = smoothdamp(_currentValue.x, _targetValue.x, vX, _duration, _dt);
        result.y = smoothdamp(_currentValue.y, _targetValue.y, vY, _duration, _dt);
        result.z = smoothdamp(_currentValue.z, _targetValue.z, vZ, _duration, _dt);
        
        _currentValue = float3(vX, vY, vZ);
        return result;
    }

    //--------------------------------------------------------------------------------------
    float4 smoothdamp(float4 _currentValue, float4 _targetValue, float4 & _currentVelocity, float _duration, float _dt)
    {
        float vX = (float)_currentVelocity.x;
        float vY = (float)_currentVelocity.y;
        float vZ = (float)_currentVelocity.z;
        float vW = (float)_currentVelocity.w;

        float4 result;
        result.x = smoothdamp(_currentValue.x, _targetValue.x, vX, _duration, _dt);
        result.y = smoothdamp(_currentValue.y, _targetValue.y, vY, _duration, _dt);
        result.z = smoothdamp(_currentValue.z, _targetValue.z, vZ, _duration, _dt);
        result.w = smoothdamp(_currentValue.w, _targetValue.w, vW, _duration, _dt);

        _currentValue = float4(vX, vY, vZ, vW);
        return result;
    }

    //--------------------------------------------------------------------------------------
    quaternion slerpShortestPath(quaternion _q0, quaternion _q1, float _blend)
    {
        _q0 = normalize(_q0);
        _q1 = normalize(_q1);
        _blend = clamp(_blend, 0.0f, 1.0f);

        // Compute cosine of the angle between q0 and q1
        float cosq = dot(_q0, _q1);

        // If the dot product is smaller than epsilon, negate one quaternion to take the shorter path
        const float eps = 0.01f;
        if (cosq < -eps)
        {
            _q0.x = -_q0.x;
            _q0.y = -_q0.y;
            _q0.z = -_q0.z;
            _q0.w = -_q0.w;
            cosq = -cosq;
        }

        // If the quaternions are too close use linear interpolation
        const float cosq_threshold = 0.9995f;
        if (cosq > cosq_threshold)
        {
            quaternion result = normalize(lerp(_q0, _q1, _blend));
            return result;
        }

        // Calculate the angle and interpolate
        float theta_0 = acos(cosq);     
        float theta = theta_0 * _blend;  

        float2 sin_theta = sin(float2(theta, theta_0));

        float s1 = sin_theta.x / sin_theta.y;   
        float s0 = cos(theta) - cosq * s1;     

        return quaternion(s0 * _q0 + s1 * _q1);
    }
}