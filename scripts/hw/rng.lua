-- if the emulator is at a path like: path/to/dtemu.exe
-- then put this in a path like:      path/to/hw/rng.lua

-- interrupt values
local INT_GENERATE = 0;
local INT_SEED = 1;

function interrupt(cpu)
  -- cpu is a table that lets you do things to the CPU.
  if (cpu.registers.A == INT_GENERATE) then
    cpu.registers.B = math.random(0x0, 0xFFFF);
  elseif (cpu.registers.A == INT_SEED) then
    math.randomseed(cpu.registers.B);
  end
end

function cycle(cpu)
  -- cpu is a table that lets you do things to the CPU.
  -- according to hardware standards, this is not called
  -- until the first interrupt has been received and handled,
  -- however at the moment the toolchain ignores this
  -- requirement and it's called anyway :P
  
  --[[print(string.format("0x%X", cpu.ram[0x0]))
  print(cpu.registers.Z)
  print(cpu.registers.PC)
  cpu.registers.Z = math.random(0x0, 0xFFFF);
  cpu.ram[0x0] = 0x0;]]--
end

function write(cpu, pos)
  -- cpu is a table that lets you do things to the CPU.
  -- pos is the memory address that was written to.
end

MODULE = {
  Type = "Hardware",
  Name = "RNG Hardware",
  Version = "1.0"
};

HARDWARE = {
  ID = 0x739df773,
  Version = 0x0001,
  Manufacturer = 0x93765997
};
