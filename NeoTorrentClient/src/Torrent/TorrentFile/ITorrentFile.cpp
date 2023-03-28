#include "NTCpch.h"
#include "ITorrentFile.h"

namespace NTC
{
    ITorrentFile::ITorrentFile(const std::string& announce, Ref<std::vector<std::string>> pieceHashes, int64_t pieceLength)
        : Announce_(announce), PieceLength_(pieceLength), PieceHashes_(pieceHashes) {}

    ITorrentFile::ITorrentFile(std::string&& announce, Ref<std::vector<std::string>>&& pieceHashes, int64_t pieceLength)
        : Announce_(std::move(announce)), PieceLength_(pieceLength), PieceHashes_(std::move(pieceHashes)) {}


#pragma region Setters
    void ITorrentFile::SetInfoHash(const std::string& infoHash) { InfoHash_ = infoHash; }

    void ITorrentFile::SetInfoHash(std::string&& infoHash) { InfoHash_ = std::move(infoHash); }

    void ITorrentFile::SetAnnounceList(const AnnounceList_t& announceList) { AnnounceList_ = announceList; }

    void ITorrentFile::SetAnnounceList(AnnounceList_t&& announceList) { AnnounceList_ = std::move(announceList); }

    void ITorrentFile::SetCreationDate(int64_t creationDate) { CreationDate_ = creationDate; }

    void ITorrentFile::SetComment(const std::string& comment) { Comment_ = comment; }

    void ITorrentFile::SetComment(std::string&& comment) { Comment_ = std::move(comment); }

    void ITorrentFile::SetCreatedBy(const std::string& createdBy) { CreatedBy_ = createdBy; }

    void ITorrentFile::SetCreatedBy(std::string&& createdBy) { CreatedBy_ = std::move(createdBy); }

    void ITorrentFile::SetEncoding(const std::string& encoding) { Encoding_ = encoding; }

    void ITorrentFile::SetEncoding(std::string&& encoding) { Encoding_ = std::move(encoding); }

    void ITorrentFile::SetPrivate(int64_t inPrivate) { Private_ = inPrivate; }

#pragma endregion Setters
}