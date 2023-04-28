-- This is a Lua global variable
some_variable = 3;

if some_variable == 42 then
	print("The value of the variable is "..some_variable)
end

-- This is a lua table
config = {
	title = "My Game Engine",
	fullscreen = false,
	resolution = {
		width = 1280,
		height = 720
	}
}

-- This is a lua function
function factorial(n)
	local result = n
	while n > 0 do
		result = result * (n - 1)
		n = n - 1

	print(result)
end