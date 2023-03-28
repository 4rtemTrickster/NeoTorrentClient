#pragma once

namespace NTC
{
    class FileReader final
    {
    public:
        FileReader() = delete;
        FileReader(const FileReader&) = delete;
        FileReader(const FileReader&&) = delete;
        FileReader& operator=(const FileReader&) = delete;
        FileReader& operator=(const FileReader&&) = delete;

        [[nodiscard]]
        static Ref<std::string> ReadFile(const std::filesystem::path& path);

    protected:
        
        static std::fstream fs_;
    };
}