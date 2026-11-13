## Change Scoreboard Mapname

![Change Scoreboard Mapname](preview.png)

*Adds a suffix to the card name in scoreboard.*

Better than other versions because:
- no additional dependencies
- works at the netmessages level
- server information displays correct map

**Config** - ``addons/configs/change_scoreboard_mapname.ini``

### Version 1.3.0

Maintained fork of [qstage's plugin](https://github.com/qstage/cs2_change_scoreboard_mapname).
Built for Metamod:Source 2.0.0-dev+1490 with matching KHook headers and the current CS2 SDK.
Older Metamod/KHook builds are not supported by these binaries.

Download `linux.zip` or `windows.zip` from [Releases](https://github.com/Magic8Ballin/cs2_change_scoreboard_mapname/releases).
Stop the server, back up your existing configuration, and extract the archive into `game/csgo`.
Restore your configuration if upgrading, then restart. Confirm version `1.3.0` with `meta list`.

```text
"Change Scoreboard Mapname"
{
    "map_ending" " | My Server"
    "use_hostname" "0"
}
```

Set `use_hostname` to `1` to append ` | ` followed by the server hostname instead.
Configuration changes require a plugin reload or server restart. Reconnect to see changes.

The build workflow pins Metamod, the SDK, SDK manifests, and AMBuild revisions and produces
both archives with the binary, Metamod loader, and default configuration. Version 1.3.0 also
restores the original network message after serialization, preventing repeated suffixes when
the engine reuses a message for multiple clients, and installs its hook only after configuration loads.
