#include "allocator.h"

namespace mtrebi
{

Allocator::Allocator(const std::size_t totalSize){
    m_totalSize = totalSize;
    m_used = 0;
}

Allocator::~Allocator(){
    m_totalSize = 0;
}

std::size_t Allocator::GetUsed()
{
    return m_used;
}
std::size_t Allocator::GetPeak()
{
    return m_peak;
}
std::size_t Allocator::GetTotal()
{
    return m_totalSize;
}

}
