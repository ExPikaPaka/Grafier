#pragma once

#include <iostream>
#include <sys/stat.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <fileapi.h>
#undef min
#undef max
#else
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace ent {
    namespace io {

        bool fileExists(const std::string& filePath) {
            #ifdef _WIN32
            DWORD ftyp = GetFileAttributesA(filePath.c_str());
            return (ftyp != INVALID_FILE_ATTRIBUTES && !(ftyp & FILE_ATTRIBUTE_DIRECTORY));
            #else
            struct stat info;
            return (stat(filePath.c_str(), &info) == 0 && S_ISREG(info.st_mode));
            #endif
        }

        bool directoryExists(const std::string& dirPath) {
            #ifdef _WIN32
            DWORD ftyp = GetFileAttributesA(dirPath.c_str());
            return (ftyp != INVALID_FILE_ATTRIBUTES && (ftyp & FILE_ATTRIBUTE_DIRECTORY));
            #else
            struct stat info;
            return (stat(dirPath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
            #endif
        }

        bool createDirectory(const std::string& dirPath) {
            if (!directoryExists(dirPath)) {
                #ifdef _WIN32
                return CreateDirectoryA(dirPath.c_str(), NULL) != 0;
                #else
                return mkdir(dirPath.c_str(), 0755) == 0;
                #endif
            }
            return false;
        }

        bool clearDirectory(const std::string& dirPath, bool debug = false) {
            #ifdef _WIN32
            std::string searchPath = dirPath + "\\*";
            WIN32_FIND_DATAA findData;
            HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    const std::string fileName = findData.cFileName;
                    if (fileName != "." && fileName != "..") {
                        std::string fullPath = dirPath + "\\" + fileName;

                        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                            // Recursively delete the contents of the directory
                            clearDirectory(fullPath, debug);
                            RemoveDirectoryA(fullPath.c_str());
                        } else {
                            DeleteFileA(fullPath.c_str());
                        }
                    }
                } while (FindNextFileA(hFind, &findData) != 0);
                FindClose(hFind);
                return true;
            } else {
                if (debug) std::cerr << "Error accessing directory: " << dirPath << std::endl;
                return false;
            }
            #else // POSIX (Linux/macOS)
            DIR* dir = opendir(dirPath.c_str());
            if (!dir) {
                if (debug) std::cerr << "Error accessing directory: " << dirPath << std::endl;
                return false;
            }

            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string fileName = entry->d_name;
                if (fileName != "." && fileName != "..") {
                    std::string fullPath = dirPath + "/" + fileName;

                    struct stat pathStat;
                    stat(fullPath.c_str(), &pathStat);
                    if (S_ISDIR(pathStat.st_mode)) {
                        // Recursively delete the contents of the subdirectory
                        clearDirectory(fullPath, debug);
                        rmdir(fullPath.c_str());
                    } else {
                        remove(fullPath.c_str());
                    }
                }
            }
            closedir(dir);
            return true;
            #endif
        }

        bool deleteDirectory(const std::string& dirPath) {
            if (directoryExists(dirPath)) {
                #ifdef _WIN32
                return RemoveDirectoryA(dirPath.c_str()) != 0;
                #else
                return rmdir(dirPath.c_str()) == 0;
                #endif
            }
            return false;
        }

    }
}
