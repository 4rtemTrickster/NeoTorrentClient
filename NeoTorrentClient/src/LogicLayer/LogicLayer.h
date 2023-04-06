#pragma once
#include "Walnut/Layer.h"

namespace NTC
{
    class LogicLayer : public Walnut::Layer
    {
    public:
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate(float ts) override;
    };
}
