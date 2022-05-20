/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/util.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <variant/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* TODO: Dq/Dqs only applies to LPDDR4? */
	const struct mb_cfg board_config = {
		.type = MEM_TYPE_DDR4,
		/* Use FSP default */
		.ect = true,
		.ddr4_config = {
			.dq_pins_interleaved = true,
		},
	};
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = { .addr_dimm[0] = 0x50, },
			[1] = { .addr_dimm[0] = 0x52, },
		},
	};

	variant_memory_init_params(mupd);

	/* `half_populated` only applies to LPDDR4? */
	memcfg_init(mupd, &board_config, &spd_info, false);
}
