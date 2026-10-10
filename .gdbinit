file build/firmware.elf
target extended-remote :3333
monitor reset halt
load
monitor rtt setup 0x20000000 0x5000 "SEGGER RTT"
monitor rtt start
monitor rtt server start 9090 0

define rebuild
  shell make
  monitor rtt stop
  load
  monitor reset halt
  monitor rtt start
end
