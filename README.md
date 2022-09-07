UTag
===
A Wii U plugin which sends the titles you play to your [RiiTag](https://tag.rc24.xyz/)!

## Using

1. Download the [latest stable release](https://github.com/RiiConnect24/UTag/releases/latest)
2. Place the `utag.wps` inside `SD://wiiu/environments/aroma/plugins`
3. Create a text file in `SD://wiiu/utag.txt`
4. Paste your RiiTag key inside (you can find it on the RiiTag website
   under ["Edit Your Tag"](https://tag.rc24.xyz/edit))

**NOTE:** All titles except applets (browser, etc.) and system titles are counted, but not all
games have covers.

## Building

For building you need:

* [wups](https://github.com/wiiu-env/WiiUPluginSystem)
* [wut](https://github.com/devkitPro/wut)
