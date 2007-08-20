ADD_CUSTOM_COMMAND(OUTPUT file-structure.eps
   COMMAND dot -Tps file-structure.dot -o file-structure.ps
   COMMAND ps2ps file-structure.ps file-structure.fixed.ps
   COMMAND ps2epsi file-structure.fixed.ps file-structure.eps
   )

ADD_CUSTOM_COMMAND(OUTPUT cmake-file-tree.eps
   COMMAND dot -Tps cmake-file-tree.dot -o cmake-file-tree.ps
   COMMAND ps2ps cmake-file-tree.ps cmake-file-tree.fixed.ps
   COMMAND ps2epsi cmake-file-tree.fixed.ps cmake-file-tree.eps
   )

ADD_CUSTOM_TARGET(build-system.pdf ALL
   COMMAND latex main.tex
   COMMAND dvipdf main.dvi build-system.pdf
   DEPENDS 
      file-structure.eps
      cmake-file-tree.eps
   )
