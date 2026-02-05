local function main()
  local window = wsi.open_window("test", 800, 600, wsi.window_flag.RESIZEABLE)

  window:on_closed(function()
    print("Close event!")
    return true
  end)

  local vec = vector.create(0, 1, 0)

  window:on_resized(function(extent)
    print("Resize event: %s", extent)
    return true
  end)

  window:on_monitor_changed(function(monitor)
    print("Monitor changed event: %s", monitor)
    return true
  end)

  window:on_mouse_moved(function(_, position)
    print("Mouse move event: %s", position)
    return true
  end)

  window:on_mouse_button_down(function(_, button, position)
    print("Mouse button down event: %s %s", button, position)
    return true
  end)

  window:on_mouse_button_up(function(_, button, position)
    print("Mouse button up event: %s %s", button, position)
    return true
  end)

  window:on_restored(function()
    print("Restored event!")
    return true
  end)

  window:on_minimized(function()
    print("Minimzed event!")
    return true
  end)

  window:on_activate(function()
    print("Activate event!")
    return true
  end)

  window:on_deactivate(function()
    print("Deactivate event!")
    return true
  end)

  window:on_key_down(function(_, key, c)
    local closures = {
      [wsi.Key.ESCAPE] = function()
        window:close()
        print("Closing window")
      end,
      [wsi.Key.W] = function()
        window:close()
        print("Closing window")
      end,
      [wsi.Key.T] = function()
        window:close()
        print("Closing window")
      end,
      [wsi.Key.H] = function()
        window:close()
        print("Closing window")
      end,
      [wsi.Key.F11] = function()
        window:close()
        print("Closing window")
      end,
      [wsi.Key.F1] = function()
        window:close()
        print("Closing window")
      end,
      [wsi.Key.F2] = function()
        window:close()
        print("Closing window")
      end,
      [wsi.Key.F3] = function()
        window:close()
        print("Closing window")
      end,
      [wsi.Key.F4] = function()
        window:close()
        print("Closing window")
      end,
      [wsi.Key.F5] = function()
        window:close()
        print("Closing window")
      end,
    }

    for k, closure in pairs(closures) do
      if k == key then
        closure()
        break
      end
    end

    print("Key down --\n    code: %s\n    value: '%s'\n)", key, c);
  end)

  window:on_key_up(function(_, key, c)
    print("Key up --\n    code: %s\n    value: '%s'\n)", key, c);
  end)

  window:event_loop(function()
    window:clear()
  end)
end

main()
