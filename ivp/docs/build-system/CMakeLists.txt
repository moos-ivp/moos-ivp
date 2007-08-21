ADD_CUSTOM_COMMAND(OUTPUT file-structure.eps
   COMMAND dot -Tps file-structure.dot -o file-structure.ps
   COMMAND ps2ps file-structure.ps file-structure.fixed.ps
   COMMAND ps2epsi file-structure.fixed.ps file-structure.eps
   DEPENDS file-structure.dot
   )

ADD_CUSTOM_COMMAND(OUTPUT cmake-file-tree.eps
   COMMAND dot -Gsize="7,10" -Gpage="8.5,11" -Tps cmake-file-tree.dot -o cmake-file-tree.ps
   COMMAND ps2ps cmake-file-tree.ps cmake-file-tree.fixed.ps
   COMMAND ps2epsi cmake-file-tree.fixed.ps cmake-file-tree.eps
   DEPENDS cmake-file-tree.dot
   )

ADD_CUSTOM_COMMAND(OUTPUT cmake-data-flow.eps
   COMMAND fig2dev -L eps cmake-data-flow.fig cmake-data-flow.eps
   DEPENDS cmake-data-flow.fig
   )

ADD_CUSTOM_TARGET(build-system.pdf ALL
   COMMAND latex main.tex
   COMMAND dvipdf main.dvi build-system.pdf
   DEPENDS 
      file-structure.eps
      cmake-file-tree.eps
      cmake-data-flow.eps
   )
