# Butuh devkitARM (devkitPro). Build: make  -> akademi_rpg.gba
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
CFLAGS  = -mthumb -mthumb-interwork -O2 -Wall -fno-strict-aliasing -specs=gba.specs

akademi_rpg.gba: main.c
	$(CC) $(CFLAGS) main.c -o akademi_rpg.elf
	$(OBJCOPY) -O binary akademi_rpg.elf akademi_rpg.gba
	-gbafix akademi_rpg.gba -tAKADEMIRPG

clean:
	rm -f *.elf *.gba
