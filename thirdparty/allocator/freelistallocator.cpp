/////////////////////////////////////////////////////////////////////////////
// Name:         FreeListAllocator.cpp
// Description:  ...
// Author:       Mariano Trebino (https://github.com/mtrebi)
// Modified by:  Alexey Orlov (https://github.com/m110h)
// Modified:     08/08/2020
// Licence:      MIT licence
/////////////////////////////////////////////////////////////////////////////

#include "freelistallocator.h"
#include "utils.h"  /* CalculatePaddingWithHeader */

#include <new>
#include <limits>  /* limits_max */

namespace mtrebi
{

FreeListAllocator::FreeListAllocator(const std::size_t totalSize, const PlacementPolicy pPolicy): Allocator(totalSize)
{
    m_pPolicy = pPolicy;
}

FreeListAllocator::~FreeListAllocator()
{
    if (m_start_ptr != nullptr)
    {
        ::operator delete (m_start_ptr);
        m_start_ptr = nullptr;
    }
}

void FreeListAllocator::Init()
{
    if (m_start_ptr != nullptr)
    {
        ::operator delete(m_start_ptr);
        m_start_ptr = nullptr;
    }

    m_start_ptr = ::operator new(m_totalSize);

    Reset();
}

void* FreeListAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
    wxASSERT_MSG(m_start_ptr, wxT("FreeListAllocator::Allocate: allocator isn't initialized, m_start_ptr is NULL"));
    wxASSERT_MSG((size > sizeof(Node)), wxT("FreeListAllocator::Allocate: Allocation size must be bigger"));
    wxASSERT_MSG((alignment >= 8), wxT("FreeListAllocator::Allocate: Alignment must be 8 at least"));

    const std::size_t allocationHeaderSize = sizeof(FreeListAllocator::AllocationHeader);

    // Search through the free list for a free block that has enough space to allocate our data

    //
    std::size_t padding;

    // a node which we will split to take necessary memory
    Node* affectedNode {nullptr};

    // this node is placed before the affected node
    Node* previousNode {nullptr};

    Find(size, alignment, padding, previousNode, affectedNode);

    //assert (affectedNode != nullptr && "Hasn't enough memory");
    if (affectedNode == nullptr)
        return nullptr;


    // calculating the size we will take
    wxASSERT_MSG((padding >= allocationHeaderSize), wxT("FreeListAllocator::Allocate: padding < allocationHeaderSize"));

    const std::size_t alignmentPadding =  padding - allocationHeaderSize;
    const std::size_t requiredSize = size + padding;

    // calculating a rest of affected node's memory
    wxASSERT_MSG((affectedNode->data.blockSize >= requiredSize), wxT("FreeListAllocator::Allocate: affectedNode->data.blockSize < requiredSize"));
    const std::size_t rest = affectedNode->data.blockSize - requiredSize;

    // an issue with Free memory fixed here: [rest > sizeof(Node)] instead [rest > 0]
    if ( rest > sizeof(Node) )
    {
        // We have to split the block into the [data block] and a [free block] of size 'rest'

        Node* newFreeNode = reinterpret_cast<Node*>(reinterpret_cast<std::size_t>(affectedNode) + requiredSize);
        newFreeNode->data.blockSize = rest;

        m_freeList.insert(affectedNode, newFreeNode);
    }

    m_freeList.remove(previousNode, affectedNode);

    // Setup data block
    const std::size_t headerAddress = reinterpret_cast<std::size_t>(affectedNode) + alignmentPadding;
    const std::size_t dataAddress = headerAddress + allocationHeaderSize;

    FreeListAllocator::AllocationHeader* headerAddressPtr = reinterpret_cast<FreeListAllocator::AllocationHeader*>(headerAddress);
    headerAddressPtr->blockSize = requiredSize;
    headerAddressPtr->padding = alignmentPadding;

    m_used += requiredSize;
    m_peak = (m_peak < m_used) ? m_used : m_peak;

    return reinterpret_cast<void*>(dataAddress);
}

void FreeListAllocator::Find(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node *& previousNode, Node *& foundNode)
{
    switch (m_pPolicy) {
        case FIND_FIRST:
            FindFirst(size, alignment, padding, previousNode, foundNode);
            break;
        case FIND_BEST:
            FindBest(size, alignment, padding, previousNode, foundNode);
            break;
        default:
            break;
    }
}

void FreeListAllocator::FindFirst(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node *& previousNode, Node *& foundNode)
{
    Node* _it = m_freeList.head;
    Node* _itPrev {nullptr};

    while (_it != nullptr)
    {
        const std::size_t _padding = Utils::CalculatePaddingWithHeader(reinterpret_cast<std::size_t>(_it), alignment, sizeof(FreeListAllocator::AllocationHeader));
        const std::size_t _requiredSpace = size + _padding;

        if (_it->data.blockSize >= _requiredSpace)
        {
            previousNode = _itPrev;
            foundNode = _it;
            padding = _padding;

            return;
        }

        _itPrev = _it;
        _it = _it->next;
    }
}

// Iterate WHOLE list keeping a pointer to the best fit
void FreeListAllocator::FindBest(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node *& previousNode, Node *& foundNode)
{
    std::size_t smallestDiff = std::numeric_limits<std::size_t>::max();

    Node* it = m_freeList.head;
    Node* itPrev {nullptr};

    while (it != nullptr)
    {
        const std::size_t _padding = Utils::CalculatePaddingWithHeader(reinterpret_cast<std::size_t>(it), alignment, sizeof(FreeListAllocator::AllocationHeader));
        const std::size_t requiredSpace = size + _padding;

        if ( (it->data.blockSize >= requiredSpace) && ((it->data.blockSize - requiredSpace) < smallestDiff) )
        {
            smallestDiff = it->data.blockSize - requiredSpace;
            padding = _padding;

            previousNode = itPrev;
            foundNode = it;

            if (smallestDiff == 0)
                return;
        }

        itPrev = it;
        it = it->next;
    }
}

void FreeListAllocator::Free(void* ptr)
{
    wxASSERT_MSG(m_start_ptr, wxT("FreeListAllocator::Free: allocator isn't initialized, m_start_ptr is NULL"));
    wxASSERT_MSG(ptr, wxT("FreeListAllocator::Free: passed argument is NULL"));

    const std::size_t currentAddress = reinterpret_cast<std::size_t>(ptr);

    wxASSERT_MSG((currentAddress >= sizeof(FreeListAllocator::AllocationHeader)), wxT("FreeListAllocator::Free: currentAddress < sizeof(FreeListAllocator::AllocationHeader)"));
    const std::size_t headerAddress = currentAddress - sizeof(FreeListAllocator::AllocationHeader);

    const FreeListAllocator::AllocationHeader* allocationHeader = reinterpret_cast<FreeListAllocator::AllocationHeader*>(headerAddress);

    std::size_t allocationHeaderBlockSize = allocationHeader->blockSize;
    std::size_t allocationHeaderPadding = allocationHeader->padding;

    Node* freeNode = reinterpret_cast<Node*>(headerAddress);

    freeNode->data.blockSize = allocationHeaderBlockSize + allocationHeaderPadding;
    freeNode->next = nullptr;

    Node* it = m_freeList.head;
    Node* itPrev = nullptr;

    while (it != nullptr)
    {
        if (ptr < it) {
            m_freeList.insert(itPrev, freeNode);
            break;
        }
        itPrev = it;
        it = it->next;
    }

    wxASSERT_MSG((m_used >= freeNode->data.blockSize), wxT("FreeListAllocator::Free: m_used < freeNode->data.blockSize"));
    m_used -= freeNode->data.blockSize;

    // Merge contiguous nodes
    Merge(itPrev, freeNode);
}

void FreeListAllocator::Merge(Node* previousNode, Node* freeNode)
{
    if ( (freeNode->next != nullptr) && ( (reinterpret_cast<std::size_t>(freeNode) + freeNode->data.blockSize) == reinterpret_cast<std::size_t>(freeNode->next)) )
    {
        freeNode->data.blockSize += freeNode->next->data.blockSize;
        m_freeList.remove(freeNode, freeNode->next);
    }

    if ( (previousNode != nullptr) && ( (reinterpret_cast<std::size_t>(previousNode) + previousNode->data.blockSize) == reinterpret_cast<std::size_t>(freeNode)) )
    {
        previousNode->data.blockSize += freeNode->data.blockSize;
        m_freeList.remove(previousNode, freeNode);
    }
}

void FreeListAllocator::Reset()
{
    wxASSERT_MSG(m_start_ptr, wxT("FreeListAllocator::Reset: allocator isn't initialized, m_start_ptr is NULL"));

    m_used = 0;
    m_peak = 0;

    // create the first block that contains all of memory

    Node * firstNode = reinterpret_cast<Node*>(m_start_ptr);

    firstNode->data.blockSize = m_totalSize;
    firstNode->next = nullptr;

    m_freeList.head = nullptr;
    m_freeList.insert(nullptr, firstNode);
}

}
