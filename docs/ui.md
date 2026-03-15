# Web UI Requirements

Entry point: `http://<device>/ui`

---

## Header (persistent)

- Show BLE state (`Idle / Scanning / Connecting / Discovering / Ready / Disconnected`); highlight red unless Ready
- Show live counts: Devices, Groups, RX packets
- SSE connection indicator (dot: green = connected, grey = disconnected)
- **Save** button — persists DB to NVS; disabled while save is in-flight; shows confirmation for 3 s after success

---

## Devices Section

### Device List

- Render one card per claimed device
- Show empty state when no devices exist
- **Filter bar** — filter visible cards by device type (product name); selecting a type hides non-matching cards; "All" shows every card

### Device Card

Each card shows:

| Element | Detail |
|---------|--------|
| Name | Human-readable label |
| Subtitle | Product type name · device ID |
| State pill | `ON · <brightness>` or `OFF` or blank when unknown; updates in-place without re-rendering the card |
| On / Off buttons | Send brightness 255 / 0 |
| MQTT toggle | Enable/disable HA discovery for this device; optimistically updates UI, reverts on error |
| Group tags | One tag per group the device belongs to |

Expandable **Controls** panel (collapsed by default):

- Brightness slider (0–255); label shows live value; does not send while dragging, sends on release; does not update if slider is active
- Color temp slider (2000–6500 K, step 100); same interaction as brightness; hidden for devices without color temp capability
- **Examine** button — queries device firmware/vendor info; shows spinner while pending; displays result inline for 15 s
- **Remove** button — confirms via dialog before unclaiming

---

## Groups Section

### Group List

- Always includes a synthetic **All (Broadcast)** card (group ID 0) at the top
- Render one card per user-created group
- **Create group** input + button in section header; clears input on success

### Group Card

| Element | Detail |
|---------|--------|
| Name | Group label |
| Subtitle | `Group · #<id>` |
| On / Off buttons | Send brightness 255 / 0 to group address |
| MQTT toggle | Enable/disable HA discovery for this group |
| Member tags | One tag per member device (name or ID); each tag has an × to remove that device from the group |

Expandable **Controls & Members** panel:

- Brightness and color temp sliders (same interaction as device cards; no cached state displayed for groups)
- Add-member dropdown — lists devices not already in the group; **Add** button sends membership command
- Shows "All devices in group" if no eligible devices remain

**Delete** button (not shown on All Broadcast card) — confirms via dialog.

---

## Scan & Add Devices (collapsible)

- **Mesh Ping Scan** — broadcasts a discovery ping; shows spinner on button while in-flight; renders a results table on completion
  - Table columns: ID, FW version, Vendor ID, CSR Product ID, Status (Known / New)
  - New devices show a name input, product type dropdown, and **Add** button
- **Scan for New** — scans for unassociated (unpaired) BLE devices; renders a list of UUID hashes
  - Each entry has a name input, product type dropdown, and **Claim** button
  - Claimed entries are removed from the list on success

---

## Setup (collapsible)

- Passphrase input (password field) + **Set** button — validates: non-empty, valid base64, decodes to ≥ 16 bytes
- **Generate** button — requests a new random passphrase from the device; displays it in a success message
- **Factory Reset** button (danger style) — confirms via dialog; shows success message and reloads after 2 s

---

## Event Feed (collapsible)

- Timestamped log of SSE events and API responses (most recent first)
- Capped at 50 entries; oldest entries removed automatically

---

## Real-time Updates (SSE)

All state changes arrive over a persistent SSE connection (`/api/events`). The UI must:

- Update the state pill and sliders in-place when a `state` event arrives (do not re-render the full card)
- Not update a slider that is currently being dragged
- Re-render device/group lists on `device_added`, `device_removed`, `group_added`, `group_removed`, `group_updated`
- Reflect MQTT toggle changes from `mqtt_toggled` events (handles concurrent sessions)
- Reconnect automatically on SSE disconnect

---

## Product Types

Known product type values used in dropdowns and filter:

| Value | Name |
|-------|------|
| 90 | Lamp Dimmer |
| 93 | Recessed DL |
| 94 | Light Adapter |
| 97 | Smart Dimmer |
| 134 | Smart Bulb |
| 137 | Surface DL |
| 162 | Micro Edge |
| 167 | Smart Switch |
