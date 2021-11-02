## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e430X linker.cmd package/cfg/empty_pe430X.oe430X

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/empty_pe430X.xdl
	$(SED) 's"^\"\(package/cfg/empty_pe430Xcfg.cmd\)\"$""\"C:/TI_RTOS/Workspace/RT_FinProj_Part1_HadadYanousYazdiKaduri/.config/xconfig_empty/\1\""' package/cfg/empty_pe430X.xdl > $@
	-$(SETDATE) -r:max package/cfg/empty_pe430X.h compiler.opt compiler.opt.defs
