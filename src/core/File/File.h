#pragma once

#include "core/Types/Types.h"

namespace vg::core::io
{
    class Buffer;

    using FileAccessTime = u64;
    static const FileAccessTime invalidFileTime = (FileAccessTime)0;

    using FileHandle = u64;
    const FileHandle invalidFileHandle = (FileHandle)0;

    FileAccessTime getCurrentFileTime();
    bool getLastWriteTime(const string _file, FileAccessTime * _lastWrite);
    bool setLastWriteTime(const string & _file, FileAccessTime _filetime);

    bool readFile(const string & _file, string & _out, bool _mustExist = true);
    bool readFile(const string & _file, Buffer & _out, bool _mustExist = true);

    bool writeFile(const string & _file, const string & _in, bool _mustExist = false);
    bool writeFile(const string & _file, const Buffer & _in, bool _mustExist = false);

    bool exists(const string & _file);
    bool createPath(const string & _dir);
    bool createFolder(const string & _dir);
    string getFilePath(const string _file);

    bool openFileRead(const string & _file, FileHandle & _fileHandle);
    bool openFileWrite(const string & _file, FileHandle & _fileHandle, bool _append = false);
    bool closeFile(FileHandle & _fileHandle);

    bool read(FileHandle _fileHandle, void * _data, size_t _bytes);
    bool write(FileHandle _fileHandle, const void * _data, size_t _bytes);

    string getCookedPath(const string & _file);
    string getMetaPath(const string & _file);
    string getFileDir(const string & _file);
    string getFileName(const string & _file);
    string getFileNameWithoutExt(const string & _file);
    bool fileHasExtension(const string & _file, const string & _ext);
    string getFileExtension(const string & _file);
    string addExtensionIfNotPresent(const string & _file, const core::string & _ext);
    string addExtensionIfNotPresent(const string & _file, const core::vector<core::string> _ext);

    void initRootDirectory();
    string getRootDirectory();

    void setCurrentWorkingDirectory(const core::string & _path);
    string getCurrentWorkingDirectory();

    string getRelativePath(const string & _file);
    string cleanPath(const string & _file);
    string findAndReplace(const string & _string, const string & _find, const std::string & _replace);

    struct FileInfo
    {
        string name;
        bool isFolder;
    };

    core::vector<FileInfo> getFilesInFolder(const string & _folder);
}



