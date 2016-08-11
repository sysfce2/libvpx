/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP10_COMMON_ENTROPY_H_
#define VP10_COMMON_ENTROPY_H_

#include "vpx/vpx_integer.h"
#include "vpx_dsp/prob.h"

#if CONFIG_ANS
#include "vp10/common/ans.h"
#endif  // CONFIG_ANS
#include "vp10/common/common.h"
#include "vp10/common/enums.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DIFF_UPDATE_PROB 252
#define GROUP_DIFF_UPDATE_PROB 252

#if CONFIG_ENTROPY
#define COEF_PROBS_BUFS 16
#define QCTX_BIN_BITS 2
#define QCTX_BINS (1 << QCTX_BIN_BITS)
#endif  // CONFIG_ENTROPY

// Coefficient token alphabet
#define ZERO_TOKEN 0        // 0     Extra Bits 0+0
#define ONE_TOKEN 1         // 1     Extra Bits 0+1
#define TWO_TOKEN 2         // 2     Extra Bits 0+1
#define THREE_TOKEN 3       // 3     Extra Bits 0+1
#define FOUR_TOKEN 4        // 4     Extra Bits 0+1
#define CATEGORY1_TOKEN 5   // 5-6   Extra Bits 1+1
#define CATEGORY2_TOKEN 6   // 7-10  Extra Bits 2+1
#define CATEGORY3_TOKEN 7   // 11-18 Extra Bits 3+1
#define CATEGORY4_TOKEN 8   // 19-34 Extra Bits 4+1
#define CATEGORY5_TOKEN 9   // 35-66 Extra Bits 5+1
#define CATEGORY6_TOKEN 10  // 67+   Extra Bits 14+1
#define EOB_TOKEN 11        // EOB   Extra Bits 0+0

#define ENTROPY_TOKENS 12

#define ENTROPY_NODES 11

DECLARE_ALIGNED(16, extern const uint8_t, vp10_pt_energy_class[ENTROPY_TOKENS]);

#define CAT1_MIN_VAL 5
#define CAT2_MIN_VAL 7
#define CAT3_MIN_VAL 11
#define CAT4_MIN_VAL 19
#define CAT5_MIN_VAL 35
#define CAT6_MIN_VAL 67

// Extra bit probabilities.
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat1_prob[1]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat2_prob[2]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat3_prob[3]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat4_prob[4]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat5_prob[5]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat6_prob[14]);

#if CONFIG_VP9_HIGHBITDEPTH
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat1_prob_high10[1]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat2_prob_high10[2]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat3_prob_high10[3]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat4_prob_high10[4]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat5_prob_high10[5]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat6_prob_high10[16]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat1_prob_high12[1]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat2_prob_high12[2]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat3_prob_high12[3]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat4_prob_high12[4]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat5_prob_high12[5]);
DECLARE_ALIGNED(16, extern const uint8_t, vp10_cat6_prob_high12[18]);
#endif  // CONFIG_VP9_HIGHBITDEPTH

#define EOB_MODEL_TOKEN 3

typedef struct {
  const vpx_tree_index *tree;
  const vpx_prob *prob;
  int len;
  int base_val;
  const int16_t *cost;
} vp10_extra_bit;

// indexed by token value
extern const vp10_extra_bit vp10_extra_bits[ENTROPY_TOKENS];
#if CONFIG_VP9_HIGHBITDEPTH
extern const vp10_extra_bit vp10_extra_bits_high10[ENTROPY_TOKENS];
extern const vp10_extra_bit vp10_extra_bits_high12[ENTROPY_TOKENS];
#endif  // CONFIG_VP9_HIGHBITDEPTH

#define DCT_MAX_VALUE 16384
#if CONFIG_VP9_HIGHBITDEPTH
#define DCT_MAX_VALUE_HIGH10 65536
#define DCT_MAX_VALUE_HIGH12 262144
#endif  // CONFIG_VP9_HIGHBITDEPTH

/* Coefficients are predicted via a 3-dimensional probability table. */

#define REF_TYPES 2  // intra=0, inter=1

/* Middle dimension reflects the coefficient position within the transform. */
#define COEF_BANDS 6

/* Inside dimension is measure of nearby complexity, that reflects the energy
   of nearby coefficients are nonzero.  For the first coefficient (DC, unless
   block type is 0), we look at the (already encoded) blocks above and to the
   left of the current block.  The context index is then the number (0,1,or 2)
   of these blocks having nonzero coefficients.
   After decoding a coefficient, the measure is determined by the size of the
   most recently decoded coefficient.
   Note that the intuitive meaning of this measure changes as coefficients
   are decoded, e.g., prior to the first token, a zero means that my neighbors
   are empty while, after the first token, because of the use of end-of-block,
   a zero means we just decoded a zero and hence guarantees that a non-zero
   coefficient will appear later in this block.  However, this shift
   in meaning is perfectly OK because our context depends also on the
   coefficient band (and since zigzag positions 0, 1, and 2 are in
   distinct bands). */

#define COEFF_CONTEXTS 6
#define BAND_COEFF_CONTEXTS(band) ((band) == 0 ? 3 : COEFF_CONTEXTS)

// #define ENTROPY_STATS

typedef unsigned int
    vp10_coeff_count[REF_TYPES][COEF_BANDS][COEFF_CONTEXTS][ENTROPY_TOKENS];
typedef unsigned int
    vp10_coeff_stats[REF_TYPES][COEF_BANDS][COEFF_CONTEXTS][ENTROPY_NODES][2];

#define SUBEXP_PARAM 4   /* Subexponential code parameter */
#define MODULUS_PARAM 13 /* Modulus parameter */

struct VP10Common;
void vp10_default_coef_probs(struct VP10Common *cm);
void vp10_adapt_coef_probs(struct VP10Common *cm);
#if CONFIG_ENTROPY
void vp10_partial_adapt_probs(struct VP10Common *cm, int mi_row, int mi_col);
#endif  // CONFIG_ENTROPY

// This is the index in the scan order beyond which all coefficients for
// 8x8 transform and above are in the top band.
// This macro is currently unused but may be used by certain implementations
#define MAXBAND_INDEX 21

DECLARE_ALIGNED(16, extern const uint8_t, vp10_coefband_trans_8x8plus[1024]);
#if CONFIG_EXT_TX
DECLARE_ALIGNED(16, extern const uint8_t, vp10_coefband_trans_4x8_8x4[32]);
#endif  // CONFIG_EXT_TX
DECLARE_ALIGNED(16, extern const uint8_t, vp10_coefband_trans_4x4[16]);

DECLARE_ALIGNED(16, extern const uint16_t, band_count_table[TX_SIZES_ALL][8]);
DECLARE_ALIGNED(16, extern const uint16_t,
                band_cum_count_table[TX_SIZES_ALL][8]);

static INLINE const uint8_t *get_band_translate(TX_SIZE tx_size) {
  switch (tx_size) {
    case TX_4X4: return vp10_coefband_trans_4x4;
#if CONFIG_EXT_TX
    case TX_4X8:
      return vp10_coefband_trans_4x8_8x4;
#endif  // CONFIG_EXT_TX
    default: return vp10_coefband_trans_8x8plus;
  }
}

// 128 lists of probabilities are stored for the following ONE node probs:
// 1, 3, 5, 7, ..., 253, 255
// In between probabilities are interpolated linearly

#define COEFF_PROB_MODELS 255

#define UNCONSTRAINED_NODES 3

#define PIVOT_NODE 2  // which node is pivot

#define MODEL_NODES (ENTROPY_NODES - UNCONSTRAINED_NODES)
extern const vpx_tree_index vp10_coef_con_tree[TREE_SIZE(ENTROPY_TOKENS)];
extern const vpx_prob vp10_pareto8_full[COEFF_PROB_MODELS][MODEL_NODES];
#if CONFIG_ANS
extern const AnsP10
    vp10_pareto8_token_probs[COEFF_PROB_MODELS][ENTROPY_TOKENS - 2];

typedef rans_dec_lut coeff_cdf_model[REF_TYPES][COEF_BANDS][COEFF_CONTEXTS];
#endif  // CONFIG_ANS

typedef vpx_prob vp10_coeff_probs_model[REF_TYPES][COEF_BANDS][COEFF_CONTEXTS]
                                       [UNCONSTRAINED_NODES];

typedef unsigned int vp10_coeff_count_model
    [REF_TYPES][COEF_BANDS][COEFF_CONTEXTS][UNCONSTRAINED_NODES + 1];

void vp10_model_to_full_probs(const vpx_prob *model, vpx_prob *full);

typedef char ENTROPY_CONTEXT;

static INLINE int combine_entropy_contexts(ENTROPY_CONTEXT a,
                                           ENTROPY_CONTEXT b) {
  return (a != 0) + (b != 0);
}

static INLINE int get_entropy_context(TX_SIZE tx_size, const ENTROPY_CONTEXT *a,
                                      const ENTROPY_CONTEXT *l) {
  ENTROPY_CONTEXT above_ec = 0, left_ec = 0;

  switch (tx_size) {
    case TX_4X4:
      above_ec = a[0] != 0;
      left_ec = l[0] != 0;
      break;
#if CONFIG_EXT_TX
    case TX_4X8:
      above_ec = a[0] != 0;
      left_ec = !!*(const uint16_t *)l;
      break;
    case TX_8X4:
      above_ec = !!*(const uint16_t *)a;
      left_ec = l[0] != 0;
      break;
    case TX_8X16:
      above_ec = !!*(const uint16_t *)a;
      left_ec  = !!*(const uint32_t *)l;
      break;
    case TX_16X8:
      above_ec = !!*(const uint32_t *)a;
      left_ec  = !!*(const uint16_t *)l;
      break;
    case TX_16X32:
      above_ec = !!*(const uint32_t *)a;
      left_ec  = !!*(const uint64_t *)l;
      break;
    case TX_32X16:
      above_ec = !!*(const uint64_t *)a;
      left_ec  = !!*(const uint32_t *)l;
      break;
#endif  // CONFIG_EXT_TX
    case TX_8X8:
      above_ec = !!*(const uint16_t *)a;
      left_ec = !!*(const uint16_t *)l;
      break;
    case TX_16X16:
      above_ec = !!*(const uint32_t *)a;
      left_ec = !!*(const uint32_t *)l;
      break;
    case TX_32X32:
      above_ec = !!*(const uint64_t *)a;
      left_ec = !!*(const uint64_t *)l;
      break;
    default: assert(0 && "Invalid transform size."); break;
  }
  return combine_entropy_contexts(above_ec, left_ec);
}

#if CONFIG_ANS
struct frame_contexts;
void vp10_coef_pareto_cdfs(struct frame_contexts *fc);
#endif  // CONFIG_ANS

#if CONFIG_ENTROPY
#define COEF_COUNT_SAT_BITS 5
#define COEF_MAX_UPDATE_FACTOR_BITS 7
#define COEF_COUNT_SAT_AFTER_KEY_BITS 5
#define COEF_MAX_UPDATE_FACTOR_AFTER_KEY_BITS 7
#define MODE_MV_COUNT_SAT_BITS 5
#define MODE_MV_MAX_UPDATE_FACTOR_BITS 7

#else

#define COEF_COUNT_SAT 24
#define COEF_MAX_UPDATE_FACTOR 112
#define COEF_COUNT_SAT_AFTER_KEY 24
#define COEF_MAX_UPDATE_FACTOR_AFTER_KEY 128

#endif  // CONFIG_ENTROPY

static INLINE vpx_prob vp10_merge_probs(vpx_prob pre_prob,
                                        const unsigned int ct[2],
                                        unsigned int count_sat,
                                        unsigned int max_update_factor) {
#if CONFIG_ENTROPY
  const vpx_prob prob = get_binary_prob(ct[0], ct[1]);
  const unsigned int count =
      VPXMIN(ct[0] + ct[1], (unsigned int)(1 << count_sat));
  const unsigned int factor = count << (max_update_factor - count_sat);
  return weighted_prob(pre_prob, prob, factor);
#else
  return merge_probs(pre_prob, ct, count_sat, max_update_factor);
#endif  // CONFIG_ENTROPY
}

static INLINE vpx_prob vp10_mode_mv_merge_probs(vpx_prob pre_prob,
                                                const unsigned int ct[2]) {
#if CONFIG_ENTROPY
  return vp10_merge_probs(pre_prob, ct, MODE_MV_COUNT_SAT_BITS,
                          MODE_MV_MAX_UPDATE_FACTOR_BITS);
#else
  return mode_mv_merge_probs(pre_prob, ct);
#endif  // CONFIG_ENTROPY
}

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // VP10_COMMON_ENTROPY_H_