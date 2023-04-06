#include "NTCpch.h"

#include "UI/UILayer.h"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification specification;
    specification.Name = "NeoTorrent";

    Walnut::Application* app = new Walnut::Application(specification);

    app->PushLayer<NTC::UILayer>();
    app->SetMenubarCallback(&NTC::UILayer::MenuBarCallback);

    return app;
}
