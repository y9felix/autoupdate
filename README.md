# autoupdate

Daily idle system autoupdate and reboot daemon for Arch Linux.

`autoupdate` monitors system state (AC power, lid switch, idle timer, and cooldown timestamps). When conditions are satisfied, it executes pacman system updates and reboots cleanly.

## Requirements

- Arch Linux (`pacman`, `systemd`)
- C99 compiler (`cc`)
- GNU Make

## Installation

```sh
make
sudo make install
```

## Configuration

Settings and thresholds are configured at compile-time in `config.h`:

```sh
cp config.def.h config.h
# edit config.h
make
```

## License

MIT License.
