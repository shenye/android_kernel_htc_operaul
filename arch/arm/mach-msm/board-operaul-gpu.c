/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <mach/kgsl.h>
#include <mach/msm_bus_board.h>
#include <mach/board.h>
#include <mach/msm_dcvs.h>
#include <mach/socinfo.h>

#include <mach/msm_iomap.h>

#include "../devices.h"
#include "../board-8930.h"

#include "../board-operaul.h"

#ifdef CONFIG_MSM_DCVS
static struct msm_dcvs_freq_entry grp3d_freq[] = {
       {0, 0, 333932},
       {0, 0, 497532},
       {0, 0, 707610},
       {0, 0, 844545},
};

static struct msm_dcvs_core_info grp3d_core_info = {
       .freq_tbl = &grp3d_freq[0],
       .core_param = {
               .max_time_us = 100000,
               .num_freq = ARRAY_SIZE(grp3d_freq),
       },
       .algo_param = {
               .slack_time_us = 39000,
               .disable_pc_threshold = 86000,
               .ss_window_size = 1000000,
               .ss_util_pct = 95,
               .em_max_util_pct = 97,
               .ss_iobusy_conv = 100,
       },
};
#endif 
#ifdef CONFIG_MSM_BUS_SCALING
static struct msm_bus_vectors grp3d_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors grp3d_low_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(2000),
	},
};

static struct msm_bus_vectors grp3d_nominal_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(3200),
	},
};

static struct msm_bus_vectors grp3d_max_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(4264),
	},
};

static struct msm_bus_paths grp3d_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(grp3d_init_vectors),
		grp3d_init_vectors,
	},
	{
		ARRAY_SIZE(grp3d_low_vectors),
		grp3d_low_vectors,
	},
	{
		ARRAY_SIZE(grp3d_nominal_vectors),
		grp3d_nominal_vectors,
	},
	{
		ARRAY_SIZE(grp3d_max_vectors),
		grp3d_max_vectors,
	},
};

static struct msm_bus_scale_pdata grp3d_bus_scale_pdata = {
	grp3d_bus_scale_usecases,
	ARRAY_SIZE(grp3d_bus_scale_usecases),
	.name = "grp3d",
};
#endif

static struct resource kgsl_3d0_resources[] = {
	{
		.name = KGSL_3D0_REG_MEMORY,
		.start = 0x04300000, 
		.end = 0x0431ffff,
		.flags = IORESOURCE_MEM,
	},
	{
		.name = KGSL_3D0_IRQ,
		.start = GFX3D_IRQ,
		.end = GFX3D_IRQ,
		.flags = IORESOURCE_IRQ,
	},
};

static const struct kgsl_iommu_ctx kgsl_3d0_iommu0_ctxs[] = {
	{ "gfx3d_user", 0 },
	{ "gfx3d_priv", 1 },
};

static struct kgsl_device_iommu_data kgsl_3d0_iommu_data[] = {
	{
		.iommu_ctxs = kgsl_3d0_iommu0_ctxs,
		.iommu_ctx_count = ARRAY_SIZE(kgsl_3d0_iommu0_ctxs),
		.physstart = 0x07C00000,
		.physend = 0x07C00000 + SZ_1M - 1,
	},
};

static struct kgsl_device_platform_data kgsl_3d0_pdata = {
	.pwrlevel = {
		{
			.gpu_freq = 400000000,
			.bus_freq = 3,
			.io_fraction = 0,
		},
		{
			.gpu_freq = 320000000,
			.bus_freq = 2,
			.io_fraction = 33,
		},
		{
			.gpu_freq = 192000000,
			.bus_freq = 1,
			.io_fraction = 100,
		},
		{
			.gpu_freq = 27000000,
			.bus_freq = 0,
		},
	},
	.init_level = 2,
	.num_levels = 4,
	.set_grp_async = NULL,
	.idle_timeout = HZ/12,
	.nap_allowed = true,
	.clk_map = KGSL_CLK_CORE | KGSL_CLK_IFACE | KGSL_CLK_MEM_IFACE,
#ifdef CONFIG_MSM_BUS_SCALING
	.bus_scale_table = &grp3d_bus_scale_pdata,
#endif
	.iommu_data = kgsl_3d0_iommu_data,
	.iommu_count = ARRAY_SIZE(kgsl_3d0_iommu_data),
#ifdef CONFIG_MSM_DCVS
	.core_info = &grp3d_core_info,
#endif
	.snapshot_address = MSM_GPU_SNAP_SHOT_3D0_PHYS,
};

static struct platform_device device_kgsl_3d0 = {
	.name = "kgsl-3d0",
	.id = 0,
	.num_resources = ARRAY_SIZE(kgsl_3d0_resources),
	.resource = kgsl_3d0_resources,
	.dev = {
		.platform_data = &kgsl_3d0_pdata,
	},
};

void operaul_init_gpu(void)
{
    unsigned int version = socinfo_get_version();

	/* Set the turbo speed for the AA and AB respectively */

	if (cpu_is_msm8930aa())
		kgsl_3d0_pdata.pwrlevel[0].gpu_freq = 450000000;

	

	/* Set up the chip ID based on the SoC version */

	 if ((SOCINFO_VERSION_MAJOR(version) == 1) &&
		(SOCINFO_VERSION_MINOR(version) == 2))
		kgsl_3d0_pdata.chipid = ADRENO_CHIPID(3, 0, 5, 2);
	else
		kgsl_3d0_pdata.chipid = ADRENO_CHIPID(3, 0, 5, 0);

	platform_device_register(&device_kgsl_3d0);
}

