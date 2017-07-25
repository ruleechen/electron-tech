{
  "targets": [
    {
      "target_name": "wb",
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "sources": [],
      "conditions": [
        ["OS == \"win\"", {
          "sources": [
            "src/cpp/window_win.cc"
          ]
        }],
        ["OS == \"mac\"", {
          "sources": [
            "src/cpp/window_mac.cc"
          ]
        }]
      ]
    }
  ]
}
