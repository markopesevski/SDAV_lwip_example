setMode -bscan
setCable -target "digilent_plugin DEVICE=SN:000000000000 FREQUENCY=-1"
identify
assignfile -p 1 -file bootloop.bit
program -p 1
quit
