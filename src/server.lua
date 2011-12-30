--[[
    tcp socket server portal for cbc server daemon

    Copyright (C) 2012  Pan, Shi Zhu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
--]]--
local socket = require("socket")

local all_connections = {}

local function co_receive(target)
    local result, err
    target:settimeout(0)
    repeat
        result, err = target:receive()
        if err == 'timeout' then
            coroutine.yield(target)
            -- when resume, always remove the target
            for i,v in ipairs(all_connections) do
                if v == target then
                    table.remove(all_connections, i)
                    break
                end
            end
        elseif err then
            print("co_receive", err)
        end
    until result
    return result
end

local function co_accept(target)
    local result, err
    target:settimeout(0)
    repeat
        result, err = target:accept()
        if err == 'timeout' then
            coroutine.yield(target)
            -- when resume, always remove the target
            for i,v in ipairs(all_connections) do
                if v == target then
                    table.remove(all_connections, i)
                    break
                end
            end
        elseif err then
            print("co_accept", err)
        end
    until result
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
    print("server_thread co_accept client=", client)
    -- create a new server to accept another connection
    new_server_thread(server)
    local quit_client = false
    repeat
        local line = co_receive(client)
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

local repr = tools.repr
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
                print("dispatcher", task)
            end
        end
        local nt = #all_threads
        local nc = #all_connections
        if nt ~= 0 and nt == nc then
            socket.select(all_connections)
        end
    until nt == 0
end

local function main()
    print "server started"
    -- create a TCP socket and bind it to the local host, at any port
    local server = assert(socket.bind("*", 5061))
    local ip, port = server:getsockname()
    print("Please connect tcp socket to ".. ip .." on port " .. port)

    new_server_thread(server)
    dispatcher()
    print "server quit"
end

main()
