FROM wiiuenv/devkitppc:20200625

RUN apt update && \
    apt install -y build-essential cmake

RUN git clone --recursive https://github.com/Maschell/WiiUPluginSystem.git && \
    cd WiiUPluginSystem && \
    make && make install

ENV WUT_ROOT=/opt/devkitpro/wut

COPY --from=wiiuwut/libutils:0.1 /artifacts $WUT_ROOT

WORKDIR project

# == NOTES ==
# docker build . -t wups-builder
# docker run -it --rm -v ${PWD}:/project wups-builder make
