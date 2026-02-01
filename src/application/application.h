#pragma once

#include "core/IPlugin.h"
#include "application_consts.h"

namespace vg
{
    namespace core { struct Singletons; }
    namespace engine { class IEngine; }

    class Application 
    {
    public:
        Application();
        ~Application();

        void Init(const ApplicationCreationParams & _creationParams, const core::Singletons & _singletons);
        void Deinit();

        bool Update();

    private:
        bool RegisterClasses();

    private:
        vg::engine::IEngine * m_engine = nullptr;
    };

}