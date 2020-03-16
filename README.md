UTag
===
A Wii U plugin whichs sends the titles you play to your [RiiTag](https://tag.rc24.xyz/)!

## Building
For building you need:
* [wups](https://github.com/Maschell/WiiUPluginSystem)
* [wut](https://github.com/decaf-emu/wut)
* [libutils](https://github.com/Maschell/libutils/tree/wut) for common functions (WUT version).

Install them (in this order) according to their README's. Don't forget the dependencies of the libs itself.

You should then be able to run the following commands in the directory of this repository:
```bash
make clean
make
wiiload UTag.mod # If you wish to load the plugin over the network while the WUPL is running on the target console
```
The output `UTag.mod` file should be placed in `sd:/wiiu/plugins/`, and loaded with the WUPL.
