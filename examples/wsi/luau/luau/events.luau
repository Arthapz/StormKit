local function main()
  local window = wsi.open_window("test", 800, 600, wsi.WindowFlag.RESIZEABLE)


  window:on_closed(function()
    print("Close event!")
    return true
  end)

  window:event_loop(function()
    print("AAAAAAAAAA")
    window:clear()
    print("BBBBBBBBB")
  end)
end

main()
