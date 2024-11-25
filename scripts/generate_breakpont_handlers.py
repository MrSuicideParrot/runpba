from pwn import *


def generate_breakpoints(exe, targets):
    for i  in targets:
        counter = 0
        if type(i) is tuple:
            aux = i
            i = aux[0]
            counter = aux[1]
        addr = exe.symbols[i]
        print(f'#{i} -> {hex(addr+counter)}')
        print(f'b *{hex(addr)}')

exe_s = ELF('trusted-firmware-m/build/bin/tfm_s.elf', checksec=False)

targets_s = [
    'SecureFault_Handler',
    'UsageFault_Handler',
    'BusFault_Handler',
    'MemManage_Handler',
    'HardFault_Handler',
    'tfm_pacbti_entry',
    'pacbti_setContextFrame',
    'tfm_spm_partition_psa_irq_enable'
]

generate_breakpoints(exe_s, targets_s)

#138

exe_ns = ELF('trusted-firmware-m/build/bin/tfm_ns.elf', checksec=False)
targets_ns = [
    ('echo_processing', 138),
]


generate_breakpoints(exe_ns, targets_ns)
