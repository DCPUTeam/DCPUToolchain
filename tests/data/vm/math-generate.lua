#!/usr/bin/env dtlua

function generate(fileName, testName, asmOp, assertOp, handler)
	local file = io.open("math-" .. fileName .. ".i", "w")
	file:write(".UNIT register" .. testName .. "\n")
	for a = 0x100, 0x2000, 0x234 do
		for b = 0x200, 0x4000, 0x765 do
			file:write("SET A, " .. a .. "\n")
			file:write(asmOp .. " A, " .. b .. "\n")
			file:write(".ASSERT \"A == " .. (handler(a, b) & 0xFFFF) .. "\"\n")
		end
	end
	file:write(".ENDUNIT\n")
	file:write("\n")
	file:close()
end

generate("add", "Add", "ADD", "+", function(a, b) return a + b; end)
generate("sub", "Subtract", "SUB", "-", function(a, b) return a - b; end)
generate("mul", "Multiply", "MUL", "*", function(a, b) return a * b; end)
generate("div", "Divide", "DIV", "/", function(a, b) return math.floor(a / b); end)
generate("mli", "SignedMultiply", "MLI", "S+", function(a, b) return (a - 0x8000) * (b - 0x8000); end)
generate("dvi", "SignedDivide", "DVI", "S-", function(a, b) return math.floor((a - 0x8000) / (b - 0x8000)); end)
generate("mod", "Modulo", "MOD", "%", function(a, b) return a % b; end)
generate("mdi", "SignedModulo", "MDI", "S%", function(a, b) return (a - 0x8000) % (b - 0x8000); end)
