#pragma once

#include <stdio.h>
#include <string>
#include <filesystem>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;


class FilesystemJS  {
public:
    FilesystemJS() = delete;
    
    static inline std::map<std::string, std::string> mounts;
    
    static std::string _getMountPath(std::string baseDir) {
        for (const auto& [mountPoint, originalPath] : mounts) {
            if (baseDir.compare(0, mountPoint.size(), mountPoint) == 0) {
                return originalPath + baseDir.substr(mountPoint.size());
            }
        }
        return "";
    }
    
    static void createPath(std::string baseDir, std::string path) {
        try {
            auto joinedPath = fs::path(baseDir) / path;
            auto modifiedBaseDir = _getMountPath(joinedPath.string());

            if (fs::exists(fs::path(modifiedBaseDir))) {
                return;
            }
            // Create a directory or file
            if(fs::create_directories(fs::path(modifiedBaseDir))) {
                std::cout << "Path created: " << (fs::path(modifiedBaseDir)) << std::endl;
                return;
            } else {
                throw std::invalid_argument("Failed to create path.");
            }

            // If no mount point matches, raise an error
            if (modifiedBaseDir.empty()) {
                throw std::invalid_argument("Invalid baseDir. It is not within a mount point.");
            }
        } catch (const std::exception& e) {
            std::cerr << "Error creating path: " << e.what() << std::endl;
        }
    }
    
    static std::string readdir(std::string path) {
        std::stringstream ss;
        ss << "[\".\",\"..\",";
        auto modifiedBaseDir = _getMountPath(path);
        
        try {

            // Iterate through the directory contents
            for (const auto& entry : fs::directory_iterator(modifiedBaseDir)) {
                ss << "\"" << entry.path().filename().string() << "\",";
            }
            ss.seekp(-1, std::ios_base::end);

        } catch (const std::exception& e) {
            std::cerr << "Error reading directory: " << e.what() << std::endl;
        }

        ss << "]";
        return ss.str();
    }
    
    static void rename(std::string oldPath, std::string newPath) {
        try {
            auto modifiedOldPath = _getMountPath(oldPath);
            auto modifiedNewPath = _getMountPath(newPath);

            if (std::rename(modifiedOldPath.c_str(), modifiedNewPath.c_str()) != 0) {
                throw std::runtime_error("Failed to rename: " + oldPath + " to " + newPath);
            }

        } catch (const std::exception& e) {
            std::cerr << "Error renaming: " << e.what() << std::endl;
        }
    }
    
    static void writeFile(std::string path, std::string data) {
        try {
            
            auto modifiedPath = _getMountPath(path);

            std::ofstream outputFile(modifiedPath, std::ios::out | std::ios::binary);

            if (!outputFile.is_open()) {
                throw std::runtime_error("Failed to open file for writing: " + path);
            }

            // Write the data to the file
            outputFile << data;

            outputFile.close();

            std::cout << "File written: " << path << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error writing file: " << e.what() << std::endl;
        }
    }
    
    static std::string readFile(std::string path) {
        try {
            auto modifiedPath = _getMountPath(path);

            std::ifstream inputFile(modifiedPath, std::ios::in | std::ios::binary);

            if (!inputFile.is_open()) {
                throw std::runtime_error("Failed to open file for reading: " + path);
            }

            // Read the file content into a string
            std::ostringstream contentStream;
            contentStream << inputFile.rdbuf();
            std::string content = contentStream.str();

            inputFile.close();

            return content;
        } catch (const std::exception& e) {
            std::cerr << "Error reading file: " << e.what() << std::endl;
            return ""; // Return an empty string in case of error
        }
    }
    
    static void remove(const std::string& path) {
        try {
            auto modifiedPath = _getMountPath(path);

            if (fs::exists(modifiedPath)) {
                fs::remove(modifiedPath);
                std::cout << "File or directory removed: " << path << std::endl;
            } else {
                throw std::invalid_argument("File or directory does not exist: " + path);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error removing file or directory: " << e.what() << std::endl;
        }
    }
    
    static std::string analyzePath(const std::string& path) {
        std::map<std::string, bool> result;

        try {
            auto modifiedPath = _getMountPath(path);

            if (fs::exists(modifiedPath)) {
                result["exists"] = true;

                if (fs::is_regular_file(modifiedPath)) {
                    result["isFile"] = true;
                    result["isDirectory"] = false;
                    std::cout << "Path is a regular file: " << path << std::endl;
                } else if (fs::is_directory(modifiedPath)) {
                    result["isFile"] = false;
                    result["isDirectory"] = true;
                    std::cout << "Path is a directory: " << path << std::endl;
                } else {
                    result["isFile"] = false;
                    result["isDirectory"] = false;
                    std::cout << "Path is neither a regular file nor a directory: " << path << std::endl;
                }
            } else {
                result["exists"] = false;
                result["isFile"] = false;
                result["isDirectory"] = false;
                std::cout << "Path does not exist: " << path << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error analyzing path: " << e.what() << std::endl;
        }

        return _mapToJsonString(result);
    }
    
private:

    
    static std::string _mapToJsonString(const std::map<std::string, bool>& inputMap) {
            std::stringstream ss;
            ss << "{";
            bool firstEntry = true;

            for (const auto& [key, value] : inputMap) {
                if (!firstEntry) {
                    ss << ",";
                } else {
                    firstEntry = false;
                }

                ss << "\"" << key << "\":" << (value ? "true" : "false");
            }

            ss << "}";
            return ss.str();
        }
};
