{
  "targets": [
    {
      "target_name": "dock",
      "sources": [
        "src/index.cc",
        "src/get_rect_win.cc"
      ],
      "libraries": [
        "psapi.lib"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
