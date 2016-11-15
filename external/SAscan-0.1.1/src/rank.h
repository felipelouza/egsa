//==============================================================================
// Data structure answering general rank queries over byte alphabet based
// on the rank data structure from the bwtdisk implementation
// (http://people.unipmn.it/manzini/bwtdisk/) of the algorithm from:
//
//     Paolo Ferragina, Travis Gagie, Giovanni Manzini:
//     Lightweight Data Indexing and Compression in External Memory.
//     Algorithmica 63(3): 707-730 (2012)
//
// Our key modification is applying two new techniques:
//
//   * alphabet partitioning
//
//     Jérémy Barbay, Travis Gagie, Gonzalo Navarro, Yakov Nekrich:
//     Alphabet Partitioning for Compressed Rank/Select and Applications.
//     Proc. ISAAC 2010.
//
//   * fixed block boosting
//
//     Juha Kärkkäinen, Simon J. Puglisi:
//     Fixed Block Compression Boosting in FM-Indexes.
//     Proc. SPIRE 2011.
//==============================================================================

#ifndef __RANK4N_H_INCLUDED
#define __RANK4N_H_INCLUDED

#include <algorithm>
#include <vector>

#include "utils.h"


template<unsigned k_sblock_size_log = 24, unsigned k_cblock_size_log = 18>
class rank4n {
  private:
    static const unsigned k_cblock_size;
    static const unsigned k_cblocks_in_sblock_log;
    static const unsigned k_cblocks_in_sblock;
    static const unsigned k_cblocks_in_sblock_mask;
    static const unsigned k_sblock_size;
    static const unsigned k_sblock_size_mask;

    static const unsigned k_char_type_freq =    0x01;
    static const unsigned k_char_type_rare =    0x02;
    static const unsigned k_char_type_missing = 0x03;

    long m_length;   // length of original sequence
    long n_cblocks;  // number of context blocks
    long n_sblocks;  // number of super block

    long *m_cblock_header;
    long *m_sblock_header;
    unsigned char *m_cblock_mapping;

    unsigned *m_freq_trunk;
    unsigned *m_rare_trunk;

  public:
    long *m_count; // symbol counts

  public:
    rank4n(unsigned char *text, long length) {
      m_length = length;

      // Compute the number of blocks.
      n_cblocks = (m_length + k_cblock_size - 1) / k_cblock_size;
      n_sblocks = (n_cblocks + k_cblocks_in_sblock - 1) / k_cblocks_in_sblock;

      // Allocate and initialize symbol counts.
      m_count = (long *)malloc(256L * sizeof(long));
      std::fill(m_count, m_count + 256, 0L);

      if (!m_length) return;

      //------------------------------------------------------------------------
      // STEP 1: compute rare trunk size, m_cblock_header and m_cblock_mapping.
      //------------------------------------------------------------------------
      m_cblock_header = (long *)malloc(n_cblocks * sizeof(long));
      m_cblock_mapping = (unsigned char *)malloc(n_cblocks * 512L);
      long rare_trunk_size = 0;

      bool isfreq[256];
      std::vector<std::pair<uint32_t, unsigned char> > sorted_chars;
      std::vector<unsigned char> freq_chars;
      std::vector<unsigned char> rare_chars;
      unsigned cblock_count[256];
      for (long cblock_id = 0; cblock_id < n_cblocks; ++cblock_id) {
        long cblock_beg = cblock_id << k_cblock_size_log;
        long cblock_end = cblock_beg + k_cblock_size;

        // Compute symbol counts inside a block.
        std::fill(cblock_count, cblock_count + 256, 0L);
        for (long j = cblock_beg; j < cblock_end; ++j) {
          unsigned char c = (j < m_length ? text[j] : 0);
          ++cblock_count[c];
        }

        // Sort symbol counts by frequencies.
        sorted_chars.clear();
        for (long j = 0; j < 256; ++j)
          if (cblock_count[j])
            sorted_chars.push_back(std::make_pair(cblock_count[j], j));
        std::sort(sorted_chars.begin(), sorted_chars.end());

        // Separate (at most, due to rounding of freq_cnt)
        // about 3% of rarest symbols.
        unsigned rare_cnt = 0L, rare_sum = 0L;
        while (rare_cnt < sorted_chars.size() &&
            16L * (rare_sum + sorted_chars[rare_cnt].first) <= k_cblock_size)
          rare_sum += sorted_chars[rare_cnt++].first;

        // Compute freq_cnt. Then round up freq_cnt + 1 (+1 is
        // for rare char marker) to the smallest power of two.
        // Note: rare_cnt > 0, so after rounding freq_cnt <= 256.
        unsigned freq_cnt = sorted_chars.size() - rare_cnt;
        unsigned freq_cnt_log = utils::log2ceil(freq_cnt + 1);
        freq_cnt = (1 << freq_cnt_log);

        // Recompute rare_cnt (note the +1).
        rare_cnt = 0;
        if (sorted_chars.size() + 1 > freq_cnt)
          rare_cnt = sorted_chars.size() + 1 - freq_cnt;

        // Compute freq and rare chars.
        rare_chars.clear();
        freq_chars.clear();
        for (unsigned i = 0; i < rare_cnt; ++i)
          rare_chars.push_back(sorted_chars[i].second);
        for (unsigned i = rare_cnt; i < sorted_chars.size(); ++i)
          freq_chars.push_back(sorted_chars[i].second);
 
        // If there are rare symbols, round up
        // rare_cnt to the smallest power of two.
        unsigned rare_cnt_log = 0;
        if (rare_cnt) {
          rare_cnt_log = utils::log2ceil(rare_cnt);
          rare_cnt = (1 << rare_cnt_log);
        }

        // Store log2 of rare_cnt and freq_cnt into block header.
        m_cblock_header[cblock_id] = freq_cnt_log;
        m_cblock_header[cblock_id] |= (rare_cnt_log << 8);

        // Compute and store symbols mapping.
        std::sort(freq_chars.begin(), freq_chars.end());
        std::sort(rare_chars.begin(), rare_chars.end());
        std::fill(isfreq, isfreq + 256, false);
        for (unsigned c = 0; c < 256; ++c)
          m_cblock_mapping[2 * (c * n_cblocks + cblock_id)] = k_char_type_missing;
        for (unsigned i = 0; i < freq_chars.size(); ++i) {
          unsigned char c = freq_chars[i];
          isfreq[c] = true;
          m_cblock_mapping[2 * (c * n_cblocks + cblock_id) + 1] = i;
          m_cblock_mapping[2 * (c * n_cblocks + cblock_id)] = k_char_type_freq;
        }
        for (unsigned i = 0; i < rare_chars.size(); ++i) {
          unsigned char c = rare_chars[i];
          m_cblock_mapping[2 * (c * n_cblocks + cblock_id) + 1] = i;
          m_cblock_mapping[2 * (c * n_cblocks + cblock_id)] = k_char_type_rare;
        }

        // Update rare_trunk_size.
        if (rare_cnt) {
          unsigned nofreq_cnt = 0L;
          for (long i = cblock_beg; i < cblock_end; ++i) {
            unsigned char c = (i < m_length ? text[i] : 0);
            if (!isfreq[c]) ++nofreq_cnt;
          }
          long rare_micro_blocks = 1 + (nofreq_cnt + rare_cnt - 1) / rare_cnt;
          rare_trunk_size += rare_micro_blocks * rare_cnt;
        }
      }

      m_rare_trunk = (unsigned *)malloc(rare_trunk_size * sizeof(unsigned));
      m_freq_trunk = (unsigned *)malloc(n_cblocks * k_cblock_size * sizeof(unsigned));
      m_sblock_header = (long *)malloc(n_sblocks * 256L * sizeof(long));

      //------------------------------------------------------------------------
      // STEP 2: compute sb_rank, freq_trunk and rare_trunk.
      //------------------------------------------------------------------------
      unsigned char freq_map[256];
      unsigned char rare_map[256];
      long r_filled = 0; // for rare trunk
      for (long cblock_id = 0; cblock_id < n_cblocks; ++cblock_id) {
        long r_ptr = r_filled; // where to write next
        m_cblock_header[cblock_id] |= (r_filled << 16);

        long cblock_beg = cblock_id << k_cblock_size_log;
        long cblock_end = cblock_beg + k_cblock_size;

        //----------------------------------------------------------------------
        // Process cblock [cblock_beg..cblock_end).
        //----------------------------------------------------------------------

        long freq_cnt_log = (m_cblock_header[cblock_id] & 255L);
        long rare_cnt_log = ((m_cblock_header[cblock_id] >> 8) & 255L);
        long freq_cnt = (1L << freq_cnt_log);
        long freq_cnt_mask = freq_cnt - 1;
        long rare_cnt = (1L << rare_cnt_log);
        long rare_cnt_mask = rare_cnt - 1;

        freq_chars.clear();
        rare_chars.clear();
        std::fill(isfreq, isfreq + 256, false);
        for (unsigned j = 0; j < 256; ++j) {
          unsigned char type = m_cblock_mapping[2 * (j * n_cblocks + cblock_id)];
          if (type == k_char_type_freq) {
            isfreq[j] = true;
            freq_chars.push_back(j);
            freq_map[j] = m_cblock_mapping[2 * (j * n_cblocks + cblock_id) + 1];
          } else if (type == k_char_type_rare) {
            rare_chars.push_back(j);
            rare_map[j] = m_cblock_mapping[2 * (j * n_cblocks + cblock_id) + 1];
            freq_map[j] = freq_cnt - 1;
          }
        }

        if (rare_chars.empty()) {
          rare_cnt_log = 0;
          rare_cnt = 0;
        }

        // Compute ranks at superblock boundary.
        long sblock_id = (cblock_beg >> k_sblock_size_log);
        if (!(cblock_beg & k_sblock_size_mask)) {
          long sblock_ptr = (sblock_id << 8);
          for (unsigned i = 0; i < 256; ++i)
            m_sblock_header[sblock_ptr++] = m_count[i];
        }

        // Compute freq and rare trunk of current block.
        long nofreq_cnt = 0;
        for (long i = cblock_beg; i < cblock_end; ++i) {
          unsigned char c = (i < m_length ? text[i] : 0);

          //--------------------------------------------------------------------
          // Invariant: for any symbol a, c_rank[a] = number of occurrence of
          // symbols a in prefix text[0..i).
          //--------------------------------------------------------------------
          // Invariant: for any symbol a, sb_rank[(sb_id << 8) + a] = the
          // number of occurrence of a in a prefix of text up to the closest
          // superblock boundary. All ranks we store in the trunk are relative
          // to this boundary thus we compute them as
          // c_rank[a] - sb_rank[(sb_id << 8) + a].
          //--------------------------------------------------------------------

          // Compute ranks at the freq microblock boundary.
          if (!(i & freq_cnt_mask)) {
            m_freq_trunk[i + freq_cnt - 1] = (nofreq_cnt << 8);
            for (long j = 0; j + 1 < freq_cnt; ++j) {
              unsigned char ch = (j < (long)freq_chars.size() ? freq_chars[j] : 0);
              long local_rank = m_count[ch] - m_sblock_header[(sblock_id << 8) + ch];
              m_freq_trunk[i + j] = (local_rank << 8);
            }
          }

          // Store freq symbol mapping.
          m_freq_trunk[i] |= freq_map[c];

          // Handle rare symbol.
          if (!isfreq[c]) {
            // Compute ranks at the rare microblock boundary.
            if (!(nofreq_cnt & rare_cnt_mask)) {
              for (long j = 0; j < rare_cnt; ++j) {
                unsigned char ch = (j < (long)rare_chars.size() ? rare_chars[j] : 0);
                long local_rank = m_count[ch] - m_sblock_header[(sblock_id << 8) + ch];
                m_rare_trunk[r_filled++] = (local_rank << 8);
              }
            }

            // Store rare symbol mapping.
            m_rare_trunk[r_ptr++] |= rare_map[c];
            ++nofreq_cnt;
          }

          ++m_count[c];
        }

        for (long j = 0; j < rare_cnt; ++j) {
          unsigned char ch = (j < (long)rare_chars.size() ? rare_chars[j] : 0);
          long local_rank = m_count[ch] - m_sblock_header[(sblock_id << 8) + ch];
          m_rare_trunk[r_filled++] = (local_rank << 8);
        }
      }

      m_count[0] -= n_cblocks * k_cblock_size - m_length;
    }

    inline long rank(long i, unsigned char c) {
      if (i <= 0) return 0L;
      else if (i >= m_length) return m_count[c];

      long cblock_id = (i >> k_cblock_size_log);
      long sblock_id = (i >> k_sblock_size_log);
      long sblock_rank = m_sblock_header[(sblock_id << 8) + c];

      unsigned char type = m_cblock_mapping[2 * (c * n_cblocks + cblock_id)];
      unsigned char c_map = m_cblock_mapping[2 * (c * n_cblocks + cblock_id) + 1];

      long freq_cnt_bits = (m_cblock_header[cblock_id] & 255L);
      long rare_cnt_bits = ((m_cblock_header[cblock_id] >> 8) & 255L);
      long block_id = (i >> freq_cnt_bits);

      if (type == k_char_type_freq) {
        // Case 1 (fastest): symbol c was frequent in the context block.
        // Answer a query using frequent trunk.
        long block_rank = m_freq_trunk[(block_id << freq_cnt_bits) + c_map] >> 8;
        long extra = 0;
        for (long j = (block_id << freq_cnt_bits); j < i; ++j)
          if ((m_freq_trunk[j] & 255) == c_map) ++extra;

        return sblock_rank + block_rank + extra;
      } else if (type == k_char_type_rare) {
        // Case 2: symbol c was rare inside the context block.
        // Compute new_i.
        long rare_trunk_ptr = (m_cblock_header[cblock_id] >> 16);
        long new_i = m_freq_trunk[((block_id + 1) << freq_cnt_bits) - 1] >> 8;
        for (long j = (block_id << freq_cnt_bits); j < i; ++j)
          if ((m_freq_trunk[j] & 255) + 1 == (1U << freq_cnt_bits)) ++new_i;
      
        // Answer a query on rare trunk.
        long rare_block_id = (new_i >> rare_cnt_bits);
        long block_rank = m_rare_trunk[rare_trunk_ptr +
          (rare_block_id << rare_cnt_bits) + c_map] >> 8;
        long extra = 0;
        for (long j = (rare_block_id << rare_cnt_bits); j < new_i; ++j)
          if ((m_rare_trunk[rare_trunk_ptr + j] & 255) == c_map) ++extra;

        return sblock_rank + block_rank + extra;
      } else {
        // Case 3: symbol c does not occur in the context block.
        // Find the first cblock where c occurrs.
        while (cblock_id < n_cblocks && (cblock_id & k_cblocks_in_sblock_mask) &&
            m_cblock_mapping[2 * (c * n_cblocks + cblock_id)] == k_char_type_missing)
          ++cblock_id;

        if (cblock_id == n_cblocks) {
          // We reached the end of encoding, return count[c].
          return m_count[c];
        } else if (!(cblock_id & k_cblocks_in_sblock_mask)) {
          // We reached the boundary of superblock,
          // retreive the answer from superblock header.
          return m_sblock_header[256 * (cblock_id >> k_cblocks_in_sblock_log) + c];
        } else {
          // We found cblock where c occurrs, but it wasn't on the
          // sblock boundary. In the recursive call this will either
          // be case 1 or case 2.
          return rank(cblock_id << k_cblock_size_log, c);
        }
      }
    }

    ~rank4n() {
      if (m_length) {
        free(m_sblock_header);
        free(m_cblock_header);
        free(m_cblock_mapping);
        free(m_freq_trunk);
        free(m_rare_trunk);
      }
      free(m_count);
    }
};


template<unsigned k_sblock_size_log, unsigned k_cblock_size_log>
  const unsigned rank4n<k_sblock_size_log, k_cblock_size_log>
  ::k_cblock_size = (1 << k_cblock_size_log);

template<unsigned k_sblock_size_log, unsigned k_cblock_size_log>
  const unsigned rank4n<k_sblock_size_log, k_cblock_size_log>
  ::k_cblocks_in_sblock_log = k_sblock_size_log - k_cblock_size_log;

template<unsigned k_sblock_size_log, unsigned k_cblock_size_log>
  const unsigned rank4n<k_sblock_size_log, k_cblock_size_log>
  ::k_cblocks_in_sblock = (1 << (k_sblock_size_log - k_cblock_size_log));

template<unsigned k_sblock_size_log, unsigned k_cblock_size_log>
  const unsigned rank4n<k_sblock_size_log, k_cblock_size_log>
  ::k_cblocks_in_sblock_mask = (1 << (k_sblock_size_log - k_cblock_size_log)) - 1;

template<unsigned k_sblock_size_log, unsigned k_cblock_size_log>
  const unsigned rank4n<k_sblock_size_log, k_cblock_size_log>
  ::k_sblock_size = (1 << k_sblock_size_log);
    
template<unsigned k_sblock_size_log, unsigned k_cblock_size_log>
  const unsigned rank4n<k_sblock_size_log, k_cblock_size_log>
  ::k_sblock_size_mask = (1 << k_sblock_size_log) - 1;


#endif // __RANK4N_H_INCLUDED
