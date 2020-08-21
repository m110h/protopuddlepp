/////////////////////////////////////////////////////////////////////////////
// Name:         Utils.h
// Description:  ...
// Author:       Mariano Trebino (https://github.com/mtrebi)
// Modified by:  Alexey Orlov (https://github.com/m110h)
// Modified:     08/08/2020
// Licence:      MIT licence
/////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_H
#define UTILS_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace mtrebi
{

class Utils {
public:
    static const std::size_t CalculatePadding(const std::size_t baseAddress, const std::size_t alignment)
    {
        const std::size_t multiplier = (baseAddress / alignment) + 1;
        const std::size_t alignedAddress = multiplier * alignment;

        wxASSERT_MSG((alignedAddress >= baseAddress), wxT("Utils::CalculatePadding: alignedAddress < baseAddress"));
        const std::size_t padding = alignedAddress - baseAddress;

        return padding;
    }

    static const std::size_t CalculatePaddingWithHeader(const std::size_t baseAddress, const std::size_t alignment, const std::size_t headerSize)
    {
        std::size_t padding = CalculatePadding(baseAddress, alignment);
        std::size_t neededSpace = headerSize;

        if (padding < neededSpace)
        {
            // Header does not fit - Calculate next aligned address that header fits
            neededSpace -= padding;

            // How many alignments I need to fit the header
            if(neededSpace % alignment > 0)
            {
                padding += alignment * ( 1 + (neededSpace / alignment) );
            }
            else
            {
                padding += alignment * ( neededSpace / alignment );
            }
        }

        return padding;
    }
};

}

#endif /* UTILS_H */
