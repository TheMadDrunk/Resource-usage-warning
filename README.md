
# Resource Usage Warning Daemon

This simple C program is designed to monitor system resource usage, specifically CPU and memory, and send notifications when certain usage thresholds are exceeded. It operates as a background daemon process, continuously checking the system's resource status and notifying the user when necessary.


## How to Run

To run this program, follow these steps:

1. **Compile the Code:** Use the following command (if you have gcc):
   ```sh
   gcc ruwd.c -o ruwd
   ```

2. **Execute the Program:** Run the compiled program as follows:
   ```sh
   ./ruwd
   ```

   The program will start monitoring system resources in the background and send notifications when thresholds are exceeded.

## Autostart (Optional)

If you want the program to start automatically after a system restart, you can follow [this](https://www.shubhamdipt.com/blog/how-to-create-a-systemd-service-in-linux/) guide on how to create a `systemd` service on Linux to achieve that. In this case, you would not need the `init_start()` function, so it's better to comment it out.

## N.B.
Remember to adjust the resource usage thresholds in the code (`CPU_THRESHOLD` and `MEM_THRESHOLD`) according to your preferences and system specifications.
