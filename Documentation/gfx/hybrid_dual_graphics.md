# Proprietary notebook graphics
## Terms
* iGPU : integrated GPU
* dGPU : discrete GPU
* MUX : multiplexer
* MUXed : Device that has got a MUX
* MUXless : Device that hasn't got a MUX

## Nvidia Optimus™
[Nvidia Optimus] is mainly an ACPI feature to control power of the dGPU.
It also allows to toggle the MUX on ___Hybrid graphics___ capable devices
at runtime. For more details check [Wikipedia Nvidia Optimus].

**Note:** coreboot does support ___Nvidia Optimus___ since 4.10.

## Hybrid Graphics™ / Switchable Graphics™

[Hybrid Graphics] is a feature used by a notebook that has two GPUs
and a MUX to switch the notebook's panel to one of the GPUs.
Only one GPU is active at time, allowing to save power and making
display handling on software side easy.

coreboot does support ___hybrid graphics___ on some Lenovo mainboards and
you can select the GPU to use at boot (using a CMOS setting, through
nvramtool or nvramcui).

___Switchable Graphics___ allows to toggle the MUX at runtime, but it needs
complex graphics drivers handling and platform specific power management
features like ___Nvidia Optimus___ or ___ATI PowerXpress___.
You might need to install additional drivers or userspace software.
See [Bumblebee on archlinux wiki] or [GitHub Bumblebee Project]
for more details.

## ATI PowerXpress™ (Mobile)

___ATI PowerXpress___ automatically switches the graphics depending on
AC/battery state and user preference. It is ATI's solution to counter
[Nvidia Optimus].

**Note:** coreboot doesn't support ___ATI PowerXpress___.

## Dual graphics™

___Dual graphics___ is a feature used by a notebook that has two GPUs and
where the iGPU is connected to the panel and where the dGPU used to offload
rendering to. It can be used on MUXed devices and MUXless devices.

__Warning__: Enabling two GPUs draws significantly more power ! You need
[Nvidia Optimus] or ___ATI PowerXpress___ to disable the dGPU's power
when not in use! ___ATI PowerXpress___ hasn't been implemented in coreboot.

## Proprietary Option ROM usage
Please note that you have to use the Nvidia/AMD [VGA Option ROM] to drive
the second GPU. For the integrated Intel GPU you are free to use the
[VGA Option ROM] or use *Native Graphics Init* or [libgfxinit] to use
hybrid graphics.

[VGA Option ROM]: https://en.wikipedia.org/wiki/Option_ROM
[libgfxinit]: libgfxinit.md
[Bumblebee on archlinux wiki]: https://wiki.archlinux.org/index.php/bumblebee
[GitHub Bumblebee Project]: https://github.com/Bumblebee-Project/bbswitch/blob/master/README.md
[Wikipedia Nvidia Optimus]: https://en.wikipedia.org/wiki/Nvidia_Optimus
[Nvidia Optimus]: optimus.md
[Hybrid Graphics]: https://en.wikipedia.org/wiki/AMD_Hybrid_Graphics
