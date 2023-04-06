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

    void UILayer::OnAttach() {}

    void UILayer::OnDetach() {}

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
