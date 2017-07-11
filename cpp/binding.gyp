{
  "targets": [
    {
      "target_name": "rcsfb",
      "sources": [
        "src/index.cc",
        "src/get_rect_win.cc",
        "src/get_rect_mac.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
