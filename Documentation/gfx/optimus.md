# Nvidia Optimus
## Terms
* iGPU : integrated GPU
* dGPU : discrete GPU
* MUX : multiplexer
* MUXed : Device that has got a MUX
* MUXless : Device that hasn't got a MUX

## High level view
[Nvidia Optimus] is mainly an ACPI feature to control power of the dGPU.
This significantly reduces the idle power consumption of mobile device.
If enabled by the firmware the graphics driver automatically poweres down
the GPU when it's idle and poweres it on whenever it's going to be used.

For comparison on Lenovo T520 with full screen brightness and nouveau loaded:
* Optimus disabled (dGPU powered on): 22,8W
* Optimus enabled (dGPU powered off): 13,8W

It also allows to toggle the MUX on ___Hybrid graphics___ capable devices
at runtime.

**Note:** coreboot doesn't support switching [Hybrid graphics] using
[Nvidia Optimus] at runtime.

## Technology
The Nvidia graphics driver uses the ACPI `_DSM` method on ___Nvidia Optimus___
enabled notebooks to advertise and control Optimus features.

The interface isn't documented and has been partially reverse engineered.

### Calls to _DSM

Arg1 must equal 0x100.

Arg2:
* 0 (FUNCTIONS_SUPPORTED)
  * Advertises supported functions
* 0x1a (NOUVEAU_DSM_OPTIMUS_CAPS)
  * Advertises support for Optimus and features
* 0x1b (NOUVEAU_DSM_OPTIMUS_FLAGS)
  * Shows the current hardware state

Returns 0x80000002 on error.

## Adding board support
The mainboard code must select `CONFIG_DRIVERS_NVIDIA_OPTIMUS` if the following
ACPI methods have been implemented:

* `\_SB.PCI0.PEGP.DEV0._STA ()` that returns:
  *  0x0 if no dGPU present
  *  0x5 if dGPU is powered off
  *  0xf if dGPU is powered on
* `\_SB.PCI0.PEGP.DEV0._ON ()`
  *  poweres on the dGPU (including reset sequence)
* `\_SB.PCI0.PEGP.DEV0._OFF ()`
  *  poweres off the dGPU (including reset sequence)

## Adding SoC support
The ACPI code in `src/drivers/nvidia/optimus/acpi/optimus.asl` needs to be
included in the northbridge ACPI code if `CONFIG_NVIDIA_OPTIMUS` is set.

It expects the Nvidia GPU to be at `\_SB.PCI0.PEGP.DEV0` and thus the PCIe
root port to be `\_SB.PCI0.PEGP`.

The Nvidia Optimus code needs the following functions implemented in the
northbridge ACPI code:

* `\_SB.PCI0.PEGP._PS0 ()`
  * Does PCIe link training
* `\_SB.PCI0.PEGP._PS3 ()`
  * Disables the PCIe link
* `\_SB.PCI0.PEGP.PWRR`
  * PowerResources controlling the PCIe clock/power
* `\_SB.PCI0.PEGP._PR0 ()`
  * Reverences `\_SB.PCI0.PEGP.PWRR`
* `\_SB.PCI0.PEGP._PR3 ()`
  * Reverences `\_SB.PCI0.PEGP.PWRR`
* `\_SB.PCI0.PEGP._ON ()`
  * Turns the PCIe clock/power on
* `\_SB.PCI0.PEGP._OFF ()`
  * Turns the PCIe clock/power off


## Proprietary Option ROM usage
Please note that you have to use the Nvidia/AMD *VGA Option ROM* to drive
the second GPU. For the integrated Intel GPU you are free to use the
*VGA Option ROM* or use *Native Graphics Init* or [libgfxinit].

[Hybrid graphics]: hybrid_dual_graphics.md
[Nvidia Optimus]: https://de.wikipedia.org/wiki/Nvidia_Optimus
[libgfxinit]: libgfxinit.md

