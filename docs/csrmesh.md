# CSRMesh / BLE

## Service & Characteristics

BLE service UUID: `0xFEF1`

| Characteristic | UUID (128-bit, LE) | Direction |
|----------------|--------------------|-----------|
| Low | `c4edc000-9daf-11e3-8003-000200005b0b` | write (no response) — outbound PDUs |
| High | `c4edc000-9daf-11e3-8004-000200005b0b` | notify — inbound PDUs |

## BLE State Machine

```
Idle
 └─► Scanning (5 s window — collect all bridge advertisements by RSSI)
      └─► Connecting (connect to best-RSSI bridge)
           └─► Discovering (GATT service + characteristic discovery)
                └─► Ready  ◄─── all normal mesh operation
                     └─► Disconnected ──► wait 3 s ──► Idle
```

- Single bridge connection at a time; all mesh traffic routes through it
- On disconnect: reconnect after 3 s

## Association (Claiming)

Pairing an unassociated device triggers a CSRMesh association sequence. Times out after 30 s.

## Mesh Operations

| Operation | Trigger |
|-----------|---------|
| Mesh ping scan | Web UI / MQTT `discover_mesh` action |
| Unassociated scan | GAP scan for non-mesh BLE advertisers |
| Set brightness | Control command; brightness 0 = off |
| Set color temp | Control command (kelvin) |
| Read all dimming | On connect + periodic (60 s) |
| Read all color | On connect + periodic (60 s) |
| Examine device | Web UI per-device; returns fw/vendor/product/flags |
| Group membership | Add/remove device to/from group over mesh |
| Time sync | Daily interval |

## Rapid Dimming

Brightness commands arriving within 750 ms of the previous command for the same device skip the mesh send and only update cached state + publish MQTT. Prevents flooding the mesh during slider drags.

## External Dependencies

| Library | Role |
|---------|------|
| [recsrmesh-cpp](https://github.com/oyvindkinsey/recsrmesh-cpp) | CSRMesh BLE transport |
| [avionmesh-cpp](https://github.com/oyvindkinsey/avionmesh-cpp) | Avi-on command encoding / decoding |
