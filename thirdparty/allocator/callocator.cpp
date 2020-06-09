#include "callocator.h"

#include <stdlib.h>     /* malloc, free */
#include <algorithm>

CAllocator::CAllocator(): Allocator(0)
{
}

void CAllocator::Init()
{
    m_used = m_peak = m_totalSize = 0;
    m_usedBlocks.clear();
}

CAllocator::~CAllocator()
{
    m_usedBlocks.clear();
}

void* CAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
    void* memory = malloc(size);

    m_usedBlocks.push_back(std::make_pair(memory, size));
    m_used += size;
    m_peak = std::max(m_peak, m_used);

    m_totalSize++;

	return memory;
}

void CAllocator::Free(void* ptr)
{
    auto iter = m_usedBlocks.begin();

    while (iter != m_usedBlocks.end())
    {
        auto block = *iter;

        if (block.first == ptr)
        {
            m_used -= block.second;
            m_usedBlocks.erase(iter++);

            m_totalSize--;

            break;
        }

        ++iter;
    }

	free(ptr);
}


