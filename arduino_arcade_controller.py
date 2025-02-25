import serial
import serial.tools.list_ports
import tkinter as tk
from tkinter import ttk
import random

ser = None  # Global serial connection variable

def refresh_com_ports():
    ports = serial.tools.list_ports.comports()
    port_list = [port.device for port in ports]
    port_dropdown['values'] = port_list
    if port_list:
        port_var.set(port_list[0])
    else:
        port_var.set("")

def set_com_port():
    global ser
    port = port_var.get()
    if not port:
        status_label.config(text="No COM port selected!", fg="#EE4932")
        return
    try:
        ser = serial.Serial(port, 9600, timeout=1)
        status_label.config(text=f"Connected to {port}", fg="#3DD37D")
    except Exception as e:
        status_label.config(text=f"Error: {e}", fg="#EE4932")

def send_command(command):
    if ser and ser.is_open:
        ser.write(command.encode())
    else:
        status_label.config(text="Serial port not open!", fg="#EE4932")

def send_random_command():
    command = random.choice(list(commands.values()))
    send_command(command)

def send_custom_command():
    command = custom_command_var.get().strip()
    if command:
        send_command(command)
        custom_command_var.set("")  # Clear input field

def funky_button_hover(event):
    colors = ["#ff5733", "#33ff57", "#3357ff", "#f033ff", "#ffd700"]
    event.widget.config(bg=random.choice(colors))

def funky_button_leave(event):
    event.widget.config(bg="#BDD1FF", fg="black")

# Create main window
app = tk.Tk()
app.title("Funky Arduino Arcade")
app.configure(bg="#304166")

# --- COM Port Selection Frame ---
com_frame = tk.Frame(app, bg="#304166")
com_frame.grid(row=0, column=0, padx=10, pady=10, sticky="w")

tk.Label(com_frame, text="Select COM Port:", fg="#BDD1FF", bg="#304166").grid(row=0, column=0, padx=5, pady=5, sticky="w")

port_var = tk.StringVar()
port_dropdown = ttk.Combobox(com_frame, textvariable=port_var, state="readonly", width=15)
port_dropdown.grid(row=0, column=1, padx=5, pady=5)

refresh_btn = tk.Button(com_frame, text="Refresh", command=refresh_com_ports, bg="#BDD1FF", fg="black", activebackground="#555555")
refresh_btn.grid(row=0, column=2, padx=5, pady=5)

connect_btn = tk.Button(com_frame, text="Connect", command=set_com_port, bg="#BDD1FF", fg="black", activebackground="#555555")
connect_btn.grid(row=0, column=3, padx=5, pady=5)

status_label = tk.Label(app, text="Not connected", fg="#EE4932", bg="#304166", font=("Arial", 12, "bold"))
status_label.grid(row=1, column=0, padx=10, pady=5, sticky="w")

# --- Command Buttons Frame ---
btn_frame = tk.Frame(app, bg="#304166")
btn_frame.grid(row=2, column=0, padx=10, pady=10)

commands = {
    'Red': 'r', 'Green': 'g', 'Yellow': 'y', 'White': 'w',
    'Cycle': 'c', 'Swirl': 's', 'Pulse': 'p', 'Random Twinkle': 't',
    'Color Fade': 'f', 'VJ': 'v', 'Pinball': 'm', 'Blank': 'b'
}

cols = 4
row = 0
col = 0
for text, cmd in commands.items():
    btn = tk.Button(btn_frame, text=text, width=15, command=lambda c=cmd: send_command(c),
                    bg="#BDD1FF", fg="black", font=("Arial", 10, "bold"), relief="raised", bd=3)
    btn.grid(row=row, column=col, padx=5, pady=5)
    btn.bind("<Enter>", funky_button_hover)
    btn.bind("<Leave>", funky_button_leave)
    col += 1
    if col >= cols:
        col = 0
        row += 1

# --- Random Command Button ---
random_btn = tk.Button(app, text="Random Command", command=send_random_command,
                        bg="#FF9800", fg="black", font=("Arial", 10, "bold"), relief="raised", bd=3)
random_btn.grid(row=3, column=0, padx=10, pady=10)
random_btn.bind("<Enter>", funky_button_hover)
random_btn.bind("<Leave>", funky_button_leave)

# --- Custom Command Entry ---
custom_command_frame = tk.Frame(app, bg="#304166")
custom_command_frame.grid(row=4, column=0, padx=10, pady=10)

custom_command_var = tk.StringVar()
custom_command_entry = tk.Entry(custom_command_frame, textvariable=custom_command_var, width=20, font=("Arial", 12))
custom_command_entry.grid(row=0, column=0, padx=5, pady=5)

send_custom_btn = tk.Button(custom_command_frame, text="Send", command=send_custom_command,
                            bg="#4CAF50", fg="black", font=("Arial", 10, "bold"), relief="raised", bd=3)
send_custom_btn.grid(row=0, column=1, padx=5, pady=5)
send_custom_btn.bind("<Enter>", funky_button_hover)
send_custom_btn.bind("<Leave>", funky_button_leave)

refresh_com_ports()
app.mainloop()
