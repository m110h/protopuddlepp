#include "allocator.h"

namespace mtrebi
{

Allocator::Allocator(const std::size_t totalSize): m_totalSize(totalSize), m_used(0), m_peak(0) {}

Allocator::~Allocator(){
    m_totalSize = 0;
}

std::size_t Allocator::GetUsed()
{
    std::lock_guard<std::mutex> guard(_mutex);
    return m_used;
}
std::size_t Allocator::GetPeak()
{
    std::lock_guard<std::mutex> guard(_mutex);
    return m_peak;
}
std::size_t Allocator::GetTotal()
{
    std::lock_guard<std::mutex> guard(_mutex);
    return m_totalSize;
}

}