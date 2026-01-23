print(wsi.EventType)

local window = wsi.open_window("test", 800, 600, wsi.WindowFlag.RESIZEABLE)

print("wm: " .. window:wm())

-- window:on(wsi.EventType.CLOSED, function()
--   print("Close event!")
-- end)

-- window:event_loop(function()
--   window.clear()
-- end)
