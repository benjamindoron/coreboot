/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CFG_GPIO_H
#define CFG_GPIO_H

#include <gpio.h>

/* Early pad configuration */
static const struct pad_config early_gpio_table[] = {
	// GPIO (ISH_GP2) = DGPU_PRESENT
	PAD_CFG_GPI_TRIG_OWN(GPP_A20, NONE, DEEP, OFF, ACPI),
	// GPIO (VRALERT#)
	PAD_CFG_GPI_TRIG_OWN(GPP_B2, NONE, DEEP, OFF, ACPI),
	// GPIO (CPU_GP3) => DGPU_HOLD_RST#
	PAD_CFG_GPO(GPP_B4, 1, DEEP),
	// GPIO (GSPI1_MISO) => DGPU_PWR_EN#
	PAD_CFG_TERM_GPO(GPP_B21, 1, DN_20K, DEEP),
};

/* Pad configuration was generated automatically using intelp2m utility */
static const struct pad_config gpio_table[] = {

	/* ------- GPIO Community 0 ------- */

	/* ------- GPIO Group GPP_A ------- */
	// RCIN# <= H_RCIN#
	PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1),
	// LAD0 (ESPI_IO0) <=> LPC_AD_CPU_P0
	PAD_CFG_NF(GPP_A1, NATIVE, DEEP, NF1),
	// LAD1 (ESPI_IO1) <=> LPC_AD_CPU_P1
	PAD_CFG_NF(GPP_A2, NATIVE, DEEP, NF1),
	// LAD2 (ESPI_IO2) <=> LPC_AD_CPU_P2
	PAD_CFG_NF(GPP_A3, NATIVE, DEEP, NF1),
	// LAD3 (ESPI_IO3) <=> LPC_AD_CPU_P3
	PAD_CFG_NF(GPP_A4, NATIVE, DEEP, NF1),
	// LFRAME# (ESPI_CS#) => LPC_FRAME#_CPU
	PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),
	// SERIRQ <=> INT_SERIRQ
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),
	// PIRQA# = PIRQA#
	PAD_CFG_NF(GPP_A7, NONE, DEEP, NF1),
	// CLKRUN# <= PM_CLKRUN#_EC
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),
	// CLKOUT_LPC0 (ESPI_CLK) <= LPC_CLK_CPU_P0
	PAD_CFG_NF(GPP_A9, DN_20K, DEEP, NF1),
	// CLKOUT_LPC1 <= LPC_CLK_CPU_P1
	PAD_CFG_NF(GPP_A10, DN_20K, DEEP, NF1),
	// GPIO (PME#) // NC
	PAD_CFG_TERM_GPO(GPP_A11, 1, DN_20K, DEEP),
	// GPIO (SX_EXIT_HOLDOFF#/BM_BUSY#/ISH_GP6) <= GC6_FB_EN
	PAD_CFG_GPI_TRIG_OWN(GPP_A12, NONE, DEEP, OFF, ACPI),
	// SUSWARN#/SUSPWRDNACK = PM_SUSACK#
	PAD_CFG_NF(GPP_A13, NONE, DEEP, NF1),
	// SUS_STAT# (ESPI_RESET#) => PM_SUS_STAT#
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),
	// SUS_ACK# = PM_SUSACK#
	PAD_CFG_NF(GPP_A15, DN_20K, DEEP, NF1),
	// GPIO (SD_1P8_SEL) // NC
	PAD_CFG_TERM_GPO(GPP_A16, 1, DN_20K, DEEP),
	// GPIO (SD_PWR_EN#/ISH_GP7) // NC
	PAD_CFG_TERM_GPO(GPP_A17, 1, DN_20K, DEEP),
	// GPIO (ISH_GP0) => GSENSOR_INT#
	PAD_CFG_GPI_TRIG_OWN(GPP_A18, NONE, DEEP, OFF, ACPI),
	// GPIO (ISH_GP1) // NC
	PAD_CFG_TERM_GPO(GPP_A19, 1, DN_20K, DEEP),
	// GPIO (ISH_GP3) // NC
	PAD_CFG_TERM_GPO(GPP_A21, 1, DN_20K, DEEP),
	// GPIO (ISH_GP4) <= GPU_EVENT#
	PAD_CFG_GPO(GPP_A22, 1, DEEP),
	// GPIO (ISH_GP5) // NC
	PAD_CFG_TERM_GPO(GPP_A23, 1, DN_20K, DEEP),

	/* ------- GPIO Group GPP_B ------- */
	// CORE_VID0 // V0.85A_VID0
	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1),
	// CORE_VID1 // V0.85A_VID1
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1),
	// GPIO (CPU_GP2) <= TP_IN#
	PAD_CFG_GPI_APIC_HIGH(GPP_B3, NONE, PLTRST),	// TODO: _GPI_IRQ_WAKE?
	// SRCCLKREQ0# <= PEG_CLKREQ_CPU#
	PAD_CFG_NF(GPP_B5, NONE, DEEP, NF1),
	// SRCCLKREQ1# <= LAN_CLKREQ_CPU#
	PAD_CFG_NF(GPP_B6, NONE, DEEP, NF1),
	// SRCCLKREQ2# <= WLAN_CLKREQ_CPU#
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF1),
	// SRCCLKREQ3# <= MSATA_CLKREQ_CPU#
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF1),
	// SRCCLKREQ4# // SRCCLKREQ4# ("Remove TBT")
	PAD_CFG_NF(GPP_B9, NONE, DEEP, NF1),
	// SRCCLKREQ5# // SRCCLKREQ5#
	PAD_CFG_NF(GPP_B10, NONE, DEEP, NF1),
	// GPIO (EXT_PWR_GATE#) = EXT_PWR_GATE#
	PAD_CFG_TERM_GPO(GPP_B11, 1, DN_20K, DEEP),
	// GPIO (SLP_S0#) // NC
	PAD_CFG_TERM_GPO(GPP_B12, 1, DN_20K, DEEP),
	// PLTRST# => PLT_RST#
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	// GPIO (SPKR) => HDA_SPKR (Strap - Top Swap Override)
	PAD_CFG_TERM_GPO(GPP_B14, 1, DN_20K, DEEP),
	// GPIO (GSPI0_CS#) = TOUCH_DET#
	PAD_CFG_GPO(GPP_B15, 0, DEEP),
	// GPIO (GSPI0_CLK) // NC
	PAD_CFG_GPO(GPP_B16, 0, DEEP),
	// GPIO (GSPI0_MISO) // NC ("Remove TBT")
	PAD_CFG_GPI_SCI(GPP_B17, DN_20K, DEEP, EDGE_SINGLE, INVERT),
	// GPIO (GSPI0_MOSI) => GPP_B18/GSPI0_MOSI (Strap - No reboot)
	PAD_CFG_TERM_GPO(GPP_B18, 1, DN_20K, DEEP),
	// GPIO (GSPI1_CS#) => RTC_DET#
	PAD_CFG_GPI_TRIG_OWN(GPP_B19, NONE, DEEP, OFF, ACPI),
	// GPIO (GSPI1_CLK) <= PSW_CLR#
	PAD_CFG_GPI_TRIG_OWN(GPP_B20, DN_20K, DEEP, OFF, ACPI),
	// GPIO (GSPI1_MOSI) => GPP_B22/GSPI1_MOSI (Strap - Boot BIOS strap)
	PAD_CFG_TERM_GPO(GPP_B22, 1, DN_20K, DEEP),
	// GPIO (SML1ALERT#/PCHHOT#) => GPP_B23 (Strap)
	PAD_CFG_TERM_GPO(GPP_B23, 1, DN_20K, DEEP),

	/* ------- GPIO Community 1 ------- */

	/* ------- GPIO Group GPP_C ------- */
	// SMBCLK <= SMB_CLK
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	// SMBDATA = SMB_DATA
	PAD_CFG_NF(GPP_C1, DN_20K, DEEP, NF1),
	// GPIO (SMBALERT#) => GPP_C2 (Strap - TLS Confidentiality)
	PAD_CFG_TERM_GPO(GPP_C2, 1, DN_20K, DEEP),
	// GPIO (SML0CLK) // NC
	PAD_CFG_TERM_GPO(GPP_C3, 1, DN_20K, DEEP),
	// GPIO (SML0DATA) // NC
	PAD_CFG_TERM_GPO(GPP_C4, 1, DN_20K, DEEP),
	// GPIO (SML0ALERT#) // NC (Strap - eSPI or LPC)
	PAD_CFG_TERM_GPO(GPP_C5, 1, DN_20K, DEEP),
	// RESERVED (SML1CLK) <=> SML1_CLK (KBC)
	// RESERVED (SML1DATA) <=> SML1_DATA (KBC)
	// GPIO (UART0_RXD) // NC
	PAD_CFG_TERM_GPO(GPP_C8, 1, DN_20K, DEEP),
	// GPIO (UART0_TXD) // NC
	PAD_CFG_TERM_GPO(GPP_C9, 1, DN_20K, DEEP),
	// GPIO (UART0_RTS#) // NC
	PAD_CFG_TERM_GPO(GPP_C10, 1, DN_20K, DEEP),
	// GPIO (UART0_CTS#) // NC
	PAD_CFG_TERM_GPO(GPP_C11, 1, DN_20K, DEEP),
	// GPIO (UART1_RXD/ISH_UART1_RXD) // NC
	PAD_CFG_TERM_GPO(GPP_C12, 1, DN_20K, DEEP),
	// GPIO (UART1_TXD/ISH_UART1_TXD) // NC
	PAD_CFG_TERM_GPO(GPP_C13, 1, DN_20K, DEEP),
	// GPIO (UART1_RTS#/ISH_UART1_RTS#) // NC
	PAD_CFG_TERM_GPO(GPP_C14, 1, DN_20K, DEEP),
	// GPIO (UART1_CTS#/ISH_UART1_CTS#) // NC
	PAD_CFG_TERM_GPO(GPP_C15, 1, DN_20K, DEEP),
	// I2C0_SDA <=> I2C0_DATA_CPU (Touch Panel)
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	// I2C0_SCL <=> I2C0_CLK_CPU (Touch Panel)
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	// I2C1_SDA <=> I2C1_DATA_CPU (Touch Pad)
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	// I2C1_SCL <=> I2C1_CLK_CPU (Touch Pad)
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),
	// UART2_RXD = LPSS_UART2_RXD
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	// UART2_TXD = LPSS_UART2_TXD
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	// UART2_RTS# = LPSS_UART2_RTS#
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF1),
	// UART2_CTS# = LPSS_UART2_CTS#
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF1),

	/* ------- GPIO Group GPP_D ------- */
	// GPIO (SPI1_CS#) // NC
	PAD_CFG_TERM_GPO(GPP_D0, 1, DN_20K, DEEP),
	// GPIO (SPI1_CLK) // NC
	PAD_CFG_TERM_GPO(GPP_D1, 1, DN_20K, DEEP),
	// SPI1_MISO // NC
	PAD_CFG_NF(GPP_D2, NONE, DEEP, NF1),
	// SPI1_MOSI // NC
	PAD_CFG_NF(GPP_D3, NONE, DEEP, NF1),
	// GPIO (FLASHTRIG) // NC
	PAD_CFG_TERM_GPO(GPP_D4, 1, DN_20K, DEEP),
	// GPIO (ISH_I2C0_SDA) // NC
	PAD_CFG_TERM_GPO(GPP_D5, 1, DN_20K, DEEP),
	// GPIO (ISH_I2C0_SCL) // NC
	PAD_CFG_TERM_GPO(GPP_D6, 1, DN_20K, DEEP),
	// GPIO (ISH_I2C1_SDA) // NC
	PAD_CFG_TERM_GPO(GPP_D7, 1, DN_20K, DEEP),
	// GPIO (ISH_I2C1_SCL) // NC
	PAD_CFG_TERM_GPO(GPP_D8, 1, DN_20K, DEEP),
	// GPIO // NC
	PAD_CFG_GPI_TRIG_OWN(GPP_D9, NONE, DEEP, LEVEL, ACPI),
	// GPIO => TOUCH_S_RST#
	PAD_CFG_GPI_TRIG_OWN(GPP_D10, NONE, DEEP, LEVEL, ACPI),
	// GPIO // NC
	PAD_CFG_GPI_TRIG_OWN(GPP_D11, NONE, DEEP, LEVEL, ACPI),
	// GPIO // NC ("Remove TBT")
	PAD_CFG_GPI_TRIG_OWN(GPP_D12, NONE, DEEP, LEVEL, ACPI),
	// GPIO (ISH_UART0_RXD/SML0BDATA/I2C4B_SDA) // NC
	PAD_CFG_TERM_GPO(GPP_D13, 1, DN_20K, DEEP),
	// GPIO (ISH_UART0_TXD/SML0BCLK/I2C4B_SCL) // NC
	PAD_CFG_TERM_GPO(GPP_D14, 1, DN_20K, DEEP),
	// GPIO (ISH_UART0_RTS#) // NC
	PAD_CFG_TERM_GPO(GPP_D15, 1, DN_20K, DEEP),
	// GPIO (ISH_UART0_CTS#/SML0BALERT#) // NC
	PAD_CFG_TERM_GPO(GPP_D16, 1, DN_20K, DEEP),
	// GPIO (DMIC_CLK1) // NC
	PAD_CFG_TERM_GPO(GPP_D17, 1, DN_20K, DEEP),
	// GPIO (DMIC_DATA1) // NC
	PAD_CFG_TERM_GPO(GPP_D18, 1, DN_20K, DEEP),
	// DMIC_CLK0 => DMIC_CLK_CON_R
	PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1),
	// DMIC_DATA0 => DMIC_PCH_DATA
	PAD_CFG_NF(GPP_D20, NONE, DEEP, NF1),
	// SPI1_IO2 // NC
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF1),
	// SPI1_IO3 // NC
	PAD_CFG_NF(GPP_D22, NONE, DEEP, NF1),
	// GPIO (I2S_MCLK) // NC
	PAD_CFG_TERM_GPO(GPP_D23, 1, DN_20K, DEEP),

	/* ------- GPIO Group GPP_E ------- */
	// SATAXPCIE0 (SATAGP0) = SATAGP0
	PAD_CFG_NF(GPP_E0, NONE, DEEP, NF1),
	// SATAXPCIE1 (SATAGP1) // NC
	PAD_CFG_NF(GPP_E1, NONE, DEEP, NF1),
	// SATAXPCIE2 (SATAGP2) = SATAGP2
	PAD_CFG_NF(GPP_E2, NONE, DEEP, NF1),
	// GPIO (CPU_GP0) // NC
	PAD_CFG_GPO(GPP_E3, 1, DEEP),
	// GPIO (DEVSLP0) // NC ("Remove DEVSLP_PCH")
	PAD_CFG_TERM_GPO(GPP_E4, 1, DN_20K, DEEP),
	// GPIO (DEVSLP1) // NC
	PAD_CFG_TERM_GPO(GPP_E5, 1, DN_20K, DEEP),
	// GPIO (DEVSLP2) // NC
	PAD_CFG_TERM_GPO(GPP_E6, 1, DN_20K, DEEP),
	// GPIO (CPU_GP1) <= TOUCH_INT#
	PAD_CFG_GPI_APIC_LOW(GPP_E7, NONE, DEEP),
	// SATALED# = SATA_LED#
	PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1),
	// USB2_OC0# = USB_OC#
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),
	// USB2_OC1# // USB_OC#
	PAD_CFG_NF(GPP_E10, NONE, DEEP, NF1),
	// USB2_OC2# // USB_OC#
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF1),
	// USB2_OC3# // USB_OC#
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF1),
	// DDPB_HPD0 <= DDI1_HDMI_HPD_CPU
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	// DDPC_HPD1 // NC ("Remove HPD")
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	// GPIO (DDPD_HPD2) <= EC_SMI#
	PAD_CFG_TERM_GPO(GPP_E15, 1, DN_20K, DEEP),	// TODO: _GPI_SMI?
	// GPIO (DDPE_HPD3) <= EC_SCI#
	PAD_CFG_GPI_SCI(GPP_E16, NONE, PLTRST, LEVEL, INVERT),
	// EDP_HPD <= eDP_HPD_CPU
	PAD_CFG_NF(GPP_E17, NONE, DEEP, NF1),
	// DDPB_CTRLCLK <=> DDI1_HDMI_CLK_CPU
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF1),
	// DDPB_CTRLDATA <=> DDI1_HDMI_DATA_CPU (Strap - Display Port B Detected)
	PAD_CFG_NF(GPP_E19, DN_20K, DEEP, NF1),
	// DDPC_CTRLCLK // NC
	PAD_CFG_NF(GPP_E20, NONE, DEEP, NF1),
	// DDPC_CTRLDATA => DDPC_CDA (Strap - Display Port C Detected)
	PAD_CFG_NF(GPP_E21, DN_20K, DEEP, NF1),
	// GPIO // NC; FIXME: Vendor configures as _BIDIRECT. Why?
	PAD_NC(GPP_E22, NONE),
	// GPIO => DDPD_CDA (Strap - Display Port D Detected)
	PAD_CFG_TERM_GPO(GPP_E23, 1, DN_20K, DEEP),

	/* ------- GPIO Community 2 ------- */

	/* -------- GPIO Group GPD -------- */
	// GPIO (BATLOW#) = BATLOW
	PAD_CFG_TERM_GPO(GPD0, 1, DN_20K, PWROK),
	// ACPRESENT <= AC_PRESENT
	PAD_CFG_NF(GPD1, NONE, PWROK, NF1),
	// GPIO (LAN_WAKE#) = GPD2/LAN_WAKE#
	PAD_CFG_TERM_GPO(GPD2, 1, DN_20K, PWROK),
	// PWRBTN# <= PM_PWRBTN#
	PAD_CFG_NF(GPD3, UP_20K, PWROK, NF1),
	// SLP_S3# => PM_SLP_S3#
	PAD_CFG_NF(GPD4, NONE, PWROK, NF1),
	// SLP_S4# => PM_SLP_S4#
	PAD_CFG_NF(GPD5, NONE, PWROK, NF1),
	// SLP_A# // NC
	PAD_CFG_NF(GPD6, DN_20K, PWROK, NF1),
	// GPIO (RSVD#AT15) // NC
	PAD_CFG_TERM_GPO(GPD7, 1, DN_20K, PWROK),
	// SUSCLK => SUS_CLK_CPU
	PAD_CFG_NF(GPD8, NONE, PWROK, NF1),
	// SLP_WLAN# // NC
	PAD_CFG_NF(GPD9, DN_20K, PWROK, NF1),
	// SLP_S5# // NC
	PAD_CFG_NF(GPD10, DN_20K, PWROK, NF1),
	// GPIO (LANPHYPC) // NC
	PAD_CFG_TERM_GPO(GPD11, 1, DN_20K, PWROK),

	/* ------- GPIO Community 3 ------- */

	/* ------- GPIO Group GPP_F ------- */
	// GPIO (I2S2_SCLK) // NC
	PAD_CFG_TERM_GPO(GPP_F0, 1, DN_20K, DEEP),
	// GPIO (I2S2_SFRM) // NC
	PAD_CFG_TERM_GPO(GPP_F1, 1, DN_20K, DEEP),
	// GPIO (I2S2_TXD) // NC
	PAD_CFG_TERM_GPO(GPP_F2, 1, DN_20K, DEEP),
	// GPIO (I2S2_RXD) // NC
	PAD_CFG_TERM_GPO(GPP_F3, 1, DN_20K, DEEP),
	// GPIO (I2C2_SDA) // NC
	PAD_CFG_TERM_GPO(GPP_F4, 1, DN_20K, DEEP),
	// GPIO (I2C2_SCL) // NC
	PAD_CFG_TERM_GPO(GPP_F5, 1, DN_20K, DEEP),
	// GPIO (I2C3_SDA) // NC
	PAD_CFG_TERM_GPO(GPP_F6, 1, DN_20K, DEEP),
	// GPIO (I2C3_SCL) // NC
	PAD_CFG_TERM_GPO(GPP_F7, 1, DN_20K, DEEP),
	// GPIO (I2C4_SDA) // NC
	PAD_CFG_TERM_GPO(GPP_F8, 1, DN_20K, DEEP),
	// GPIO (I2C4_SCL) // NC
	PAD_CFG_TERM_GPO(GPP_F9, 1, DN_20K, DEEP),
	// GPIO (I2C5_SDA/ISH_I2C2_SDA) // NC
	PAD_CFG_TERM_GPO(GPP_F10, 1, DN_20K, DEEP),
	// GPIO (I2C5_SCL/ISH_I2C2_SCL) // NC
	PAD_CFG_TERM_GPO(GPP_F11, 1, DN_20K, DEEP),
	// GPIO (EMMC_CMD) // NC
	PAD_CFG_TERM_GPO(GPP_F12, 1, DN_20K, DEEP),
	// GPIO (EMMC_DATA0) // NC
	PAD_CFG_TERM_GPO(GPP_F13, 1, DN_20K, DEEP),
	// GPIO (EMMC_DATA1) // NC
	PAD_CFG_TERM_GPO(GPP_F14, 1, DN_20K, DEEP),
	// GPIO (EMMC_DATA2) // NC
	PAD_CFG_TERM_GPO(GPP_F15, 1, DN_20K, DEEP),
	// GPIO (EMMC_DATA3) // NC
	PAD_CFG_TERM_GPO(GPP_F16, 1, DN_20K, DEEP),
	// GPIO (EMMC_DATA4) // NC
	PAD_CFG_TERM_GPO(GPP_F17, 1, DN_20K, DEEP),
	// GPIO (EMMC_DATA5) // NC
	PAD_CFG_TERM_GPO(GPP_F18, 1, DN_20K, DEEP),
	// GPIO (EMMC_DATA6) // NC
	PAD_CFG_TERM_GPO(GPP_F19, 1, DN_20K, DEEP),
	// GPIO (EMMC_DATA7) // NC
	PAD_CFG_TERM_GPO(GPP_F20, 1, DN_20K, DEEP),
	// GPIO (EMMC_RCLK) // NC
	PAD_CFG_TERM_GPO(GPP_F21, 1, DN_20K, DEEP),
	// GPIO (EMMC_CLK) // NC
	PAD_CFG_TERM_GPO(GPP_F22, 1, DN_20K, DEEP),
	// GPIO // NC
	PAD_CFG_GPI_APIC_HIGH(GPP_F23, NONE, DEEP),

	/* ------- GPIO Group GPP_G ------- */
	// GPIO (SD_CMD) // NC
	PAD_CFG_TERM_GPO(GPP_G0, 1, DN_20K, DEEP),
	// GPIO (SD_DATA0) // NC
	PAD_CFG_TERM_GPO(GPP_G1, 1, DN_20K, DEEP),
	// GPIO (SD_DATA1) // NC
	PAD_CFG_TERM_GPO(GPP_G2, 1, DN_20K, DEEP),
	// GPIO (SD_DATA2) // NC
	PAD_CFG_TERM_GPO(GPP_G3, 1, DN_20K, DEEP),
	// GPIO (SD_DATA3) // NC
	PAD_CFG_GPO(GPP_G4, 0, DEEP),
	// GPIO (SD_CD#) // NC
	PAD_CFG_TERM_GPO(GPP_G5, 1, DN_20K, DEEP),
	// GPIO (SD_CLK) // NC
	PAD_CFG_TERM_GPO(GPP_G6, 1, DN_20K, DEEP),
	// GPIO (SD_WP) // NC
	PAD_CFG_TERM_GPO(GPP_G7, 1, DN_20K, DEEP),
};

#endif /* CFG_GPIO_H */
