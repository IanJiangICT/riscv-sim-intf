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
```
UserApp   SimProxy                    sc_func
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Start     Init()                      sc_init()
          RunUntil(0x800)             loop sc_run_next() until(npc == 0x800)
                                      sc_get_state()
          <S0: pc=800 npc=804 regs=x>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Decode    DecodeIStram()              sc_decode()
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute   RunAt(0x804)                sc_force_pc(0x804)
                                      sc_get_state()
          <S0: pc=800 npc=804 regs=x>
          <S1: pc=804 npc=808 regs=x>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute   RunAt(0x808)                sc_force_pc(0x808)
                                      sc_get_state()
          <S0: pc=800 npc=804 regs=x>
          <S1: pc=804 npc=808 regs=x>
          <S2: pc=808 npc=8A0 regs=x>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute   RunAt(0x80C)                sc_force_pc(0x80C)
(wrong)                               sc_get_state()
          <S0: pc=800 npc=804 regs=x>
          <S1: pc=804 npc=808 regs=x>
          <S2: pc=808 npc=8A0 regs=x>
          <S3: pc=80C npc=810 regs=x>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute   RunAt(0x810)                sc_force_pc(0x810)
(wrong)                               sc_get_state()
          <S0: pc=800 npc=804 regs=x>
          <S1: pc=804 npc=808 regs=x>
          <S2: pc=808 npc=8A0 regs=x>
          <S3: pc=80C npc=810 regs=x>
          <S4: pc=810 npc=814 regs=x>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Revert    RevertTo(0x808)             sc_set_state(regs)
          <S0: pc=800 npc=804 regs=x>
          <S1: pc=804 npc=808 regs=x>
          <S2: pc=808 npc=8A0 regs=x>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute   RunAt(0x8A0)                sc_force_pc(0x8A0)
                                      sc_get_state()
          <S0: pc=800 npc=804 regs=x>
          <S1: pc=804 npc=808 regs=x>
          <S2: pc=808 npc=8A0 regs=x>
          <S3: pc=8A0 npc=8A4 regs=x>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Decode    DecodeIStram()              sc_decode()
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execute ...
```
