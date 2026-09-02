#!/usr/bin/env python3

# Author    KMS - Martin Dubois, P. Eng.
# Copyright (C) 2026 KMS
# License   http://www.apache.org/licenses/LICENSE-2.0
# Product   KMS-Tools
# File      USB-To/USB-To-I2C.py

# Interactive I2C controller for the WaveShare USB-TO-UART/I2C/SPI/JTAG board (CH347 chip).
#
# Talks directly to the CH347's I2C-stream HID protocol (no vendor SDK needed).
# Requires the `hidapi` package: pip install hidapi
#
# Usage: run the script, enter the I2C device address, then issue commands:
#     READ <n>              read n bytes (1-63) from the device
#     WRITE <b0> <b1> ...    write hex bytes (up to 62) to the device
#     EXIT                   quit

import struct
import sys

import hid

VENDOR_ID = 0x1A86
PRODUCT_ID = 0x55DC
INTERFACE_NUMBER = 1  # SPI/I2C/GPIO HID interface (interface 0 is UART)
READ_TIMEOUT_MS = 200
0
MAX_WRITE_BYTES = 62
MAX_READ_BYTES = 63

CMD_I2C_STREAM = 0xAA
SUB_CMD_SET = 0x60       # | clock_level
SUB_CMD_OUT = 0x74       # | len(data), MSB set: write bytes with start condition
SUB_CMD_IN_MULTI = 0xC0  # | (n - 1): read n bytes, ACK all but the last
SUB_CMD_IN_SINGLE = 0xC0 # alone: read 1 byte, NACK it
SUB_CMD_STOP = 0x75

CLOCK_100K = 1


def find_device() -> hid.device:
    target_path = None
    for info in hid.enumerate(VENDOR_ID, PRODUCT_ID):
        if info["interface_number"] == INTERFACE_NUMBER:
            target_path = info["path"]
            break
    if target_path is None:
        raise RuntimeError(
            "CH347 I2C/SPI interface not found. Is the WaveShare board plugged in "
            "and in the right mode?"
        )
    device = hid.device()
    device.open_path(target_path)
    return device


def i2c_init(device: hid.device, clock_level: int = CLOCK_100K) -> None:
    frame = struct.pack("<BHBB", 0x00, 3, CMD_I2C_STREAM, SUB_CMD_SET | clock_level)
    device.write(frame)


def _i2c_stream(device: hid.device, data: bytes, tail: bytes) -> bytes:
    frame = struct.pack(
        "<BHBBB", 0x00, len(data) + len(tail) + 4, CMD_I2C_STREAM, SUB_CMD_OUT, len(data) | 0x80
    )
    frame += data + tail
    device.write(frame)
    return bytes(device.read(512, timeout_ms=READ_TIMEOUT_MS))


def i2c_write(device: hid.device, addr7: int, data: bytes) -> bool:
    if len(data) > MAX_WRITE_BYTES:
        raise ValueError(f"can write at most {MAX_WRITE_BYTES} bytes per transaction")

    addr_byte = bytes([(addr7 << 1) & 0xFE])
    payload = addr_byte + data
    tail = bytes([SUB_CMD_STOP])
    feedback = _i2c_stream(device, payload, tail)

    ack_stop = len(payload) + 2
    if len(payload) == 1:
        ack_stop -= 1
    acks = feedback[2:ack_stop]

    return len(acks) > 0 and all(acks)


def i2c_read(device: hid.device, addr7: int, length: int) -> bytes:
    if not 1 <= length <= MAX_READ_BYTES:
        raise ValueError(f"length must be between 1 and {MAX_READ_BYTES}")

    addr_byte = bytes([((addr7 << 1) & 0xFE) | 0x01])
    if length == 1:
        tail = bytes([SUB_CMD_IN_SINGLE, SUB_CMD_STOP])
    else:
        tail = bytes([(SUB_CMD_IN_MULTI | (length - 1)) & 0xFF, SUB_CMD_IN_SINGLE, SUB_CMD_STOP])
    feedback = _i2c_stream(device, addr_byte, tail)

    if len(feedback) < 3 or feedback[2] == 0:
        raise IOError(f"device 0x{addr7:02X} did not ACK the read address")

    payload_length = struct.unpack("<H", feedback[:2])[0]
    ack_stop = 3  # 1 address byte -> 1 ack byte, always at feedback[2]
    return feedback[ack_stop:payload_length + 2]


def parse_hex_byte(token: str) -> int:
    value = int(token, 16) if not token.lower().startswith("0x") else int(token, 0)
    if not 0 <= value <= 0xFF:
        raise ValueError(f"byte value out of range: {token}")
    return value


def prompt_address() -> int:
    while True:
        raw = input("Enter I2C device address (7-bit, e.g. 0x50 or 80): ").strip()
        try:
            addr = int(raw, 0)
        except ValueError:
            print("Invalid address, try again.")
            continue
        if not 0 <= addr <= 0x7F:
            print("Address must be a 7-bit value (0-127 / 0x00-0x7F).")
            continue
        return addr


def handle_read(device: hid.device, addr: int, args: list) -> None:
    if len(args) != 1:
        print("Usage: READ <byte_count>")
        return
    try:
        length = int(args[0], 0)
        data = i2c_read(device, addr, length)
    except ValueError as exc:
        print(f"Error: {exc}")
        return
    except IOError as exc:
        print(f"Error: {exc}")
        return
    print(" ".join(f"{b:02X}" for b in data))


def handle_write(device: hid.device, addr: int, args: list) -> None:
    if not args:
        print("Usage: WRITE <hex_byte> [hex_byte ...]")
        return
    try:
        data = bytes(parse_hex_byte(tok) for tok in args)
        ok = i2c_write(device, addr, data)
    except ValueError as exc:
        print(f"Error: {exc}")
        return
    print("OK" if ok else "NACK: device did not acknowledge")


def main() -> None:
    try:
        device = find_device()
    except RuntimeError as exc:
        print(f"Error: {exc}")
        sys.exit(1)

    try:
        i2c_init(device)
        addr = prompt_address()
        print(f"Using I2C address 0x{addr:02X}. Commands: READ <n>, WRITE <b0> [b1 ...], EXIT")

        while True:
            try:
                line = input("> ").strip()
            except EOFError:
                break
            if not line:
                continue

            parts = line.split()
            command, args = parts[0].upper(), parts[1:]

            if command in ("EXIT", "QUIT"):
                break
            elif command == "READ":
                handle_read(device, addr, args)
            elif command == "WRITE":
                handle_write(device, addr, args)
            else:
                print("Unknown command. Use READ <n>, WRITE <b0> [b1 ...], or EXIT.")
    except KeyboardInterrupt:
        print()
    finally:
        device.close()


if __name__ == "__main__":
    main()
