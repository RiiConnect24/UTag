# UTag

[![CI-Release](https://github.com/RiiConnect24/UTag/actions/workflows/ci.yml/badge.svg)](https://github.com/RiiConnect24/UTag/actions/workflows/ci.yml)

A Wii U plugin which sends the titles you play to your [RiiTag](https://tag.rc24.xyz/)!

**This will only work with [Aroma](https://aroma.foryour.cafe/)!**

## Usage

1. Install [Aroma](https://aroma.foryour.cafe/)
2. Download the [latest stable release](https://github.com/RiiConnect24/UTag/releases/latest)
3. Place the `utag.wps` inside `SD://wiiu/environments/aroma/plugins/`
4. Create a text file in `SD://wiiu/utag.txt`
5. Paste your RiiTag key inside (you can find it on the RiiTag website
   under ["Account"](https://tag.rc24.xyz/account)). Do NOT use Windows Notepad!

**NOTE:** All titles except applets (browser, etc.) and system titles are counted, but not all
games have covers.

## Build flags

### Logging

Building via `make` only logs errors (via OSReport). To enable logging via the [LoggingModule](https://github.com/wiiu-env/LoggingModule) set `DEBUG` to `1` or `VERBOSE`.

`make` Logs errors only (via OSReport).  
`make DEBUG=1` Enables information and error logging via [LoggingModule](https://github.com/wiiu-env/LoggingModule).  
`make DEBUG=VERBOSE` Enables verbose information and error logging via [LoggingModule](https://github.com/wiiu-env/LoggingModule).

If the [LoggingModule](https://github.com/wiiu-env/LoggingModule) is not present, it'll fallback to UDP (Port 4405) and [CafeOS](https://github.com/wiiu-env/USBSerialLoggingModule) logging.

### Server

You can pass the `SERVER` variable to `make` to change the server domain. This makes it possible to use [your own RiiTag instance](https://github.com/WiiDatabase/RiiTag-Next). It defaults to `tag.rc24.xyz` (http).

`make` Use `tag.rc24.xyz`
`make SERVER=example.com` Use `example.com` as server

## Building using the Dockerfile

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# Build docker image (only needed once)
docker build . -t utag-builder

# make
docker run -it --rm -v ${PWD}:/project utag-builder make

# make clean
docker run -it --rm -v ${PWD}:/project utag-builder make clean
```

## Format the code via docker

`docker run --rm -v ${PWD}:/src wiiuenv/clang-format:13.0.0-2 -r ./src -i`
