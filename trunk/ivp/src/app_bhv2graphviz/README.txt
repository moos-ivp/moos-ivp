bhv2graphviz writes a GraphViz "dot" file from the hierarchical mode 
declaration of a MOOS-IvP behavior (.bhv) file. From the dot file, you 
can generate a graphical view of the mode structure.

To use, install graphviz

On Ubuntu: "sudo apt-get install graphviz"
On OS X:   "sudo port install graphviz"

Run these commands (for example to generate a pdf output):

bhv2graphviz /path/to/vehicle.bhv vehicle.dot
dot -Tpdf vehicle.dot > vehicle-bhv-modes.pdf
