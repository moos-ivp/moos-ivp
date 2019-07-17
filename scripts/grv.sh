#!/bin/bash

# Check that only one argument given and that it's an existing file
if [ "$#" -ne 1 ] || ! [ -e "$1" ]; then
  echo "Usage: $0 file.bhv" >&2
  exit 1
fi

# Check that the file given is a .bhv file
filename=$(basename "$1")
extension="${filename##*.}"
if ! [ "$extension" = "bhv" ]; then
  echo "Usage: $0 file.bhv" >&2
  exit 1
fi

# Check that the bhv2graphviz program is findable
if ! hash bhv2graphviz 2>/dev/null; then
    echo "bhv2graphviz not found. "
    exit 1;
fi

# Check that dot (graphviz package) installed and findable
if ! hash dot 2>/dev/null; then
    echo "dot not found. Perhaps install graphviz package"
    exit 1;
fi

# Generate the .dot file
bhv2graphviz $1 tmp.dot 

# Check that the .dot file was successfully generated
if ! [ -e "tmp.dot" ]; then
  echo "tmp.dot not created (bhv2graphviz failed)" >&2
  exit 1
fi

# Generate the PDF from the dot file using graphviz (dot)
dot -Tpdf tmp.dot > tmp.pdf

# Now we can remove the temportary .dot file
rm -f tmp.dot

# Open the PDF 
open tmp.pdf 

# Sleep for a bit to let the PDF viewer load the PDF before
# deleting
sleep 5
rm -f tmp.pdf



