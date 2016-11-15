//==============================================================================
// Implementation of string range matching, as descrined in
//
//   Juha Kärkkäinen, Dominik Kempa, Simon J. Puglisi:
//   String Range Matching. In Proc. CPM 2014
//=============================================================================

#ifndef __SRANK_H_INCLUDED
#define __SRANK_H_INCLUDED

#include <cstring>
#include <algorithm>

#include "bitvector.h"


//==============================================================================
// Update ms-decomposition of text[0..length) from text[0..length - 1).
//==============================================================================
void next(unsigned char *text, long length, long &s, long &p, long &r) {
  if (length == 1) { s = 0; p = 1; r = 0; return; }
  long i = length - 1;
  while (i < length) {
    unsigned char a = text[s + r], b = text[i];
    if (a > b) { p = i - s + 1; r = 0; }
    else if (a < b) { i -= r; s = i; p = 1; r = 0; }
    else { ++r; if (r == p) r = 0; } ++i;
  }
}


void compute_gt_eof_bv(unsigned char *A, long A_length,
                       unsigned char *B, long B_length,
                       bitvector *gt_head_bv, bitvector *gt_eof_bv) {
  long i = 0, el = 0, s = 0, p = 0, r = 0;
  long i_max = 0, el_max = 0, s_max = 0, p_max = 0, r_max = 0;

  while (i < B_length) {
    while (i + el < B_length && el < A_length && B[i + el] == A[el])
      next(A, ++el, s, p, r);

    if (el == A_length ||
        (i + el == B_length && !gt_head_bv->get(A_length - 1 - el)) ||
        (i + el < B_length && B[i + el] > A[el]))
      gt_eof_bv->set(i);

    long j = i_max;
    if (el > el_max) {
      std::swap(el, el_max);
      std::swap(s, s_max);
      std::swap(p, p_max);
      std::swap(r, r_max);
      i_max = i;
    }

    if (p && 3 * p <= el && !memcmp(A, A + p, s)) {
      for (long k = 1; k < p; ++k)
        if (gt_eof_bv->get(j + k)) gt_eof_bv->set(i + k);
      i += p; el -= p;
    } else {
      long h = (el / 3) + 1;
      for (long k = 1; k < h; ++k)
        if (gt_eof_bv->get(j + k)) gt_eof_bv->set(i + k);
      i += h; el = 0; s = 0; p = 0;
    }
  }
}

//==============================================================================
// Compute bitmap bv[0..length) such that bv[i] == 1 iff
// text[i..length) > text[0..length).
//
// Returns the number of suffixes i st. text[i..length) < text[0..length).
//==============================================================================
long compute_new_gt_head_bv(unsigned char *T, long n,
    bitvector *new_gt_head_bv) {

  long whole_suffix_rank = n - 1;
  long i = 1, el = 0, s = 0, p = 0, r = 0;
  long i_max = 0, el_max = 0, s_max = 0, p_max = 0, r_max = 0;
  while (i < n) {
    while (i + el < n && el < n && T[i + el] == T[el]) next(T, ++el, s, p, r);
    if (i + el < n && (el == n || T[i + el] > T[el])) {
      // Fill output right-to-left to avoid reversing.
      new_gt_head_bv->set(n - 1 - i);
      --whole_suffix_rank;
    }

    long j = i_max;
    if (el > el_max) {
      std::swap(el, el_max);
      std::swap(s, s_max);
      std::swap(p, p_max);
      std::swap(r, r_max);
      i_max = i;
    }

    if (p && 3 * p <= el && !memcmp(T, T + p, s)) {
      for (long k = 1; k < p; ++k) {
        if (new_gt_head_bv->get(n - 1 - (j + k))) {
          new_gt_head_bv->set(n - 1 - (i + k));
          --whole_suffix_rank;
        }
      }

      i += p;
      el -= p;
    } else {
      long h = (el / 3) + 1;
      for (long k = 1; k < h; ++k) {
        if (new_gt_head_bv->get(n - 1 - (j + k))) {
          new_gt_head_bv->set(n - 1 - (i + k));
          --whole_suffix_rank;
        }
      }

      i += h;
      el = 0;
      s = 0;
      p = 0;
    }
  }

  return whole_suffix_rank;
}

#endif // __SRANK_H_INCLUDED
