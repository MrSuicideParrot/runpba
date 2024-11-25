#!/bin/bash

# -I -A

VHT_Corstone_SSE-310  -I -A  -C mps3_board.visualisation.disable-visualisation=1 -C mps3_board.smsc_91c111.enabled=1 -C mps3_board.hostbridge.userNetworking=1 -C mps3_board.telnetterminal0.start_telnet=0 -C mps3_board.uart0.out_file=- -C mps3_board.uart0.unbuffered_output=1 --stat -C mps3_board.DISABLE_GATING=1 -C cpu0.ID_ISAR5.PACBTI=2  -a 'cpu0*=trusted-firmware-m/build/bin/bl2.axf' --data ./trusted-firmware-m/build/bin/tfm_s_ns_signed.bin@0x38000000
