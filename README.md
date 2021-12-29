# riscv-sim-intf
Interface of RISC-V simulators, such as Spike.

```
              |----------|
              | User App |
              |----------|
                /  or  \
   (SV Module) /        \   (SV Class)
    |------------|    |------------|
    |  sim_intf  |    |  SimProxy  |
    |------------|    |------------|
               \        /
              |----------|        (TCP Socket)      |-------|
              |  sc_func | <=======================>| Spike |
              |----------|                          |-------|
```

## Getting started

1. Set path of VCS, Verdi and Spike

2. Get source codes of *riscv-sim-intf*

3. Build example tests

   ```bash
   cd riscv-sim-intf/test
   make -f Makefile-vcs
   ls sim_intf_tb?
   ```

4. Run one test, for example *sim_intf_tb1*

   ```bash
   ./sim_intf_tb1
   ./sim_intf_tb1 +DUMP_FSDB    # Dump waves (supported by TB)
   nWave sim_intf_tb.fsdb       # Check waves
   less spike-run.log           # Check log of Spike
   ```

## Integrating into user application

Module parameters are provided for customization:

- *INPUT_ELF* : Give full pathname of ELF as input of Spike

- *START_PC* : Define starting PC
- *SIM_PORT* : Use a different TCP Port to avoid conflicts with other running instance

Source files to compile:

- src/sim_intf.sv : Implement of module *sim_intf*
- src/sc_func.c : State-Control operations between *sim_intf* and Spike

## Workflow of SimProxy

StateQueueCapacity = 4

```
UserApp  SimProxy         sc_func
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Start    Init()           sc_init()               MemPages[                        ]
         RunUntil(0x800)  loop {                  <S0: pc=800 npc=804 regs=x memop0>
                                 sc_run_next()    <S1:                             >
                                 sc_save_state()  <S2:                             >
                          } until(npc == 0x800)   <S3:                             >
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Decode   DecodeIStram()   sc_decode()
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute  RunAt(0x804)     sc_force_pc(0x804)      MemPages[                        ]
                          sc_save_state()         <S0: pc=800 npc=804 regs=x memop0>
                                                  <S1: pc=804 npc=808 regs=x memop1>
                                                  <S2:                             >
                                                  <S3:                             >
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute  RunAt(0x808)     sc_force_pc(0x808)      MemPages[                        ]
                          sc_save_state()         <S0: pc=800 npc=804 regs=x memop0>
                                                  <S1: pc=804 npc=808 regs=x memop1>
                                                  <S2: pc=808 npc=8A0 regs=x memop2>
                                                  <S3:                             >
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute  RunAt(0x80C)     sc_force_pc(0x80C)      MemPages[                        ]
(wrong)                   sc_save_state()         <S0: pc=800 npc=804 regs=x memop0>
                                                  <S1: pc=804 npc=808 regs=x memop1>
                                                  <S2: pc=808 npc=8A0 regs=x memop2>
                                                  <S3: pc=80C npc=810 regs=x memop3>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute  RunAt(0x810)     sc_force_pc(0x810)      MemPages[     mem0               ]
(wrong)                                           <                       ^--memop0>
                                                  <S0: pc=804 npc=808 regs=x memop1>
                                                  <S1: pc=808 npc=8A0 regs=x memop2>
                                                  <S2: pc=80C npc=810 regs=x memop3>
                                                  <S3: pc=810 npc=814 regs=x memop4>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Revert   RevertTo(0x808)  sc_recover_state(0x808) MemPages[     mem0               ]
                            - sc_recover_mem()    <S0: pc=804 npc=808 regs=x memop1>
                            - sc_recover_reg()    <S1: pc=808 npc=8A0 regs=x memop2>
                                                  <S2:                             >
                                                  <S3:                             >
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute  RunAt(0x8A0)     sc_force_pc(0x8A0)      MemPages[     mem0               ]
                          sc_save_state()         <S0: pc=804 npc=808 regs=x memop1>
                                                  <S1: pc=808 npc=8A0 regs=x memop2>
                                                  <S2: pc=8A0 npc=8A4 regs=x memop5>
                                                  <S3:                             >
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Decode    DecodeIStram()  sc_decode()
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute  RunAt(0x8A4)     sc_force_pc(0x8A4)      MemPages[     mem0               ]
                          sc_save_state()         <S0: pc=804 npc=808 regs=x memop1>
                                                  <S1: pc=808 npc=8A0 regs=x memop2>
                                                  <S2: pc=8A0 npc=8A4 regs=x memop5>
                                                  <S3: pc=8A4 npc=8A8 regs=x memop6>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute  RunAt(0x8A8)     sc_force_pc(0x8A8)      MemPages[     mem0     mem1      ]
                          sc_save_state()         <                       ^--memop1>
                                                  <S0: pc=808 npc=8A0 regs=x memop2>
                                                  <S1: pc=8A0 npc=8A4 regs=x memop5>
                                                  <S2: pc=8A4 npc=8A8 regs=x memop6>
                                                  <S3: pc=8A8 npc=8AC regs=x memop7>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute ...
```
