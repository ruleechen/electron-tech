{
  "targets": [
    {
      "target_name": "dock",
      "sources": [
        "cpp/window_mac.cc",
        "cpp/window_win.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
