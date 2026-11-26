#pragma once
#include <string>
#include <filesystem>

#ifndef ASSET_DIR
#define ASSET_DIR "/usr/share/payobros/assets/"
#endif

inline std::string resolveAssetPath(const std::string& path) {
    namespace fs = std::filesystem;

    if (fs::exists(path)) {
        return path;
    }

    const std::string devPrefix = "src/assets/";
    if (path.rfind(devPrefix, 0) == 0) {
        std::string relative = path.substr(devPrefix.length());
        fs::path installedPath = fs::path(ASSET_DIR) / relative;
        if (fs::exists(installedPath)) {
            return installedPath.string();
        }
    }

    return path;
}