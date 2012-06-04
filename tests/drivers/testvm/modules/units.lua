function prereq_handler(state, params)
  -- all of our arguments should be words
  for i, v in ipairs(params) do
    if (v.type ~= "WORD") then
      error("error: .PREREQ only accepts words as parameters.")
    end
  end
  if (#params == 0) then
    error("error: .PREREQ must have at least one argument.")
  end

  -- create prereqs list
  local prereqs = ""
  for i, v in ipairs(params) do
    prereqs = prereqs .. ":" .. v.value
  end
  prereqs = prereqs:sub(2)

  -- output a symbol for the prerequisite.
  state:add_symbol("unit:prerequisite:" .. prereqs);
end

local current_unit = nil

function unit_handler(state, params)
  -- we should have a single argument that is a word
  if (#params ~= 1 or params[1].type ~= "WORD") then
    error("error: unit name must be a single word")
  end

  -- make sure we're not inside another unit
  if (current_unit ~= nil) then
    error("error: encountered another .UNIT before .ENDUNIT")
  end

  -- define the unit name
  local name = params[1].value

  -- output a symbol that effectively terminates
  -- any prerequisites that have been defined before
  -- here
  state:add_symbol("unit:jump")

  -- output a jump that will skip over the unit
  -- during normal execution.
  state:print_line("SET PC, __unit_test_end_" .. name)

  -- output a symbol for the unit.
  state:add_symbol("unit:definition:" .. name)

  -- set the current unit
  current_unit = name
end

function endunit_handler(state, params)
  -- we should have no arguments
  if (#params ~= 0) then
    error("error: .ENDUNIT accepts no parameters")
  end

  -- make sure we're actually in a unit.
  if (current_unit == nil) then
    error("error: encountered .ENDUNIT before .UNIT")
  end

  -- print the label to skip over the unit test.
  state:print_line(":__unit_test_end_" .. current_unit)

  -- output a termination symbol so that the debugger
  -- knows when the unit ends.
  state:add_symbol("unit:terminate")

  -- reset current_unit variable
  current_unit = nil
end

function setup()
  -- perform setup
  add_preprocessor_directive("PREREQ", prereq_handler)
  add_preprocessor_directive("UNIT", unit_handler)
  add_preprocessor_directive("ENDUNIT", endunit_handler)
end

MODULE = {
  Type = "Preprocessor",
  Name = "Unit Testing",
  Version = "1.0"
};

