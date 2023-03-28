#pragma once
#include "Torrent/TorrentFile/ITorrentFile.h"

namespace NTC
{
    
    class MultipleFileTorrent : public ITorrentFile
    {
    public:
        using Parent = ITorrentFile;

        struct file
        {
        private:
            int64_t Length_;
            std::string Md5Sum_;
            std::list<std::string> Path_;

        public:
            file(int64_t length, std::list<std::string>& path);
            file(int64_t length, std::list<std::string>&& path);

            file(const file& other) = default;
            file(file&& other) noexcept;

            ~file() = default;

            [[nodiscard]]
            inline const std::string& GetMd5Sum() const { return Md5Sum_; }

            [[nodiscard]]
            inline std::string& GetMd5Sum() { return Md5Sum_; }

            [[nodiscard]]
            inline int64_t GetLength() const { return Length_; }

            [[nodiscard]]
            inline int64_t GetLength() { return Length_; }

            inline void SetMd5Sum(const std::string& md5sum) { Md5Sum_ = md5sum; }
            inline void SetMd5Sum(std::string&& md5sum) { Md5Sum_ = std::move(md5sum); }
        };
        
        MultipleFileTorrent(
            const std::string& announce,
            int64_t pieceLength,
            Ref<std::vector<std::string>> pieceHashes,
            const std::string& name,
            const std::list<file>& files);

        MultipleFileTorrent(
            std::string&& announce,
            int64_t pieceLength,
            Ref<std::vector<std::string>>&& pieceHashes,
            std::string&& name,
            std::list<file>&& files);

        [[nodiscard]]
        virtual const std::string& GetName() const override { return Name_; }

        [[nodiscard]]
        virtual int64_t GetLength() override;
    
    protected:
        
        std::string Name_;
        std::list<file> Files_;
    };
}