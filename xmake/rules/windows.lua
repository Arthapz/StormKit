rule("platform.windows.subsystem.windows", function()
    add_deps("platform.windows.subsystem")
    on_load(function(target) target:values_set("windows.subsystem", "windows") end)
end)

rule("platform.windows.subsystem.console", function()
    add_deps("platform.windows.subsystem")
    on_load(function(target) target:values_set("windows.subsystem", "console") end)
end)
