#!/usr/bin/env dtlua



function generate(fileName, testName, asmOp, assertOp, handler)
	local file = io.open("math-" .. fileName .. ".i", "w")
	file:write(".UNIT register" .. testName .. "\n")
	for a = 0x0d, 0xFFFF, 0x2aeb do
		for b = 0x07, 0xFFFF, 0x31b3 do
			file:write("SET A, " .. a .. "\n")
			file:write(asmOp .. " A, " .. b .. "\n")
            -- use modulo 0x10000, because lua doesn't have bit operations ( & 0xffff)
			file:write(".ASSERT \"A == " .. (handler(a, b) % 0x10000) .. "\"\n")
		end
	end
    
	file:write(".ENDUNIT\n")
	file:write("\n")
	file:close()
end

-- add and sub (sign doesn't matter)
generate("add", "Add", "ADD", "+", function(a, b) return a + b; end)
generate("sub", "Subtract", "SUB", "-", function(a, b) return a - b; end)

-- unsigned operations
generate("mul", "Multiply", "MUL", "*", function(a, b) return a * b; end)
generate("div", "Divide", "DIV", "/", function(a, b) return math.floor(a / b); end)
generate("mod", "Modulo", "MOD", "%", function(a, b) return a % b; end)

-- signed operations:
generate("mli", "SignedMultiply", "MLI", "S+", function(a, b) if (a >= 0x8000) then a = -(0x10000 - a) end; if (b >= 0x8000) then b = - (0x10000 - b) end; return (a*b); end)
generate("dvi", "SignedDivide", "DVI", "S-", function(a, b) if (a >= 0x8000) then a = -(0x10000 - a) end; if (b >= 0x8000) then b = - (0x10000 - b) end; d = a / b; if (d >= 0) then return math.floor(d); else return math.ceil(d); end; end)
-- signed modulo needs the signed left operand and a unsigned (always postive) right operand
generate("mdi", "SignedModulo", "MDI", "S%", function(a, b) if (a >= 0x8000) then a = - (0x10000 - a) end; return (a % b); end)


