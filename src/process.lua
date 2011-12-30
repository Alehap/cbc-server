
local base=_G
module("process")

function cmd(line, client)
    base.print("process_line", line, client)
    client:send(line .. "\n")
    if line == "exit" or line == "quit" then
        return true
    end
end

