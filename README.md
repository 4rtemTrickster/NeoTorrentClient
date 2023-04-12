# NeoTorrentClient

## Purpose:

Lightweight and fast torrent client with a modern interface written in c++

## Basis:
Basic information about the torrent client can be found in these posts:

- [Building a BitTorrent client from the ground up in Go][1]
- [A BitTorrent client in Python 3.5][2]
- [Building a BitTorrent client from scratch in C#][3]

Information about bittorrent protocol you can find in [unofficial documentation][4] or [official][5].

## Requirements
- [Visual Studio 2022][15] or [Rider 2022.3.X][17] (not strictly required, 
however included setup scripts only support this).
- [Vulkan SDK][16] (preferably a recent version).

## Dependencies:

- Logging: [spdlog][6]
- Building: [premake5][7]
- UI: [my fork][8]
  of [Walnut][9]
- FileDialog: [my fork][10]
  of [ImGuiFileDialog][11]
- Networking: [boost][12]
- Http requests: [cpr][13]

## Building:
1. Clone the repository with all submodules **(and submodules of submodules)** using the method you choose.
2. Install non submodule dependencies([cpr][13], [boost][12]).
3. Once you've cloned and init all submodules, run `scripts/Setup.bat` to generate Visual Studio 2022 solution/project files.
4. Now the project is ready to work with.

 
You can download and install [cpr][13] and [boost][12] using the [vcpkg][14] dependency manager:
```Bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install cpr
./vcpkg install boost
```






[1]: (https://blog.jse.li/posts/torrent/)
[2]: (https://markuseliasson.se/article/bittorrent-in-python/)
[3]: (https://www.seanjoflynn.com/research/bittorrent.html)
[4]: (https://wiki.theory.org/BitTorrentSpecification)
[5]: (http://bittorrent.org/beps/bep_0003.html)

[6]: (https://github.com/gabime/spdlog/tree/069a2e8fc947f63855d770fdc3c3eb427f19988f)
[7]: (https://premake.github.io/)
[8]: (https://github.com/4rtemTrickster/Walnut-for-NeoTorrentClient/tree/baf646cbc8d217bd47314fbc45cca97ccf010094)
[9]: (https://github.com/TheCherno/Walnut)
[10]: (https://github.com/4rtemTrickster/ImGuiFileDialog-For-NeoTorrentClient/tree/d8ce36c91f59206411b51965d9a82f0b42c10497)
[11]: (https://github.com/aiekick/ImGuiFileDialog)
[12]: (https://www.boost.org/)
[13]: (https://github.com/libcpr/cpr)
[14]: (https://github.com/Microsoft/vcpkg)
[15]: (https://visualstudio.com)
[16]: (https://vulkan.lunarg.com/sdk/home#windows)
[17]: (https://www.jetbrains.com/rider/)