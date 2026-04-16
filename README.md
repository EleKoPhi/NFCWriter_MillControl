# NFCWriter_MillControl

Nfc_MillControl_V2 is the updated version of Nfc_MillControl_V1.

## Functional description

The project upgrades an analog coffee grinder with NFC-based user handling and a small OLED UI.

## User documentation

End-user operating guide:

- [docs/ANWENDERDOKUMENTATION.md](docs/ANWENDERDOKUMENTATION.md)

## Project configuration

The project is configured for PlatformIO and ESP32 (`nodemcu-32s`):

- Default environment: `nodemcu-32s_128x64_debug`
- Additional environment: `nodemcu-32s_128x64_release`
- Display mode: `-D SIXTYFOUR` (128x64 OLED)

Dependencies are managed by PlatformIO in `platformio.ini`.

## Setup in VS Code

1. Open the folder `NFCWriter_MillControl` in VS Code.
2. Install recommended extensions when prompted.
3. Build from terminal:

```powershell
platformio run
```

If `platformio` is not available in PATH, install it with one of these options:

- PlatformIO IDE extension in VS Code (recommended)
- CLI install:

```powershell
pip install -U platformio
```

Then restart terminal and run:

```powershell
platformio --version
```

## Useful commands

Build debug (default):

```powershell
platformio run -e nodemcu-32s_128x64_debug
```

Build release:

```powershell
platformio run -e nodemcu-32s_128x64_release
```

Upload firmware:

```powershell
platformio run -t upload
```

Open serial monitor:

```powershell
platformio device monitor -b 9600
```
