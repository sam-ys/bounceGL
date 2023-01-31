#ifndef _CALC_MEM_hpp
#define _CALC_MEM_hpp

#include <cstring>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/syscall.h>

namespace calc {

    template <typename T>
    T* genmap(::size_t size, ::size_t* out /* [out] */)
    {
        static const ::size_t pagesize = getpagesize();

        // Maybe expand requested size to the correct size, which is a multiple of the page size
        if ((size % pagesize) != 0) {
            size = size + (pagesize - (size % pagesize));
        }

        ::size_t nbytes = size * sizeof(T);
        if (out != nullptr) {
            *out = size;
        }

        // Create anonymous file that resides in memory and set its size
        int fd;
        if ((fd = syscall(SYS_memfd_create, "anonymous", MFD_CLOEXEC)) == -1) {
            return nullptr;
        }

        // Set file size equal to buffer size
        if (::ftruncate(fd, nbytes) == -1)
        {
            ::close(fd);
            return nullptr;
        }

        void* pbuff = ::mmap(nullptr, nbytes * 2, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        // Map first page of memory
        ::mmap(pbuff,                              nbytes, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
        // Map second page of memory
        ::mmap(static_cast<char*>(pbuff) + nbytes, nbytes, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
        // Return buffer
        return (::close(fd), static_cast<T*>(pbuff));
    }

    template <typename T>
    T* genmap(::size_t size /* [in/out] */)
    {
        return genmap<T>(size, &size);
    }

    template <typename T>
    void delmap(void* target, ::size_t size)
    {
        ::size_t nbytes = size * sizeof(T);
        ::munmap(target, nbytes);
    }

    template <typename T>
    void movmap(void* target, void* source, ::size_t size)
    {
        ::size_t nbytes = size * sizeof(T);
        ::memcpy(target, source, nbytes);
    }
}

#endif
