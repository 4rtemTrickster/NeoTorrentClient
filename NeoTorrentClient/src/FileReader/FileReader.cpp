#include "NTCpch.h"
#include "FileReader.h"

namespace NTC
{
    std::fstream FileReader::fs_;
    
    Ref<std::string> FileReader::ReadFile(const std::filesystem::path& path)
    {
        NTC_PROFILE_FUNCTION();
        NTC_TRACE("Reading new file");
        Ref<std::string> ret = CreateRef<std::string>();

        fs_.open(path, std::ios_base::in | std::ios_base::binary);

        if (fs_.is_open())
        {
            fs_.seekg(0, std::ios_base::end);
            std::fstream::pos_type fileSize = fs_.tellg();

            if (fileSize == std::fstream::pos_type(-1))
            {
                NTC_WARN("Can't get current position of the get pointer!\nFile:" + path.string());
                return ret;
            }

            fs_.seekg(0, std::fstream::beg);
            ret->resize(fileSize + 1ll);
            fs_.read(ret->data(), fileSize);
            fs_.close();
        }
        else
            NTC_WARN("Can't open file:" + path.string());

        return ret;
    }
}