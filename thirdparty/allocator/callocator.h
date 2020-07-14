#ifndef CALLOCATOR_H
#define CALLOCATOR_H

#include "allocator.h"

#include <list>

namespace mtrebi
{

class CAllocator : public Allocator {
public:
	CAllocator();

	virtual ~CAllocator();

	virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;

	virtual void Free(void* ptr) override;

	virtual void Init() override;
	
	void Reset();

protected:
    std::list<std::pair<void*, std::size_t>> m_usedBlocks;
};

}

#endif /* CALLOCATOR_H */

