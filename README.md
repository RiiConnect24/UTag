UTag
===
A Wii U plugin which sends the titles you play to your [RiiTag](https://tag.rc24.xyz/)!

## Using

1. Download the [latest release](https://github.com/RiiConnect24/UTag/releases)
2. Place the `utag.wps` inside `SD://wiiu/plugins/`
3. Create a text file in `SD://wiiu/utag.txt`
4. Paste your RiiTag key inside (you can find it on the RiiTag website
   under ["Edit Your Tag"](https://tag.rc24.xyz/edit))

**NOTE:** All titles except applets (browser, etc.), system and Wii Virtual Console titles are counted, but not all
games have covers.

## Building

For building you need:

* [wups](https://github.com/wiiu-env/WiiUPluginSystem)
* [wut](https://github.com/devkitPro/wut)

You should then be able to run the following commands in the directory of this repository:

```bash
make clean
make
wiiload utag.wps # If you wish to load the plugin over the network while the WUPL is running on the target console
```
