# Embedded Key-Value Database

A lightweight, zero-dependency, in-memory Key-Value store written in pure C. This database is optimized for embedded devices, microcontrollers, and robotics applications where memory efficiency and speed are critical.

## Features

* **Pure C Implementation:** No external dependencies, minimal memory footprint.
* **In-Memory Speed:** Utilizes a custom Hash Table with the DJB2 hashing algorithm for $O(1)$ lookups.
* **Persistent Storage:** Saves and loads data automatically to/from a custom binary format (`.tlv`) on disk.
* **Full CRUD Support:** Interactive REPL terminal interface supporting `SET`, `GET`, `DEL`, `LIST`, and `HELP` commands.
* **Robotics Ready:** Compiled executable is only a few kilobytes, making it perfect for storing robot calibrations, sensor states, or system logs.

## Command Interface

```text
comands [QUIT] [SET] [GET] [DEL] [LIST] [HELP]
> SET max_speed 150
OK: Saved max_speed -> 150

> GET max_speed
-> 150

> DEL max_speed
OK: Key 'max_speed' deleted.