local prereqs_awaiting = {};
local current_prereq = nil
local awaiting = {};
local current = nil
local current_start = nil
local all_passed = true

function start_next_prereq(state)
  -- get the next prerequisite in the queue
  if (#prereqs_awaiting == 0) then
    -- start test
    local s = current_start
    current_prereq = nil
    current_start = nil
    s()
    return
  end
  current_prereq = prereqs_awaiting[1]
  table.remove(prereqs_awaiting, 1)

  -- jump to the prerequisite and run it
  state.cpu.registers.pc = current_prereq
  state:run();
end

function string:split(sep)
  local sep, fields = sep or ":", {}
  local pattern = string.format("([^%s]+)", sep)
  self:gsub(pattern, function(c) fields[#fields+1] = c end)
  return fields
end

function load_prereqs(state, test)
  -- find all prerequisites and load their addresses
  -- into the prereqs_awaiting table.
  for i, v in ipairs(state:symbols()) do
    if (v.data:sub(1, #"unit:prerequisite:") == "unit:prerequisite:") then
      local list = v.data:sub(#"unit:prerequisite:" + 1):split(":")
      local should = false
      for ii, vv in ipairs(list) do
        if (vv == "all" or vv == test) then
          should = true
          break
        end
      end
      if (should) then
        table.insert(prereqs_awaiting, v.address)
      end
    end
  end 
end

function start_next_test(state)
  -- get the next test in the queue
  if (#awaiting == 0) then
    return
  end
  current = awaiting[1]
  table.remove(awaiting, 1)
 
  -- reset CPU state (we can't reset RAM because
  -- it has the program loaded into it!)
  state.cpu.registers.A = 0x0
  state.cpu.registers.B = 0x0
  state.cpu.registers.C = 0x0
  state.cpu.registers.X = 0x0
  state.cpu.registers.Y = 0x0
  state.cpu.registers.Z = 0x0
  state.cpu.registers.I = 0x0
  state.cpu.registers.J = 0x0
  state.cpu.registers.PC = 0x0
  state.cpu.registers.SP = 0x0
  state.cpu.registers.EX = 0x0
  state.cpu.registers.IA = 0x0
 
  -- search symbols for the start of the test
  for i, v in ipairs(state:symbols()) do
    if (v.data == "unit:definition:" .. current) then
      -- test found
      current = nil
      current_start = function()
        current = v.data:sub(#"unit:definition:" + 1)
        state.cpu.registers.pc = v.address;
        state:run();
      end
      -- load in prerequisites
      load_prereqs(state, v.data:sub(#"unit:definition:" + 1));
      start_next_prereq(state);
      return
    end
  end

  -- test not found
  print("warning: test '" .. current .. "' not found; skipping.")
  current = nil
  start_next_test(state)
end

function run_tests(state, tests)
  -- add the tests onto the queue
  for i, v in ipairs(tests) do
    table.insert(awaiting, v)
  end

  -- now start the first test
  start_next_test(state)
end

function get_all_tests(state)
  local tests = {}
  -- search symbols for unit tests
  for i, v in ipairs(state:symbols()) do
    if (v.data:sub(1, #"unit.definition:") == "unit:definition:") then
      table.insert(tests, v.data:sub(#"unit.definition:" + 1))
    end
  end
  return tests
end

function test_handler(state, params)
  local tests;

  -- check to see whether we have an argument or not
  if (#params == 0) then
    -- run all
    all_passed = true
    tests = get_all_tests(state)
  else
    -- run some
    tests = {}
    for i, v in ipairs(params) do
      table.insert(tests, v.value)
    end
  end

  -- run tests
  run_tests(state, tests)
end

function symbol_handler(state, symbol)
  -- check to see if it's our kind of symbol.
  if (symbol == "unit:terminate" and current ~= nil) then
    -- unit passed
    print("Test '" .. current .. "' passed.")
    current = nil
    state:_break(all_passed)
    start_next_test(state)
  elseif ((symbol:sub(1, #"unit:prerequisite:") == "unit:prerequisite:" or
           symbol:sub(1, #"unit:definition:") == "unit:definition:" or
           symbol == "unit:jump") and
          current_prereq ~= nil and
          current_prereq ~= state.cpu.registers.pc) then
    -- end of prerequisite section
    start_next_prereq(state)
  end
end

function assertion_failure_handler(state, symbol)
  -- make sure we're currently testing
  if (current == nil) then
    return
  end

  -- a failed assertion means that we have
  -- failed the unit test
  print("Test '" .. current .. "' failed! (see above output)")
  all_passed = false
  current = nil
  state:_break(all_passed)
  start_next_test(state)
end

function setup()
  -- perform setup
  add_symbol_hook(symbol_handler)
  add_hook("assertion_failed", assertion_failure_handler)
  add_command("test", test_handler)
end

MODULE = {
  Type = "Debugger",
  Name = "Unit Testing",
  Version = "1.0"
};
