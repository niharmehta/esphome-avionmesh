# MQTT

## Topic Prefix

`<node_name>` — ESPHome device name (e.g. `avionmesh`). Set from `mqtt.get_topic_prefix()` at startup.

## Per-Device / Per-Group Topics

| Topic | Dir | Payload | Retained |
|-------|-----|---------|----------|
| `<prefix>/light/<id>/state` | pub | `ON` / `OFF` | yes |
| `<prefix>/light/<id>/set` | sub | `ON` / `OFF` | — |
| `<prefix>/light/<id>/brightness/state` | pub | `0`–`255` | yes |
| `<prefix>/light/<id>/brightness/set` | sub | `0`–`255` | — |
| `<prefix>/light/<id>/color_temp/state` | pub | mireds | yes |
| `<prefix>/light/<id>/color_temp/set` | sub | mireds | — |

`<id>` = Avi-on device or group ID (uint16, decimal). Broadcast entity uses ID 0.

## Management Channel

| Topic | Dir |
|-------|-----|
| `<prefix>/avionmesh/command` | sub |
| `<prefix>/avionmesh/response` | pub |

Management commands are JSON. See [API Reference](api.md) for payload schemas.

## HA Auto-Discovery

Discovery topic: `homeassistant/light/<node_name>_<id>/config` (retained)

Remove: publish empty retained payload to the discovery topic.

Re-published automatically when HA comes online (`homeassistant/status` → `"online"`).

| Discovery field | Value |
|-----------------|-------|
| `unique_id` | `<node_name>_<id>` |
| `supported_color_modes` | `["color_temp"]` or `["brightness"]` |
| `min_mireds` / `max_mireds` | `200` / `370` |
| `brightness_scale` | `255` |
| `via_device` | `<node_name>` |

## Opt-in Exposure

Devices and groups are not exposed to MQTT by default. When enabled:

1. Discovery payload published (retained)
2. Current state published immediately to state topics — do not wait for next refresh
3. Command topics subscribed

When disabled: empty retained payload published to the discovery topic.

## Color Temperature

The mesh operates in kelvin (2700–6500 K). The component converts at the boundary:

- Inbound (`color_temp/set`): mireds → kelvin: `K = 1,000,000 / mireds`
- Outbound (state publish): kelvin → mireds: `mireds = 1,000,000 / K`

**Silent color temp:** sending only to `color_temp/set` adjusts warmth without changing on/off state or brightness.
