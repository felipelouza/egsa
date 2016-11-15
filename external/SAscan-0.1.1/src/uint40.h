/******************************************************************************
 * uint40.h
 * Class representing a 40-bit unsigned integer encoded in five bytes.
 *
 ******************************************************************************
 * Copyright (C) 2012 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef __UINT40_H_INCLUDED
#define __UINT40_H_INCLUDED

#include <inttypes.h>
#include <stdint.h>
#include <cassert>
#include <iostream>
#include <limits>
#include <unistd.h>

class uint40
{
private:
    uint32_t    low;
    uint8_t     high;

public:
    inline uint40()
    {
    }

    inline uint40(uint32_t l, uint8_t h)
        : low(l), high(h)
    {
    }

    inline uint40(const uint40& a)
        : low(a.low), high(a.high)
    {
    }

    inline uint40(const int& a)
        : low(a), high(0)
    {
    }

    inline uint40(const unsigned int& a)
        : low(a), high(0)
    {
    }

    inline uint40(const unsigned long& a)
        : low(a & 0xFFFFFFFF), high((a >> 32) & 0xFF)
    {
        assert( a <= 0xFFFFFFFFFFLU );
    }

    inline unsigned long ull() const
    {
        return ((unsigned long)high) << 32 | (unsigned long)low;
    }

    inline operator unsigned long() const
    {
        return ull();
    }

    // prefix increment operator
    inline uint40& operator++ ()
    {
        if (low == std::numeric_limits<uint32_t>::max())
            ++high, low = 0;
        else
            ++low;
        return *this;
    }

    // prefix decrement operator
    inline uint40& operator-- ()
    {
        if (low == 0)
            --high, low = std::numeric_limits<uint32_t>::max();
        else
            --low;
        return *this;
    }

    // addition operator
    inline uint40& operator+= (const uint40& b)
    {
        uint64_t add = low + b.low;
        low = add & 0xFFFFFFFF;
        high += b.high + ((add >> 32) & 0xFF);
        return *this;
    }

    // comparison operator
    inline bool operator== (const uint40& b) const
    {
        return (low == b.low) && (high == b.high);
    }

    // comparison operator
    inline bool operator!= (const uint40& b) const
    {
        return (low != b.low) || (high != b.high);
    }

    // comparison operator
    inline bool operator< (const uint40& b) const
    {
        return (high < b.high) || (high == b.high && low < b.low);
    }

    // comparison operator
    inline bool operator<= (const uint40& b) const
    {
        return (high < b.high) || (high == b.high && low <= b.low);
    }

    // comparison operator
    inline bool operator> (const uint40& b) const
    {
        return (high > b.high) || (high == b.high && low > b.low);
    }

    // comparison operator
    inline bool operator>= (const uint40& b) const
    {
        return (high > b.high) || (high == b.high && low >= b.low);
    }

    friend std::ostream& operator<< (std::ostream& os, const uint40& a)
    {
        return os << a.ull();
    }

} __attribute__((packed));

#endif // __UINT40_H_INCLUDED
