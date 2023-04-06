#include "NTCpch.h"

#include "LogicLayer/LogicLayer.h"
#include "UI/UILayer.h"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    ApplicationSpecification specification;
    specification.Name = "NeoTorrent";

    Application* app = new Application(specification);

    app->PushLayer<NTC::LogicLayer>();
    app->PushLayer<NTC::UILayer>();
    app->SetMenubarCallback(&NTC::UILayer::MenuBarCallback);

    return app;
}
