ADD_CUSTOM_COMMAND(OUTPUT file-structure.eps
   COMMAND dot -Tps file-structure.dot > file-structure.ps
   COMMAND ps2ps file-structure.ps file-structure.fixed.ps
   COMMAND ps2epsi file-structure.fixed.ps file-structure.eps
   )

ADD_CUSTOM_TARGET(build-system.pdf ALL
   COMMAND latex main.tex
   COMMAND dvipdf main.dvi build-system.pdf
   DEPENDS file-structure.eps
   )
