#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef> // size_t
#include <mutex>

namespace mtrebi
{

class Allocator {
public:
    Allocator(const std::size_t totalSize);

    virtual ~Allocator();

    virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) = 0;

    virtual void Free(void* ptr) = 0;

    virtual void Init() = 0;

    std::size_t GetUsed();
    std::size_t GetPeak();
    std::size_t GetTotal();

protected:
    std::size_t m_totalSize;
    std::size_t m_used;
    std::size_t m_peak;

    std::mutex _mutex;
};

}

#endif /* ALLOCATOR_H */

