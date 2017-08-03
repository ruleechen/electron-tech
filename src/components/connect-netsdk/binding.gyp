{
  "targets": [{
    "target_name": "netsdk",
    "type": "none",
    "actions": [{
      "action_name": "compile",
      'inputs': [],
      "message": "build netsdk solution",
      "action": ["msbuild", "src/netsdk.sln", "/p:Configuration=Release"]
    }]
  }]
}
