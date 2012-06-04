function assert_handler(state, params)
  -- we expect a single parameter that is an expression.
  if (#params ~= 1 or (params[1].type ~= "STRING" and params[1].type ~= "EXPRESSION")) then
    error("error: .ASSERT directive expects single expression parameter.")
  end
  local expr = nil;
  if (params[1].type == "STRING") then
    expr = expression_create(params[1].value);
  else
    expr = params[1].value
  end

  -- output a symbol for the expression.
  state:add_symbol("assertion:" .. expr:representation());
end

function setup()
  -- perform setup
  add_preprocessor_directive("ASSERT", assert_handler, false, true)
end

MODULE = {
  Type = "Preprocessor",
  Name = ".ASSERT directive",
  Version = "1.0"
};

