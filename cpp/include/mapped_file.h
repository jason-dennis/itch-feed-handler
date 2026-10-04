//
// Created by denni on 10/3/2026.
//

#ifndef ITCHFEEDHANDLER_MAPPED_FILE_H
#define ITCHFEEDHANDLER_MAPPED_FILE_H
#include <string>
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <sstream>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#endif


class MappedFile {
private:
    const uint8_t* data_{nullptr};
    size_t size_{};
public:
    explicit MappedFile(const char* path){
#ifdef _WIN32
        auto file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ,
            nullptr, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr
            );
        if (file == INVALID_HANDLE_VALUE) {
            auto error = GetLastError();
            std::ostringstream msg;
            msg << "cannot open "<< path <<": CreateFileA failed with error "<<error;
            throw std::runtime_error(msg.str());
        }
        LARGE_INTEGER file_size;
        if (!GetFileSizeEx(file, &file_size)) {
            auto error = GetLastError();
            CloseHandle(file);
            std::ostringstream msg;
            msg << "GetFileSizeEx failed for " <<path << " with error " << error;
            throw std::runtime_error(msg.str());
        }
        if (file_size.QuadPart == 0) {
            CloseHandle(file);
            return;
        }
        auto mapping = CreateFileMappingA(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (mapping == nullptr) {
            auto error = GetLastError();
            CloseHandle(file);
            std::ostringstream msg;
            msg << "CreateFileMappingA failed for " <<path << " with error " << error;
            throw std::runtime_error(msg.str());
        }

        auto view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
        if (view == nullptr) {
            auto error = GetLastError();
            CloseHandle(mapping);
            CloseHandle(file);
            std::ostringstream msg;
            msg << "MapViewOfFile failed for " <<path << " with error " << error;
            throw std::runtime_error(msg.str());
        }
        CloseHandle(mapping);
        CloseHandle(file);

        data_ = static_cast<const uint8_t*> (view);
        size_ = static_cast<size_t>(file_size.QuadPart);
#else
        int fd = open(path, O_RDONLY);
        if(fd < 0){
            auto error = errno;
            std::ostringstream msg;
            msg << "open failed for " <<path << " with error " << strerror(error);
            throw std::runtime_error(msg.str());
        }

        struct stat st;
        if(fstat(fd, &st) < 0){
            auto error = errno;
            std::ostringstream msg;
            msg << "fstat failed for " <<path << " with error " << strerror(error);
            close(fd);
            throw std::runtime_error(msg.str());
        }
        if (st.st_size == 0) {
            close(fd);
            return;
        }

        size_ = static_cast<size_t>(st.st_size);

        int flags = MAP_PRIVATE;
        #ifdef MAP_POPULATE
                flags |= MAP_POPULATE;
        #endif
        void* ptr = mmap(nullptr, size_, PROT_READ, flags, fd, 0);
        if(ptr == MAP_FAILED){
            auto error = errno;
            std::ostringstream msg;
            msg << "mmap failed for " <<path << " with error " << strerror(error);
            close(fd);
            throw std::runtime_error(msg.str());
        }
        close(fd);
        data_ = static_cast<const uint8_t*>(ptr);
#endif
    }
    ~MappedFile() {
#ifdef _WIN32
        if (data_ != nullptr) {
            UnmapViewOfFile(data_);
        }
#else
        if (data_ != nullptr) {
            munmap(const_cast<uint8_t*>(data_), size_);
        }
#endif
    }

    const uint8_t* data() const {return data_;}
    size_t size() const {return size_;}
    MappedFile(const MappedFile&) = delete;
    MappedFile& operator=(const MappedFile&) = delete;
};
#endif //ITCHFEEDHANDLER_MAPPED_FILE_H
