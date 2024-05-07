

# ALBW-Style Meter++
A "Twilight Princess" mod that provides the game with core mechanics from "A Link Between Worlds", a 3DS Zelda game.

## All Feature Include:
### Items Utilizing Oil Meter
There are a total of 12 items that affect and get affected by the value of the meter:

* Slingshot
* Boomerang
* Bow
* Normal Bombs
* Water Bombs
* Bomblings
* Clawshot
* Double Clawshot
* Spinner
* Dominion Rod
* Ball and Chain
* Armor

### Meter Expansion
When you start the game, the size of the meter will be small. After acquiring the master sword, it will increase to double the size.

### Meter Refill Drops
Slingshot seeds and Arrow drops have been replaced with red and green bottles. The green bottles will refill double the amount that the red bottles do for the meter.

### Game Over
When you get a game over, Link will lose all items he currently has that utilize the meter. If you get a game over while you're in a dungeon, you will be warped to the dungeon entrance. You also have the option to warp to Ordon village instead.

### Item Renting Shop
In Ordon Village, there is a sign next to Link's house. If you interact with it, you'll open up a menu where you can rent any of the 12 items that utilize the meter. As mentioned in the Game Over
section, if you get a game over, you'll lose all items which means you'll have to rent them again if you want them back.

## Clips:
### 




## Getting started
---
This repository provides you with a solid start, you should use the predefined main function as an entry point to your mod...
Please check Lines 19 through 30 in the Makefile to change some of the definitions and names.

You can refer to the [Randomizer](//github.com/zsrtp/Randomizer) Project for reference.

## Wiki
---
### General
Our [wiki](//wiki.tprandomizer.com) is meant to be a central source of information for TP with a slightly technical focus we provide the most crucial information such as Stages & Rooms or item ids. If you're looking for information you should be looking there or ask in the [dev discord](//discord.gg/aZx8ZFcSPy).

### Coding guidelines
If you intend to share this code with the @zsrtp community, please follow the [guidelines](//wiki.tprandomizer.com/index.php?title=Dev:Coding_Guidelines) as strictly as possible.

## Building
---
**Repository**

*You need to have git installed ([windows](https://git-scm.com/download/win))*

Open a CMD and navigate to where you want your code to be:
```
C:\>mkdir Code
C:\>cd Code
C:\Code>git clone https://github.com/zsrtp/REL.git
Cloning into REL
remote: Enumerating objects: 132, done.
remote: Counting objects: 100% (132/132), done.
remote: Compressing objects: 100% (87/87), done.
remote: Total 132 (delta 56), reused 98 (delta 29), pack-reused 0
Receiving objects: 100% (132/132), 1.24 MiB | 6.45 MiB/s, done.
Resolving deltas: 100% (56/56), done.
```
Download and upate libtp_rel
```
C:\Code\REL> git submodule update --init --remote
```
Done.

**Dependencies**
  * DevkitPPC comes with [devkitPro](//github.com/devkitPro/installer/releases)
  * [Python3](//www.python.org/downloads) (should be added to PATH or you will likely need Makefile adjustments)
  * [pycryptodomex](https://pypi.org/project/pycryptodomex/) (For Wii only, dependency of [`nandpack.py`](https://github.com/kipcode66/wii_tools))
  * Optional: [Visual Studio Code](//code.visualstudio.com) with [GC Settings](//wiki.tprandomizer.com/index.php?title=Dev:Visual_Studio_Code)

**Compiling**

*devkitPro comes with its own MSys2 shell, it is recommended to use it for compilation.

This is an example of how to build for the NTSC-U version of the Game
```shell
$ cd /c/Path/To/REL
$ make us
main.cpp
cxx.cpp
memory.cpp
patch.cpp
rel.cpp
tools.cpp
timer.cpp
stages.cpp
console.cpp
card.cpp
color_utils.cpp
texture_utils.cpp
restfpr_x.s
restgpr_x.s
linking ... REL.us.elf
output ... REL.us.rel
packing ... REL.us.gci
```
The `REL.us.gci` (`REL.wus0.bin` for wii) (other versions are: `jp`, `eu`, `wus0`, `wus2`, `wjp`, and `weu`, as well as two other build target `gcn` and `wii` which build all the version for the specified platform) file is now containing our custom REL. Load it with the REL Loader cheat or a hacked save file that contains the loader, both of which can be found on the wiki [REL Loader](//wiki.tprandomizer.com/index.php?title=REL_Loader) page.

##### Injecting into an existing Wii save

If you wish to inject the mod into an existing Wii save instead of generating an empty save, you can use the `nandpack.py` script to do that. Here is an example:

```shell
$ ./bin/nandpack.py inject -i 3 -g us0 -f "REL Example" REL.wus0.rel data.bin REL.wus0.bin
```

Where:
- `-i 3`: inject the mod into the 3rd slot.
- `-g us0`: specify we're injecting into an NTSCU 1.0 save.
- `-f "REL Example"`: set the name of the slot to `REL Example`
- `REL.wus0.rel`: this is the file containing the mod itself that we want to inject into the save.
- `data.bin`: this is the save file we want to inject the mod into.
- `REL.wus0.bin`: this is the resulting save file with the mod injected into it.

---
This mod is powered by [libtp_rel](//github.com/zsrtp/libtp_rel) and licensed under [GPLv3](/LICENSE) | (C) [AECX](//github.com/AECX), [LunarSoap](//github.com/lunarsoap5), [Zephiles](//github.com/Zephiles), [kipcode66](//github.com/kipcode66)
