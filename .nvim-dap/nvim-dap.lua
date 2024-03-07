local dap = require("dap")
local nmap = require("custom.utils").nmap

nmap("<leader>e", ":sp term://cmake --build build -j30<cr>")
nmap("<leader>E", ":sp term://bash program.sh<cr>")

dap.adapters.cppdbg = {
    id = 'cppdbg',
    type = "executable",
    command = "/home/oskari/dl/cpptools/extension/debugAdapters/bin/OpenDebugAD7"
}

local exec_name = "n64-sniffer.elf"

dap.configurations.cpp = {
    {
        name = "Debug J-Link",
        type = "cppdbg",
        request = "launch",
        MIMode = "gdb",
        miDebuggerServerAddress = "localhost:3333",
        miDebuggerPath = "/usr/bin/arm-none-eabi-gdb",
        cwd = "${workspaceFolder}",
        program = "${workspaceFolder}/build/" .. exec_name,

        stopAtEntry = false,
        postRemoteConnectCommands = {
            {
                text = "load build/" .. exec_name,
            },
            {
                text = "monitor reset halt",
            },
            {
                text = "load",
            },
            {
                text = "break main",
            },
        },
    }
}
