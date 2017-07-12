{
  "targets": [
    {
      "target_name": "dock",
      "sources": [
        "src/cpp/window_mac.cc",
        "src/cpp/window_win.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
