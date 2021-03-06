#include "afl/types.h"

#include "afl/mutate/stage/user_extras_ui.h"

#include "afl/alloc-inl.h"
#include "afl/globals.h"

#include "afl/fuzz/common.h"
#include "afl/fuzz/stages.h"
#include "afl/mutate/eff.h"

/* Insertion of user-supplied extras. */
int stage_user_extras_ui(char** argv,
                         u64* orig_hit_cnt,
                         u64* new_hit_cnt,
                         u8* out_buf,
                         s32 len,
                         u8* eff_map) {
  s32 mutate_len;
  u8* mutate_buf = mutation_buffer_pos(out_buf, len, &mutate_len);

  u8* ex_tmp;
  stage_name = "user extras (insert)";
  stage_short = "ext_UI";
  stage_cur = 0;
  stage_max = extras_cnt * mutate_len;

  *orig_hit_cnt = *new_hit_cnt;

  ex_tmp = ck_alloc(mutate_len + MAX_DICT_FILE);

  for (int i = 0; i <= mutate_len; i++) {
    stage_cur_byte = i;

    for (int j = 0; j < extras_cnt; j++) {
      if (mutate_len + extras[j].len > MAX_FILE) {
        stage_max--;
        continue;
      }

      /* Insert token */
      memcpy(ex_tmp + i, extras[j].data, extras[j].len);

      /* Copy tail */
      memcpy(ex_tmp + i + extras[j].len, mutate_buf + i, mutate_len - i);

      if (common_fuzz_stuff(argv, ex_tmp, mutate_len + extras[j].len)) {
        ck_free(ex_tmp);
        return 0;
      }

      stage_cur++;
    }

    /* Copy head */
    ex_tmp[i] = mutate_buf[i];
  }

  ck_free(ex_tmp);

  *new_hit_cnt = queued_paths + unique_crashes;

  stage_finds[STAGE_EXTRAS_UI] += *new_hit_cnt - *orig_hit_cnt;
  stage_cycles[STAGE_EXTRAS_UI] += stage_max;

  return 1;
}
