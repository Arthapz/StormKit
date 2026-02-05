namespace("stormkit", function()
    rule("library", function()
        add_deps("flags")

        on_config(function(target)
            local stormkit_components = target:values("stormkit.components") or {}
            if not target:kind() then target:set("kind", "$(kind)") end
            target:set("languages", "cxxlatest", "clatest")
            target:add("packages", "stormkit", { components = table.join("core", "main", stormkit_components) })
        end)
    end)
end)
