namespace("stormkit", function()
    rule("application", function()
        add_deps("flags")

        on_config(function(target)
            local stormkit_components = target:values("stormkit.components") or {}
            target:set("kind", "binary")
            target:set("languages", "cxxlatest", "clatest")
            target:add("packages", "stormkit", { components = table.join("core", "main", stormkit_components) })
        end)
    end)
end)
