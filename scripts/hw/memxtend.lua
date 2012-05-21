-- memory storage locations
local bank = {};
bank[0] = bank_init();
bank[1] = bank_init();
bank[2] = bank_init();
bank[3] = bank_init();
bank[4] = bank_init();
bank[5] = bank_init();
bank[6] = bank_init();
bank[7] = bank_init();
bank[8] = bank_init();
bank[9] = bank_init();

-- interrupt values
local INT_MEM_SWAP = 0;
local INT_MEM_CLONE = 1;

-- initialize an empty memory bank
function bank_init()
	local result = {};
	for i = 0, 0xFFFF do
		result[i] = 0x0
	end
	return result
end

-- swap the current CPU ram with then
-- memory bank.
function bank_swap(cpu, target)
	local temp = {};
	for i = 0, 0xFFFF do
		temp[i] = cpu.ram[i]
	end
	for i = 0, 0xFFFF do
		cpu.ram[i] = target[i]
	end
	for i = 0, 0xFFFF do
		target[i] = temp[i]
	end
end

-- clone the current CPU ram into the
-- memory bank.
function bank_clone(cpu, target)
	for i = 0, 0xFFFF do
		target[i] = cpu.ram[i]
	end
end

-- handle interrupt
function interrupt(cpu)
	if (cpu.registers.A == INT_MEM_SWAP) then
		if (cpu.registers.B < 0x0) or (cpu.registers.B > 0x9) then
			bank_corrupt(cpu)
		else
			bank_swap(cpu, bank[cpu.registers.B]);
		end
	elseif (cpu.registers.A == INT_MEM_CLONE) then
		if (cpu.registers.B < 0x0) or (cpu.registers.B > 0x9) then
			bank_corrupt(cpu)
		else
			bank_clone(cpu, bank[cpu.registers.B]);
		end
	end
end

MODULE = {
	Type = "Hardware",
	Name = "MEMXTEND - Physical Memory Extensions",
	Version = "0.1"
};

HARDWARE = {
	ID = 0x38f6ab35,
	Version = 0x0001,
	Manufacturer = 0x00000000
};
