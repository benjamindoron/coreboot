/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_RAMSTAGE_H
#define VARIANT_RAMSTAGE_H

#include <soc/ramstage.h>

void variant_silicon_init_params(FSP_S_CONFIG *params);
void variant_ec_init(void);

#endif
