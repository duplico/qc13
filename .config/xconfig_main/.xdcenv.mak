#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = D:/ti/grace_3_10_00_82/packages;D:/ti/msp430/MSPWare_3_30_00_18/driverlib/packages;D:/ti/msp430/MSPWare_3_30_00_18/driverlib;D:/ti/ccsv6/ccs_base;C:/Users/George/workspace_v6_1/qc13/.config
override XDCROOT = D:/ti/xdctools_3_31_01_33_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = D:/ti/grace_3_10_00_82/packages;D:/ti/msp430/MSPWare_3_30_00_18/driverlib/packages;D:/ti/msp430/MSPWare_3_30_00_18/driverlib;D:/ti/ccsv6/ccs_base;C:/Users/George/workspace_v6_1/qc13/.config;D:/ti/xdctools_3_31_01_33_core/packages;..
HOSTOS = Windows
endif
