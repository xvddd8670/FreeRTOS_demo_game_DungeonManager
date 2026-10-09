file build/firmware.elf
target extended-remote :3333
monitor reset halt
load

define rebuild
  shell make
  load
  monitor reset halt
end
