namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const renderer::IAnimation * AnimationResource::getAnimation() const
    {
        return (renderer::IAnimation *)m_shared; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE renderer::IAnimation * AnimationResource::getAnimation()
    {
        return (renderer::IAnimation *)m_shared; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool AnimationResource::isPlaying() const
    {
        return m_play; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float AnimationResource::getTime() const
    {
        return m_time; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float AnimationResource::getWeight() const
    {
        return m_weight; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float AnimationResource::getSpeed() const
    {
        return m_speed; 
    }
                                        
    //--------------------------------------------------------------------------------------
    VG_INLINE void AnimationResource::setPlay(bool _play)
    {
        m_play = _play; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void AnimationResource::setLoop(bool _loop)
    {
        m_loop = _loop;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint AnimationResource::getLayer() const
    {
        return m_layer;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE renderer::BodyPartFlags AnimationResource::getBodyParts() const
    {
        return m_useBodyParts ? m_bodyParts : (renderer::BodyPartFlags)-1;
    }
}