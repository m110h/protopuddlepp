/////////////////////////////////////////////////////////////////////////////
// Name:         Allocator.h
// Description:  ...
// Author:       Mariano Trebino (https://github.com/mtrebi)
// Modified by:  Alexey Orlov (https://github.com/m110h)
// Modified:     08/08/2020
// Licence:      MIT licence
/////////////////////////////////////////////////////////////////////////////

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef> // size_t

namespace mtrebi
{

class Allocator {
public:
    explicit Allocator(const std::size_t totalSize);

    virtual ~Allocator();

    virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) = 0;
    virtual void Free(void* ptr) = 0;
    virtual void Init() = 0;
    virtual void Reset() = 0;

    std::size_t GetUsed();
    std::size_t GetPeak();
    std::size_t GetTotal();

protected:
    std::size_t m_totalSize;
    std::size_t m_used;
    std::size_t m_peak;
};

}

#endif /* ALLOCATOR_H */
