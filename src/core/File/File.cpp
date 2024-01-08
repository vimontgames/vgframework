#include "core/Precomp.h"
#include "file.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Buffer.h"
#include "core/string/string.h"

using namespace std;

namespace vg::core::io
{
    //--------------------------------------------------------------------------------------
    bool getLastWriteTime(const string _file, FileAccessTime * _lastWrite)
    {
        bool succeeded = false;

        #if VG_WINDOWS
        WIN32_FILE_ATTRIBUTE_DATA fileAttributeData;

        succeeded = ::GetFileAttributesExA(_file.c_str(), GetFileExInfoStandard, &fileAttributeData);

        if (succeeded)
            *_lastWrite = ((FileAccessTime)(fileAttributeData.ftLastWriteTime.dwHighDateTime) << 32) | fileAttributeData.ftLastWriteTime.dwLowDateTime;
        else
            *_lastWrite = invalidFileTime;

        #elif
        VG_STATIC_ASSERT_NOT_IMPLEMENTED();
        #endif

        return succeeded;
    }

    //--------------------------------------------------------------------------------------
    bool setLastWriteTime(const string & _file, FileAccessTime _lastWrite)
    {
        bool succeeded = false;

        #if VG_WINDOWS
        const DWORD fileAttributes = ::GetFileAttributesA(_file.c_str());

        if (DWORD(-1) != fileAttributes)
            ::SetFileAttributesA(_file.c_str(), fileAttributes & ~FILE_ATTRIBUTE_READONLY);

        const HANDLE file = ::CreateFileA(_file.c_str(), FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (INVALID_HANDLE_VALUE != file)
        {
            FILETIME creation, lastAccess, lastWrite;
            if (::GetFileTime(file, &creation, &lastAccess, &lastWrite))
            {
                FILETIME timeStamp;
                         timeStamp.dwHighDateTime = DWORD(_lastWrite >> 32);
                         timeStamp.dwLowDateTime = DWORD(_lastWrite);

                succeeded = ::SetFileTime(file, &creation, &lastAccess, &timeStamp);
            }

            ::CloseHandle(file);
        }

        if (DWORD(-1) != fileAttributes)
            ::SetFileAttributesA(_file.c_str(), fileAttributes);

        #else
        VG_STATIC_ASSERT_NOT_IMPLEMENTED();
        #endif

        return succeeded;
    }

    //--------------------------------------------------------------------------------------
    bool readFile(const string & _file, string & _out, bool _mustExist)
    {
        Buffer buffer;

        if (readFile(_file, buffer, _mustExist))
        {
            _out = string((char*)buffer.data(), buffer.size());

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool readFile(const string & _file, Buffer & _out, bool _mustExist)
    {
        ifstream file(_file.c_str(), ios::in | ios::binary | ios::ate);

        if (file.is_open())
        {
            ifstream::pos_type fileSize = file.tellg();
            if (fileSize < 0)
            {
                file.close();
            }
            else
            {
                file.seekg(0, ios::beg);
                _out.resize(fileSize);
                file.read((char*)_out.data(), fileSize);
                file.close();

                return true;
            }
        }

        VG_ASSERT(!_mustExist || file.is_open(), "Could not read file \"%s\"", _file.c_str());
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool writeFile(const string & _file, const string & _in, bool _mustExist)
    {
        return writeFile(_file, Buffer((const u8*)_in.c_str(), _in.size()), _mustExist);
    }

    //--------------------------------------------------------------------------------------
    bool writeFile(const string & _file, const Buffer & _in, bool _mustExist)
    {
        const auto dir = getFileDir(_file);

        if (createPath(getFileDir(_file)))
        {
            ofstream file(getFilePath(_file.c_str()), ios::out | ios::binary);

            if (file.is_open())
            {
                file.write((const char*)_in.data(), _in.size());
                file.close();

                return true;
            }            
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    string getFilePath(const string _file)
    {
        char fullpath[1024];
        ::GetFullPathNameA((_file).c_str(), (DWORD)countof(fullpath), fullpath, nullptr);
        return fullpath;
    }

    //--------------------------------------------------------------------------------------
    bool openFileWrite(const string & _file, FileHandle & _fileHandle, bool _append)
    {
        const auto dir = getFileDir(_file);

        if (createPath(getFileDir(_file)))
        {
            #if VG_WINDOWS
            const DWORD creation = _append ? OPEN_ALWAYS : CREATE_ALWAYS;
            const string fullpath = getFilePath(_file);

            HANDLE handle = CreateFileA(fullpath.c_str(), GENERIC_WRITE, 0, nullptr, creation, 0, nullptr);

            if (INVALID_HANDLE_VALUE != handle)
            {
                if (_append)
                    SetFilePointer(handle, 0, nullptr, FILE_END);
                _fileHandle = (FileHandle)handle;
            }
            else
                VG_DEBUGPRINT("[IO] Could not open file \"%s\" for write\n", _file.c_str());

            return handle != INVALID_HANDLE_VALUE;
            #else
            VG_STATIC_ASSERT_NOT_IMPLEMENTED();
            #endif
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool closeFile(FileHandle & _fileHandle)
    {
        #if VG_WINDOWS
        if (invalidFileHandle != _fileHandle)
            return FALSE != CloseHandle((HANDLE)_fileHandle);
        else
            return false;
        #elif
        VG_STATIC_ASSERT_NOT_IMPLEMENTED();
        #endif
    }

    //--------------------------------------------------------------------------------------
    string getFileDir(const string & _file)
    {
        size_t found = _file.find_last_of("/\\");
        return _file.substr(0, found);
    }

    //--------------------------------------------------------------------------------------
    string getFileName(const string & _file)
    {
        auto found = _file.find_last_of('/');
        if (string::npos != found)
        {
            string name = _file.substr(found+1);
            return name;
        }
        else
            return _file;
    }

    //--------------------------------------------------------------------------------------
    string getFileNameWithoutExt(const string & _file)
    {
        string filename = getFileName(_file);
        auto dot = filename.find_last_of('.');
        if (string::npos != dot)
        {
            string name = filename.substr(0, dot);
            return name;
        }
        else
            return filename;
    }

    //--------------------------------------------------------------------------------------
    string addExtensionIfNotPresent(const string & _file, const string & _ext)
    {
        size_t findExt = _file.find_last_of(_ext);
        if (findExt != _file.length() - 1)
            return _file + _ext;
        else
            return _file;
    }

    //--------------------------------------------------------------------------------------
    string addExtensionIfNotPresent(const string & _file, const vector<string> _ext)
    {
        VG_ASSERT(_ext.size() > 0);

        bool extensionFound = false;
        for (uint i = 0; i < _ext.size(); ++i)
        {
            size_t findExt = _file.find_last_of(_ext[i]); 
            if (findExt == _file.length() - 1)
            {
                extensionFound = true;
                break;
            }
        }

        if (!extensionFound && _ext.size() > 0)
        {
            string newFilepath = core::fmt::sprintf("%s%s", _file.c_str(), _ext[0].c_str());
            return newFilepath;
        }

        return _file;
    }

    //--------------------------------------------------------------------------------------
    string getCookedPath(const string & _file)
    {
        return "cache/" + _file + ".bin";
    }

    //--------------------------------------------------------------------------------------
    string findAndReplace(const string & _string, const string & _find, const std::string & _replace)
    {
        string res;

        const size_t length = _find.size();
        size_t pos;
        size_t from = 0;
        
        while (string::npos != (pos = _string.find(_find, from))) 
        {
            res.append(_string, from, pos - from);
            res.append(_replace);
            from = pos + length;
        }
        res.append(_string, from, string::npos);

        return res;
    }

    //--------------------------------------------------------------------------------------
    string cleanPath(const string & _file)
    {
        return findAndReplace(_file, "\\", "/");
    }

    //--------------------------------------------------------------------------------------
    string getCurrentWorkingDirectory()
    {
        const string syscwd = (std::filesystem::current_path()).string();
        const string path = cleanPath(syscwd);
        return path;
    }

    static string root = "";

    //--------------------------------------------------------------------------------------
    void initRootDirectory()
    {
        getRootDirectory();
    }

    //--------------------------------------------------------------------------------------
    string getRootDirectory()
    {
        if (root.empty())
            root = getCurrentWorkingDirectory();
        return root;
    }

    //--------------------------------------------------------------------------------------
    void setCurrentWorkingDirectory(const string & _path)
    {
        std::filesystem::current_path(_path);
    }

    //--------------------------------------------------------------------------------------
    string getRelativePath(const string & _file)
    {
        auto path = cleanPath(_file);
        const auto cwd = getRootDirectory();

        const auto beginOffset = path.find(cwd);
        if (string::npos != beginOffset)
            path = path.substr(cwd.length()+1, path.length() - cwd.length()-1);

        return path;
    }

    //--------------------------------------------------------------------------------------
    bool exists(const string & _file)
    {
        #if VG_WINDOWS
        return INVALID_FILE_ATTRIBUTES != GetFileAttributesA(_file.c_str());
        #elif
        VG_STATIC_ASSERT_NOT_IMPLEMENTED();
        #endif
    }

    //--------------------------------------------------------------------------------------
    bool createPath(const string & _dir)
    {
        string directory = _dir;

        auto slash = directory.find_first_of("/\\");
        
        while(string::npos != slash)
        {
            string folder = directory.substr(0, slash);
            if (createFolder(folder))
                slash = directory.find_first_of("/\\", slash+1);
            else
                return false;
        }

        createFolder(_dir);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool createFolder(const string & _dir)
    {
        #if VG_WINDOWS
        if (!CreateDirectoryA(_dir.c_str(), nullptr))
        {
            const auto err = GetLastError();
            if (ERROR_ALREADY_EXISTS != err)
                return false;
        }
        return true;
        #elif
        VG_STATIC_ASSERT_NOT_IMPLEMENTED();
        #endif
    }
}