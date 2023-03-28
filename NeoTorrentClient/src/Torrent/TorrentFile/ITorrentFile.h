#pragma once

namespace NTC
{
    using AnnounceList_t = std::list<std::string>;
    constexpr std::size_t HasSize = 20;
    using Hash_t = std::array<std::byte, HasSize>;

    class ITorrentFile
    {
    public:
        ITorrentFile(const ITorrentFile& other) = delete;
        ITorrentFile(ITorrentFile&& other) = delete;
        ITorrentFile& operator=(const ITorrentFile&) = delete;
        ITorrentFile& operator=(ITorrentFile&&) = delete;

        #pragma region Getters
        [[nodiscard]]
        virtual const std::string& GetName() const = 0;

        [[nodiscard]]
        const std::string& GetAnnounce() const { return Announce_; }

        [[nodiscard]]
        const std::string& GetInfoHash() const { return InfoHash_; }

        [[nodiscard]]
        const AnnounceList_t& GetAnnounceList() const { return AnnounceList_; }

        [[nodiscard]]
        const int64_t& GetCreationDate() const { return CreationDate_; }

        [[nodiscard]]
        const std::string& GetComment() const { return Comment_; }

        [[nodiscard]]
        const std::string& GetCreatedBy() const { return CreatedBy_; }

        [[nodiscard]]
        const std::string& GetEncoding() const { return Encoding_; }

        [[nodiscard]]
        const int64_t& GetPieceLength() const { return PieceLength_; }

        [[nodiscard]]
        const int64_t& GetPrivate() const { return Private_; }

        [[nodiscard]]
        Ref<std::vector<std::string>> GetPieceHashes() { return PieceHashes_; }

        
        /**
         * \return The number of bytes needed to download to be 100% complete and get all the included files in the torrent.
         */
        [[nodiscard]]
        virtual int64_t GetLength() = 0;

        #pragma endregion Getters

        #pragma region Setters
        void SetInfoHash(const std::string& infoHash);
        void SetInfoHash(std::string&& infoHash);
        
        void SetAnnounceList(const AnnounceList_t& announceList);
        void SetAnnounceList(AnnounceList_t&& announceList);

        void SetCreationDate(int64_t creationDate);

        void SetComment(const std::string& comment);
        void SetComment(std::string&& comment);

        void SetCreatedBy(const std::string& createdBy);
        void SetCreatedBy(std::string&& createdBy);

        void SetEncoding(const std::string& encoding);
        void SetEncoding(std::string&& encoding);

        void SetPrivate(int64_t inPrivate);
        #pragma endregion Setters

    protected:
        ITorrentFile(const std::string& announce, Ref<std::vector<std::string>> pieceHashes, int64_t pieceLength);
        ITorrentFile(std::string&& announce, Ref<std::vector<std::string>>&& pieceHashes, int64_t pieceLength);
        virtual ~ITorrentFile() = default;

        std::string Announce_;
        std::string InfoHash_;
        AnnounceList_t AnnounceList_;
        int64_t CreationDate_ = 0;
        std::string Comment_;
        std::string CreatedBy_;
        std::string Encoding_;

        int64_t PieceLength_;
        int64_t Private_;
        Ref<std::vector<std::string>> PieceHashes_;
    };
}