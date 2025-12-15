#!/usr/bin/env python3
"""
SparkFun Variable Loader for Artemis
This script is used to upload firmware to SparkFun Artemis boards via the bootloader.
"""

import argparse
import serial
import sys
import time
import os


class ArtemisBootloader:
    def __init__(self, port, baud):
        self.port = port
        self.baud = baud
        self.ser = None

    def connect(self):
        """Connect to the serial port"""
        try:
            self.ser = serial.Serial(self.port, self.baud, timeout=1)
            time.sleep(0.1)
            print(f"Connected to {self.port} at {self.baud} baud")
            return True
        except Exception as e:
            print(f"Error connecting to {self.port}: {e}")
            return False

    def send_bootload_command(self):
        """Send the bootload command to enter bootloader mode"""
        # DTR/RTS sequence to enter bootloader
        self.ser.setDTR(False)
        self.ser.setRTS(False)
        time.sleep(0.1)
        self.ser.setDTR(True)
        self.ser.setRTS(True)
        time.sleep(0.1)

    def upload_binary(self, binary_path):
        """Upload the binary file to the board"""
        if not os.path.exists(binary_path):
            print(f"Error: Binary file not found: {binary_path}")
            return False

        file_size = os.path.getsize(binary_path)
        print(f"Uploading {binary_path} ({file_size} bytes)...")

        try:
            with open(binary_path, "rb") as f:
                binary_data = f.read()

            # Enter bootloader mode
            self.send_bootload_command()
            time.sleep(1)

            # Clear any existing data
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()

            # Send binary data
            bytes_written = 0
            chunk_size = 512

            for i in range(0, len(binary_data), chunk_size):
                chunk = binary_data[i : i + chunk_size]
                self.ser.write(chunk)
                bytes_written += len(chunk)

                # Progress indicator
                percent = (bytes_written / len(binary_data)) * 100
                print(
                    f"\rProgress: {percent:.1f}% ({bytes_written}/{len(binary_data)} bytes)",
                    end="",
                )
                sys.stdout.flush()

                time.sleep(0.01)  # Small delay between chunks

            print("\nUpload complete!")
            return True

        except Exception as e:
            print(f"\nError during upload: {e}")
            return False

    def close(self):
        """Close the serial connection"""
        if self.ser:
            self.ser.close()
            print("Connection closed")


def main():
    parser = argparse.ArgumentParser(description="SparkFun Artemis Firmware Uploader")
    parser.add_argument(
        "-p", "--port", type=str, help="Serial port (e.g., /dev/tty.usbserial-1234)"
    )
    parser.add_argument(
        "-b", "--baud", type=int, default=921600, help="Baud rate (default: 921600)"
    )
    parser.add_argument(
        "-f", "--file", type=str, required=True, help="Binary file to upload"
    )

    args = parser.parse_args()

    # Auto-detect port if not specified
    if not args.port:
        import serial.tools.list_ports

        ports = serial.tools.list_ports.comports()
        artemis_ports = [
            p.device
            for p in ports
            if "usbserial" in p.device.lower() or "CH340" in str(p)
        ]

        if not artemis_ports:
            print("Error: No serial ports detected. Please specify port with -p flag.")
            sys.exit(1)

        args.port = artemis_ports[0]
        print(f"Auto-detected port: {args.port}")

    # Create bootloader instance and upload
    bootloader = ArtemisBootloader(args.port, args.baud)

    if bootloader.connect():
        success = bootloader.upload_binary(args.file)
        bootloader.close()

        if success:
            print("\n✓ Firmware uploaded successfully!")
            sys.exit(0)
        else:
            print("\n✗ Upload failed!")
            sys.exit(1)
    else:
        print("Failed to connect to board")
        sys.exit(1)


if __name__ == "__main__":
    main()
