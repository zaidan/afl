#include "afl/types.h"

#include "afl/mutate/stage/flip16.h"

#include "afl/globals.h"

#include "afl/fuzz/common.h"
#include "afl/fuzz/stages.h"
#include "afl/hash.h"
#include "afl/mutate/eff.h"
#include "afl/mutate/flip.h"

int stage_flip16(char** argv,
                 u64* orig_hit_cnt,
                 u64* new_hit_cnt,
                 u8* out_buf,
                 s32 len,
                 u8* eff_map) {
  s32 mutate_len;
  u8* mutate_buf = mutation_buffer_pos(out_buf, len, &mutate_len);

  /* Two walking bytes. */
  stage_name = "bitflip 16/8";
  stage_short = "flip16";
  stage_cur = 0;
  stage_max = mutate_len - 1;

  *orig_hit_cnt = *new_hit_cnt;

  for (int i = 0; i < mutate_len - 1; i++) {
    /* Let's consult the effector map... */
    if (!eff_map[EFF_APOS(i)] && !eff_map[EFF_APOS(i + 1)]) {
      stage_max--;
      continue;
    }

    stage_cur_byte = i;

    *(u16*)(mutate_buf + i) ^= 0xFFFF;

    if (common_fuzz_stuff(argv, out_buf, len)) {
      return 0;
    }
    stage_cur++;

    *(u16*)(mutate_buf + i) ^= 0xFFFF;
  }

  *new_hit_cnt = queued_paths + unique_crashes;

  stage_finds[STAGE_FLIP16] += *new_hit_cnt - *orig_hit_cnt;
  stage_cycles[STAGE_FLIP16] += stage_max;

  return 1;
}
