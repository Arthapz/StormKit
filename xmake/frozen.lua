package("frozen", function()
	set_kind("library")
	set_homepage("https://github.com/serge-sans-paille/frozen")
	set_description("A header-only, constexpr alternative to gperf for C++14 users")
	set_license("Apache-2.0")

	set_urls("https://github.com/Arthapz/frozen.git")

	add_configs("modules", { default = false, type = "boolean" })
	add_configs("std_import", { default = false, type = "boolean" })

	on_install(function(package)
		local configs = {
			modules = package:config("modules"),
			std_import = package:config("std_import"),
		}
		import("package.tools.xmake").install(package, configs)
	end)
end)
