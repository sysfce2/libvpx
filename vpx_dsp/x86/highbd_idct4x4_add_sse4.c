/*
 *  Copyright (c) 2017 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <smmintrin.h>  // SSE4.1

#include "./vpx_dsp_rtcd.h"
#include "vpx_dsp/x86/highbd_inv_txfm_sse2.h"
#include "vpx_dsp/x86/highbd_inv_txfm_sse4.h"
#include "vpx_dsp/x86/inv_txfm_sse2.h"
#include "vpx_dsp/x86/transpose_sse2.h"

static INLINE void highbd_idct4(__m128i *const io) {
  __m128i temp[2], step[4];

  transpose_32bit_4x4(io, io);

  // stage 1
  temp[0] = _mm_add_epi32(io[0], io[2]);  // input[0] + input[2]
  extend_64bit(temp[0], temp);
  step[0] = multiplication_round_shift_sse4_1(temp, (int)cospi_16_64);
  temp[0] = _mm_sub_epi32(io[0], io[2]);  // input[0] - input[2]
  extend_64bit(temp[0], temp);
  step[1] = multiplication_round_shift_sse4_1(temp, (int)cospi_16_64);
  highbd_multiplication_and_add_sse4_1(io[1], io[3], (int)cospi_24_64,
                                       (int)cospi_8_64, &step[2], &step[3]);

  // stage 2
  io[0] = _mm_add_epi32(step[0], step[3]);  // step[0] + step[3]
  io[1] = _mm_add_epi32(step[1], step[2]);  // step[1] + step[2]
  io[2] = _mm_sub_epi32(step[1], step[2]);  // step[1] - step[2]
  io[3] = _mm_sub_epi32(step[0], step[3]);  // step[0] - step[3]
}

void vpx_highbd_idct4x4_16_add_sse4_1(const tran_low_t *input, uint16_t *dest,
                                      int stride, int bd) {
  __m128i io[4];

  io[0] = _mm_load_si128((const __m128i *)(input + 0));
  io[1] = _mm_load_si128((const __m128i *)(input + 4));
  io[2] = _mm_load_si128((const __m128i *)(input + 8));
  io[3] = _mm_load_si128((const __m128i *)(input + 12));

  if (bd == 8) {
    __m128i io_short[2];

    io_short[0] = _mm_packs_epi32(io[0], io[1]);
    io_short[1] = _mm_packs_epi32(io[2], io[3]);
    idct4_sse2(io_short);
    idct4_sse2(io_short);
    io_short[0] = _mm_add_epi16(io_short[0], _mm_set1_epi16(8));
    io_short[1] = _mm_add_epi16(io_short[1], _mm_set1_epi16(8));
    io[0] = _mm_srai_epi16(io_short[0], 4);
    io[1] = _mm_srai_epi16(io_short[1], 4);
  } else {
    highbd_idct4(io);
    highbd_idct4(io);
    io[0] = wraplow_16bit_shift4(io[0], io[1], _mm_set1_epi32(8));
    io[1] = wraplow_16bit_shift4(io[2], io[3], _mm_set1_epi32(8));
  }

  recon_and_store_4(io, dest, stride, bd);
}
