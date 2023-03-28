#include "NTCpch.h"

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "AppSettings/AppSettings.h"
#include "FileReader/FileReader.h"
#include "Torrent/Bencode/BencodeTypes/BencodeDictionary/BencodeDictionary.h"
#include "Torrent/Downloader/Downloader.h"
#include "Torrent/TorrentFileFactory/TorrentFileFactory.h"
#include "Torrent/Bencode/BencodeDecoder/BencodeDecoder.h"
#include "Torrent/TorrentPeer/TorrentPeer.h"


class ExL : public Walnut::Layer
{
public:
    virtual void OnAttach() override
    {
        NTC::Logger::Init();
        netF = std::async(std::launch::async, &NTC::TorrentPeer::StartLoop);

        //TODO: Change to settings file
        NTC::AppSettings::ClientId = "-NT0001-";

        std::random_device rd; // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution distrib(1, 9);
        for (int i = 0; i < 12; i++)
            NTC::AppSettings::ClientId += std::to_string(distrib(gen));

        NTC::AppSettings::Port = 8080;
    }

    virtual void OnDetach() override
    {
        NTC::AppSettings::bIsWorking = false;
        netF.wait();
    }

    virtual void OnUIRender() override
    {
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
        ImGui::Begin("ClickBtn", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);
        ImGui::Button("FFFFFF");
        ImGui::End();
    }

protected:
    std::future<void> netF;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification specification;
    specification.Name = "NeoTorrent";

    Walnut::Application* app = new Walnut::Application(specification);

    app->PushLayer<ExL>();
    app->SetMenubarCallback([app]()
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Add torrent"))
            {
                //TODO: File dialog here!
                std::string path("C:/res/debian-10.10.0-amd64-netinst.iso.torrent");
                auto readed = NTC::FileReader::ReadFile(std::filesystem::path(path));

                NTC::Ref<NTC::BencodeDictionary> dic = std::dynamic_pointer_cast<NTC::BencodeDictionary>(
                    NTC::BencodeDecoder::Decode(*readed));

                auto f = NTC::TorrentFileFactory::CreateTorrentFile(dic);

                if (f != nullptr)
                {
                    NTC_TRACE("Torrent file created!");
                    NTC::Downloader::DownloadFile(f);
                }
                else
                    NTC_TRACE("Error during torrent file creation!");
            }
            ImGui::EndMenu();
        }
    });

    return app;
}
