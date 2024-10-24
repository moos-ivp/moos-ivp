ARG FROM=moosivp/moos-ivp:trunk
FROM $FROM

LABEL maintainer="Conlan Cesar <conlanc@csail.mit.edu>"

# Install required MOOS dependencies
USER root
RUN apt-get update -y && apt-get install -y cmake build-essential subversion git libtiff5-dev libfltk1.3-dev freeglut3-dev libpng-dev libjpeg-dev libxinerama-dev libxft-dev && apt-get clean
USER moos

# Build the MOOS-IvP tools
RUN cd "$HOME/moos-ivp" && ./build-moos.sh && ./build-ivp.sh

