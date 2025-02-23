import serial
import serial.tools.list_ports
import tkinter as tk
from tkinter import ttk

ser = None  # Global serial connection variable

def refresh_com_ports():
    """Detect available COM ports and populate the dropdown."""
    ports = serial.tools.list_ports.comports()
    port_list = [port.device for port in ports]
    port_dropdown['values'] = port_list
    if port_list:
        port_var.set(port_list[0])
    else:
        port_var.set("")

def set_com_port():
    """Set and connect to the selected COM port."""
    global ser
    port = port_var.get()
    if not port:
        status_label.config(text="No COM port selected!")
        return
    try:
        ser = serial.Serial(port, 9600, timeout=1)
        status_label.config(text=f"Connected to {port}", fg="green")
    except Exception as e:
        status_label.config(text=f"Error: {e}", fg="red")

def send_command(command):
    """Send a command character to the Arduino."""
    if ser and ser.is_open:
        ser.write(command.encode())
    else:
        status_label.config(text="Serial port not open!", fg="red")

# Create the main window
app = tk.Tk()
app.title("Arduino Arcade Controller")

# --- COM Port Selection Frame ---
com_frame = tk.Frame(app)
com_frame.grid(row=0, column=0, padx=10, pady=10, sticky="w")

tk.Label(com_frame, text="Select COM Port:").grid(row=0, column=0, padx=5, pady=5, sticky="w")

port_var = tk.StringVar()
port_dropdown = ttk.Combobox(com_frame, textvariable=port_var, state="readonly", width=15)
port_dropdown.grid(row=0, column=1, padx=5, pady=5)

tk.Button(com_frame, text="Refresh", command=refresh_com_ports).grid(row=0, column=2, padx=5, pady=5)
tk.Button(com_frame, text="Connect", command=set_com_port).grid(row=0, column=3, padx=5, pady=5)

status_label = tk.Label(app, text="Not connected", fg="red")
status_label.grid(row=1, column=0, padx=10, pady=5, sticky="w")

# --- Command Buttons Frame ---
btn_frame = tk.Frame(app)
btn_frame.grid(row=2, column=0, padx=10, pady=10)

# Define commands and their corresponding Arduino characters
commands = {
    'Red': 'r',
    'Green': 'g',
    'Yellow': 'y',
    'White': 'w',
    'Cycle': 'c',
    'Swirl': 's',
    'Pulse': 'p',
    'Random Twinkle': 't',
    'Color Fade': 'f',
    'VJ': 'v',
    'Pinball': 'm',
    'Blank': 'b'
}

# Arrange buttons in a grid with 4 columns per row
cols = 4
row = 0
col = 0
for text, cmd in commands.items():
    btn = tk.Button(btn_frame, text=text, width=15, command=lambda c=cmd: send_command(c))
    btn.grid(row=row, column=col, padx=5, pady=5)
    col += 1
    if col >= cols:
        col = 0
        row += 1

# Populate the COM port dropdown initially
refresh_com_ports()

app.mainloop()
