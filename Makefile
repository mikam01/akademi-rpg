.RECIPEPREFIX := >
DKA = /opt/devkitpro/devkitARM/bin
CC = $(DKA)/arm-none-eabi-gcc
OBJCOPY = $(DKA)/arm-none-eabi-objcopy
CFLAGS = -mthumb -mthumb-interwork -O2 -Wall -fno-strict-aliasing -specs=gba.specs

akademi_rpg.gba: main.c
> $(CC) $(CFLAGS) main.c -o akademi_rpg.elf
> $(OBJCOPY) -O binary akademi_rpg.elf akademi_rpg.gba
> -/opt/devkitpro/tools/bin/gbafix akademi_rpg.gba -tAKADEMIRPG
