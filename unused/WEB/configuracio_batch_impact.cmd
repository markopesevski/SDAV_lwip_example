setMode -bscan																	
setCable -target "digilent_plugin DEVICE=SN:000000000000 FREQUENCY=-1"			
identify																			
assignfile -p 1 -file ..\SDK\SDK_Export\lwip_example_hw_platform\download.bit		
program -p 1																		
quit																				
