#pragma once

namespace NTC
{
    template <typename T>
    class Connection;

    template <typename T>
    class IComp
    {
    public:
        virtual void OnValidated(Ref<Connection<T>> con) = 0;
        virtual ~IComp() = default;
    };
}