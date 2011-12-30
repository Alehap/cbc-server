local socket = require("socket")

local all_connections = {}

local function co_receive(target)
    print("co_receive", target)
    local result, err
    target:settimeout(0)
    repeat
        result, err = target:receive()
        print("co_receive", result, err)
        if err == 'timeout' then
            coroutine.yield(target)
        elseif err then
            print(err)
        end
    until result
    for i,v in ipairs(all_connections) do
        if v == target then
            table.remove(all_connections, i)
            break
        end
    end
    return result
end

local function co_accept(target)
    local result, err
    target:settimeout(0)
    repeat
        result, err = target:accept()
        print("co_accept", result, err)
        if err == 'timeout' then
            coroutine.yield(target)
        elseif err then
            print(err)
        end
    until result
    for i,v in ipairs(all_connections) do
        if v == target then
            table.remove(all_connections, i)
            break
        end
    end
    return result
end

local function process_line(line, client)
    print("process_line", line, client)
    client:send(line .. "\n")
    if line == "exit" or line == "quit" then
        return true
    end
end

local function server_thread(server)
    local client = co_accept(server)
    print("server_thread1", client)
    -- create a new server to accept another connection
    new_server_thread(server)
    local quit_client = false
    repeat
        local line = co_receive(client)
        print("server_thread1", line)
        quit_client = process_line(line, client)
    until quit_client
    client:close()
end

local all_threads = {}
function new_server_thread(server)
    local thread = coroutine.create(function()
        server_thread(server)
    end)
    table.insert(all_threads, thread)
end

local function dispatcher()
    repeat
        for i,v in ipairs(all_threads) do
            local status, task = coroutine.resume(v)
            if status then
                -- coroutine yields or returns
                if not task then    -- thread returns
                    table.remove(all_threads, i)
                    -- the index of table changed, we must break the loop
                    break
                else    -- thread yields
                    table.insert(all_connections, task)
                end
            else
                -- error in coroutine
                print(task)
            end
        end
        local n = #all_threads
        if n ~= 0 and #all_connections == n then
            socket.select(all_connections)
        end
    until n == 0
end

local function main()
    print "server started"
    -- create a TCP socket and bind it to the local host, at any port
    local server = assert(socket.bind("*", 10007))
    local ip, port = server:getsockname()
    print("Please telnet to ".. ip .." on port " .. port)

    new_server_thread(server)
    dispatcher()
    print "server quit"
end

main()
