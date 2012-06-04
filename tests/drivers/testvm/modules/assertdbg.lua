function assert_handler(state, symbol)
  function resolve_label(needed)
    if (state.cpu.registers[needed] == nil) then
      state:_break()
      error("unable to resolve '" .. needed .. "' for assertion evaluation (halted vm)")
    else
      return state.cpu.registers[needed]
    end
  end
  -- check to see if it's our kind of symbol.
  if (string.sub(symbol, 0, #"assertion:") == "assertion:") then
    -- handle assertion
    local expr = expression_create(string.sub(symbol, #"assertion:" + 1))
    if (expr:evaluate(resolve_label) ~= 1) then
      -- attempt to find line information
      local lineinfo = ""
      for i, v in ipairs(state:lines()) do
        if (v.address < state.cpu.registers.pc) then 
          lineinfo = " after '" .. v.file .. ":" .. v.line .. "'"
        end
      end
      -- assertion failed, break
      print("assertion \"" .. string.sub(symbol, #"assertion:" + 1) .. "\" failed" .. lineinfo .. ".")
      state:_break()
      state:raise("assertion_failed")
    end
  end
end

function setup()
  -- perform setup
  add_symbol_hook(assert_handler)
end

MODULE = {
  Type = "Debugger",
  Name = "Assertion Module",
  Version = "1.0"
};
