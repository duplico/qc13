# invoke SourceDir generated makefile for main.pe430X
main.pe430X: .libraries,main.pe430X
.libraries,main.pe430X: package/cfg/main_pe430X.xdl
	$(MAKE) -f C:\Users\George\workspace_v6_1\qc13/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\George\workspace_v6_1\qc13/src/makefile.libs clean

