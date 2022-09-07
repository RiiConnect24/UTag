FROM wiiuenv/devkitppc:20220907

COPY --from=wiiuenv/wiiupluginsystem:20220904 /artifacts $DEVKITPRO

WORKDIR project
