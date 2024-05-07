

# ALBW-Style Meter++
A "Twilight Princess" mod that provides the game with core mechanics from "A Link Between Worlds", a 3DS Zelda game.
`Currently only supports TP GC US and TP Wii US 1.2`

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





If you wish to inject the mod into an existing Wii save instead of generating an empty save, you can use the `nandpack.py` script to do that. Here is an example:

```shell
$ ./bin/nandpack.py inject -i 3 -g us2 -f "REL Example" REL.wus2.rel data.bin REL.wus2.bin
```

Where:
- `-i 3`: inject the mod into the 3rd slot.
- `-g us2`: specify we're injecting into an NTSCU 1.2 save.
- `-f "REL Example"`: set the name of the slot to `REL Example`
- `REL.wus2.rel`: this is the file containing the mod itself that we want to inject into the save.
- `data.bin`: this is the save file we want to inject the mod into.
- `REL.wus2.bin`: this is the resulting save file with the mod injected into it.

---
This mod is powered by [libtp_rel](//github.com/zsrtp/libtp_rel) and licensed under [GPLv3](/LICENSE) | (C) [AECX](//github.com/AECX), [LunarSoap](//github.com/lunarsoap5), [Zephiles](//github.com/Zephiles), [kipcode66](//github.com/kipcode66)
