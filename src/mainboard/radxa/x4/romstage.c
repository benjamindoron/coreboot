/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <spd_bin.h>
#include <fsp/api.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <gpio.h>

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	//	TODO: DQ and DQS map should be correct.
	//	something's still not right though:
	//	[SPEW ]  Calling FspMemoryInit: 0xffc6f220
	//	[SPEW ]         0xfef87308: raminit_upd
	//	[SPEW ]         0xfef944d0: &hob_list_ptr
	//	[EMERG]  FspMemoryInit error, status=0x80000007

	/* DQ CPU<>DRAM map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = { 12,  9, 10, 11, 14, 13,  8, 15 },
			.dq1 = {  3,  1,  2,  0,  4,  7,  5,  6 },
		},
		.ddr1 = {
			.dq0 = {  3,  1,  2,  0,  4,  7,  5,  6 },
			.dq1 = { 13,  9,  8, 11, 10, 14, 15, 12 },
		},
		.ddr2 = {
			.dq0 = {  2,  6,  0,  3,  5, 1,  4,  7 }, 
			.dq1 = {  11,  12, 9, 15, 10, 8, 14, 13 },
		},
		.ddr3 = {
			.dq0 = {  0,  1,  5,  4,  3,  2,  7,  6 },
			.dq1 = { 14,  9, 8,  13, 15, 12, 11, 10 },
		},
		.ddr4 = {
			.dq0 = { 12,  9, 10, 11, 14, 13,  8, 15 },
			.dq1 = {  3,  1,  2,  0,  4,  7,  5,  6 },
		},
		.ddr5 = {
			.dq0 = {  3,  1,  2,  0,  4,  7,  5,  6 },
			.dq1 = { 13,  9,  8, 11, 10, 14, 15, 12 },
		},
		.ddr6 = {
			.dq0 = {  2,  6,  0,  3,  5, 1,  4,  7 },	
			.dq1 = {  11,  12, 9, 15, 10, 8, 14, 13 },
		},
		.ddr7 = {
			.dq0 = {  0,  1,  5,  4,  3,  2,  7,  6 },
			.dq1 = { 14,  9, 8,  13, 15, 12, 11, 10 },
		},
	},
	
	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 }
	},

	/* Board uses 100ohm rcomp resistor */
	.rcomp = {
		.resistor = 100,
	},

	.lp5x_config = {
		.ccc_config = 0x00,
	},

	.ect = true, /* Early Command Training */
	.UserBd = BOARD_TYPE_ULT_ULX,
	.LpDdrDqDqsReTraining = 1,
};

static size_t get_spd_index(void)
{
	/* Following straps are used to configure memory:
	 * GPP_E10:GPP_E11:
	 * 0:0 == 4GB
	 * 0:1 == 8GB
	 * 1:0 == 16GB
	 * 1:1 == 32GB */
	gpio_t spd_gpios[] = {
		GPP_E10,
		GPP_E11,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const bool half_populated = true;
	const struct mem_spd memory_down_spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = get_spd_index(),
	};

	memcfg_init(memupd, &lp5_mem_config, &memory_down_spd_info, half_populated);
}
