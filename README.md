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
