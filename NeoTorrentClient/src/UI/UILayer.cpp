#include "NTCpch.h"
#include "UILayer.h"

namespace NTC
{
    void UILayer::MenuBarCallback()
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Add torrent"))
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".torrent", "C://");

            ImGui::EndMenu();
        }
    }

    void UILayer::OnAttach()
    {
        Logger::Init();
        netF = std::async(std::launch::async, &TorrentPeer::StartLoop);

        //TODO: Change to settings file
        AppSettings::ClientId = "-NT0001-";

        std::random_device rd; // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution distrib(1, 9);
        for (int i = 0; i < 12; i++)
            AppSettings::ClientId += std::to_string(distrib(gen));

        AppSettings::Port = 8080;
    }

    void UILayer::OnDetach()
    {
        AppSettings::bIsWorking = false;
        netF.wait();
    }

    void UILayer::OnUIRender()
    {
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoDocking))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                auto res = std::async(std::launch::async, [&filePathName]()
                {
                    try
                    {
                        auto readed = FileReader::ReadFile(std::filesystem::path(filePathName));

                        Ref<BencodeDictionary> dic = DynamicCast<BencodeDictionary>(
                            BencodeDecoder::Decode(*readed));

                        auto f = TorrentFileFactory::CreateTorrentFile(dic);

                        if (f != nullptr)
                        {
                            NTC_TRACE("Torrent file created!");
                            Downloader::DownloadFile(f);
                        }
                        else
                            NTC_TRACE("Error during torrent file creation!");
                    }
                    catch (std::exception& e)
                    {
                        NTC_CRITICAL("{0}", e.what());
                    }
                });
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
    }
}
