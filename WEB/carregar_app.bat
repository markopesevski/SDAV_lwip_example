set ISE_VER=14.7

REM mira si es correcta la localitzacio del ISE
if NOT "%XILINX%" == "C:\Xilinx\%ISE_VER%\ISE_DS\ISE" GOTO CHECK64
GOTO CREATE_MFS

REM mira si realment es 64 bits i esta al lloc on toca
:CHECK64
if NOT "%PROCESSOR_ARCHITECTURE%" == "AMD64" GOTO SET32
call C:\Xilinx\%ISE_VER%\ISE_DS\settings64.bat
GOTO CREATE_MFS

REM si no es 64 bits nomes pot ser 32
:SET32
call C:\Xilinx\%ISE_VER%\ISE_DS\settings32.bat
GOTO CREATE_MFS

REM crea la imatge per la pagina web
:CREATE_MFS
del "image.mfs"
REM mfsgen -cvbf ./image.mfs 16 css index.html
mfsgen -cvbf ./image.mfs 16 .\webpage\*
GOTO DO_BATCH

REM si ha detectat el ISE correctament i la imatge ja ha estat creada
:DO_BATCH
del download.link
mklink download.link "..\SDK\SDK_Export\lwip_example_hw_platform\download.bit"
del "configuracio_batch_impact.cmd"
@echo setMode -bscan															>  configuracio_batch_impact.cmd
@echo setCable -target "digilent_plugin DEVICE=SN:000000000000 FREQUENCY=-1"	>> configuracio_batch_impact.cmd
@echo identify																	>> configuracio_batch_impact.cmd
@echo assignfile -p 1 -file download.link										>> configuracio_batch_impact.cmd
@echo program -p 1																>> configuracio_batch_impact.cmd
@echo quit																		>> configuracio_batch_impact.cmd
GOTO CONNECT

REM un cop ja ha carregat el bootloop a la Spartan6
REM connect mb mdm -port COMx -baud 115200 -timeout 5 -cable type digilent_plugin

:CONNECT
impact -batch configuracio_batch_impact.cmd
sleep 3
del "raw_apps.link"
mklink raw_apps.link "..\SDK\SDK_Export\raw_apps\Debug\raw_apps.elf"
del "xmd.ini"
@echo connect mb mdm -cable type digilent_plugin	>  xmd.ini
@echo rst											>> xmd.ini
@echo dow -data image.mfs 0xA5000000				>> xmd.ini
@echo dow raw_apps.link								>> xmd.ini
@echo run											>> xmd.ini
@echo exit											>> xmd.ini

xmd

:EOF
del "raw_apps.link"
del "download.link"
del "image.mfs"
del "xmd.ini"
del "configuracio_batch_impact.cmd"
