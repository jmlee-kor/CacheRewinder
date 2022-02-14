/*
 * Copyright (c) 2020 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 1999-2008 Mark D. Hill and David A. Wood
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __MEM_RUBY_STRUCTURES_ADDRIDXTABLE_HH__
#define __MEM_RUBY_STRUCTURES_ADDRIDXTABLE_HH__

#include <iostream>
#include <unordered_map>

#include "mem/ruby/common/Address.hh"

class AddrIdxTable
{
  public:
    AddrIdxTable(int number_of_TBEs)
    : m_number_of_TBEs(number_of_TBEs),
      m_writebacks(0)
    {}

    bool isPresent(Addr addressReplaced) const;
    void allocate(Addr addressReplaced, Addr addressOrigin);
    void deallocate(Addr addressReplaced);
    void deallocateOriginal(Addr addressOrigin);
    bool
    areNSlotsAvailable(int n, Tick current_time) const
    {
        return (4/*WBB size*/ - m_writebacks - size()) >= n || m_writebacks==0;
    }

    Addr lookup(Addr addressReplaced);

    int size() const;
    void enqueWriteback();
    void dequeWriteback();

  private:
    std::unordered_map<Addr,Addr> m_map;
    int m_number_of_TBEs;
    int m_writebacks;

};

inline bool
AddrIdxTable::isPresent(Addr addressReplaced) const
{
    assert(addressReplaced == makeLineAddress(addressReplaced));
    assert(m_map.size() <= m_number_of_TBEs);
    return !!m_map.count(addressReplaced);
}

inline void
AddrIdxTable::allocate(Addr addressReplaced, Addr addressOrigin)
{
    assert(!isPresent(addressReplaced));
    assert(m_map.size() < m_number_of_TBEs);
    m_map[addressReplaced] = addressOrigin;
}

inline void
AddrIdxTable::deallocate(Addr addressReplaced)
{
    assert(isPresent(addressReplaced));
    assert(m_map.size() > 0);
    m_map.erase(addressReplaced);
}

inline void
AddrIdxTable::deallocateOriginal(Addr addressOrigin)
{
  for (auto it=m_map.begin();it!=m_map.end();it++) {
    if (it->second == addressOrigin) {
      it = m_map.erase(it);
      return;
    }
  }

  assert(false); // there is no entry for the original address
}

inline Addr
AddrIdxTable::lookup(Addr addressReplaced)
{
  if (m_map.find(addressReplaced) != m_map.end())
    return m_map[addressReplaced];
  return 0;
}

inline int
AddrIdxTable::size() const
{
  return m_map.size();
}

inline void
AddrIdxTable::enqueWriteback()
{
  m_writebacks++;
}

inline void
AddrIdxTable::dequeWriteback()
{
  m_writebacks--;
}


#endif // __MEM_RUBY_STRUCTURES_ADDRIDXTABLE_HH__
