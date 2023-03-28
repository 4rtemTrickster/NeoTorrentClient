#include "NTCpch.h"
#include "SingleFileTorrent.h"

namespace NTC
{
    SingleFileTorrent::SingleFileTorrent(const std::string& announce, int64_t pieceLength,
        Ref<std::vector<std::string>> pieceHashes, const std::string& name, int64_t length)
            : Parent(announce, std::move(pieceHashes), pieceLength),
              Name_(name), Length_(length) {}

    SingleFileTorrent::SingleFileTorrent(std::string&& announce, int64_t pieceLength, Ref<std::vector<std::string>>&& pieceHashes,
        std::string&& name, int64_t length)
            : Parent(std::move(announce), std::move(pieceHashes), pieceLength),
              Name_(std::move(name)), Length_(length) {}

    void SingleFileTorrent::SetMd5Sum(const std::string& md5sum) { Md5Sum = md5sum; }

    void SingleFileTorrent::SetMd5Sum(std::string&& md5sum) { Md5Sum = std::move(md5sum); }

    void SingleFileTorrent::SetLength(int64_t length) { Length_ = length; }
}