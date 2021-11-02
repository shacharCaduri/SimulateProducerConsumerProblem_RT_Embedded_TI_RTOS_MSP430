#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/tirtos_msp43x_2_14_03_28/packages;C:/ti/tirtos_msp43x_2_14_03_28/products/bios_6_42_03_35/packages;C:/ti/tirtos_msp43x_2_14_03_28/products/uia_2_00_02_39/packages;C:/ti/ccsv6/ccs_base;C:/TI_RTOS/Workspace/RT_FinProj_Part1_HadadYanousYazdiKaduri/.config
override XDCROOT = C:/ti/xdctools_3_31_01_33_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/tirtos_msp43x_2_14_03_28/packages;C:/ti/tirtos_msp43x_2_14_03_28/products/bios_6_42_03_35/packages;C:/ti/tirtos_msp43x_2_14_03_28/products/uia_2_00_02_39/packages;C:/ti/ccsv6/ccs_base;C:/TI_RTOS/Workspace/RT_FinProj_Part1_HadadYanousYazdiKaduri/.config;C:/ti/xdctools_3_31_01_33_core/packages;..
HOSTOS = Windows
endif
