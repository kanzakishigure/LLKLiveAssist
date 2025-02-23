
#include "Core/FileSystem.h"

#include <Shlobj.h>
#include <Windows.h>
#include <algorithm>

#include <filesystem>

namespace NAssist {

std::filesystem::path FileSystem::OpenFileDialog(HWND hwnd,
                                                 const char *filter) {
  OPENFILENAMEA ofn;      // common dialog box structure
  CHAR szFile[260] = {0}; // if using TCHAR macros

  // Initialize OPENFILENAME
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = filter;
  ofn.nFilterIndex = 1;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

  if (GetOpenFileNameA(&ofn) == TRUE) {
    std::string fp = ofn.lpstrFile;
    std::replace(fp.begin(), fp.end(), '\\', '/');
    return std::filesystem::path(fp);
  }

  return std::filesystem::path();
}

std::filesystem::path FileSystem::OpenFolderDialog(const char *initialFolder) {
  std::filesystem::path result = "";
  IFileOpenDialog *dialog;
  if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                                 IID_IFileOpenDialog, (void **)&dialog))) {
    DWORD options;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_PICKFOLDERS);
    if (SUCCEEDED(dialog->Show(NULL))) {
      IShellItem *selectedItem;
      if (SUCCEEDED(dialog->GetResult(&selectedItem))) {
        PWSTR pszFilePath;
        if (SUCCEEDED(selectedItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING,
                                                   &pszFilePath))) {
          result = std::filesystem::path(pszFilePath,
                                         std::filesystem::path::generic_format);
          CoTaskMemFree(pszFilePath);
        }

        selectedItem->Release();
      }
    }

    dialog->Release();
  }

  std::string fp = result.string();
  std::replace(fp.begin(), fp.end(), '\\', '/');
  return fp;
}

std::filesystem::path FileSystem::SaveFileDialog(HWND hwnd,
                                                 const char *filter) {
  OPENFILENAMEA ofn;      // common dialog box structure
  CHAR szFile[260] = {0}; // if using TCHAR macros

  // Initialize OPENFILENAME
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = filter;
  ofn.nFilterIndex = 1;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

  if (GetSaveFileNameA(&ofn) == TRUE) {
    std::string fp = ofn.lpstrFile;
    std::replace(fp.begin(), fp.end(), '\\', '/');
    return std::filesystem::path(fp);
  }

  return std::filesystem::path();
}

} // namespace NAssist
