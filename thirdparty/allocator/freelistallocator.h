/////////////////////////////////////////////////////////////////////////////
// Name:         FreeListAllocator.h
// Description:  ...
// Author:       Mariano Trebino (https://github.com/mtrebi)
// Modified by:  Alexey Orlov (https://github.com/m110h)
// Modified:     08/08/2020
// Licence:      MIT licence
/////////////////////////////////////////////////////////////////////////////

#ifndef FREELISTALLOCATOR_H
#define FREELISTALLOCATOR_H

#include "allocator.h"
#include "singlylinkedlist.h"

namespace mtrebi
{

class FreeListAllocator : public Allocator {
public:
    enum PlacementPolicy
    {
        FIND_FIRST,
        FIND_BEST
    };

private:
    struct FreeHeader
    {
        std::size_t blockSize;
    };

    struct AllocationHeader
    {
        std::size_t blockSize;
        std::size_t padding;
    };

    typedef SinglyLinkedList<FreeHeader>::Node Node;


    void* m_start_ptr {nullptr};
    PlacementPolicy m_pPolicy;

    SinglyLinkedList<FreeHeader> m_freeList;

public:
    explicit FreeListAllocator(const std::size_t totalSize, const PlacementPolicy pPolicy = PlacementPolicy::FIND_BEST);

    FreeListAllocator(FreeListAllocator &src) = delete;
    FreeListAllocator& operator=(const FreeListAllocator& r) = delete;

    ~FreeListAllocator();

    virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) final;
    virtual void Free(void* ptr) final;
    virtual void Init() final;
	virtual void Reset() final;

private:
    void Merge(Node* prevBlock, Node* freeBlock);

    void Find(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node *& previousNode, Node *& foundNode);
    void FindBest(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node *& previousNode, Node *& foundNode);
    void FindFirst(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node *& previousNode, Node *& foundNode);
};

}

#endif /* FREELISTALLOCATOR_H */
