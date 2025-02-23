#pragma once



#include <filesystem>
#include <functional>

#ifdef _WIN32
#include <Windows.h>
#endif
namespace NAssist {

class FileSystem {

public:
  static std::filesystem::path
  OpenFileDialog(HWND hwnd, const char *filter = "All\0*.*\0");
  static std::filesystem::path OpenFolderDialog(const char *initialFolder = "");
  static std::filesystem::path
  SaveFileDialog(HWND hwnd, const char *filter = "All\0*.*\0");
};
} // namespace NAssist
