import serial
import serial.tools.list_ports
import tkinter as tk
from tkinter import ttk
import random

class SerialApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Funky Arduino Arcade")
        self.root.iconbitmap("icon.ico")
        self.root.configure(bg="#304166")
        
        self.ser = None
        self.second_serial_device = None  # Added second serial device
        self.idle_mode = False
        self.commands = {
            'Red': 'r', 'Green': 'g', 'Yellow': 'y', 'White': 'w',
            'Cycle': 'c', 'Swirl': 's', 'Pulse': 'p', 'Random Twinkle': 't',
            'Color Fade': 'f', 'VJ': 'v', 'Pinball': 'm', 'Blank': 'b'
        }
        
        self.setup_ui()
        self.refresh_com_ports()
    
    def setup_ui(self):
        self.create_com_frame()
        self.create_status_label()
        self.create_command_buttons()
        self.create_extra_controls()
    
    def create_com_frame(self):
        com_frame = tk.Frame(self.root, bg="#304166")
        com_frame.grid(row=0, column=0, padx=10, pady=10, sticky="w")

        tk.Label(com_frame, text="Select COM Port:", fg="#BDD1FF", bg="#304166").grid(row=0, column=0, padx=5, pady=5, sticky="w")
        
        self.port_var = tk.StringVar()
        self.port_dropdown = ttk.Combobox(com_frame, textvariable=self.port_var, state="readonly", width=15)
        self.port_dropdown.grid(row=0, column=1, padx=5, pady=5)
        
        tk.Button(com_frame, text="Refresh", command=self.refresh_com_ports, bg="#BDD1FF").grid(row=0, column=2, padx=5, pady=5)
        tk.Button(com_frame, text="Connect", command=self.set_com_port, bg="#BDD1FF").grid(row=0, column=3, padx=5, pady=5)
        
        tk.Label(com_frame, text="Select Second:", fg="#BDD1FF", bg="#304166").grid(row=1, column=0, padx=5, pady=5, sticky="w")
        
        self.second_port_var = tk.StringVar()
        self.second_port_dropdown = ttk.Combobox(com_frame, textvariable=self.second_port_var, state="readonly", width=15)
        self.second_port_dropdown.grid(row=1, column=1, padx=5, pady=5)
        
        tk.Button(com_frame, text="Connect", command=self.set_second_com_port, bg="#BDD1FF").grid(row=1, column=3, padx=5, pady=5)
    
    def create_status_label(self):
        self.status_label = tk.Label(self.root, text="Not connected", fg="#EE4932", bg="#304166", font=("Arial", 12, "bold"))
        self.status_label.grid(row=1, column=0, padx=10, pady=5, sticky="w")
    
    def create_command_buttons(self):
        btn_frame = tk.Frame(self.root, bg="#304166")
        btn_frame.grid(row=2, column=0, padx=10, pady=10)
        
        cols = 4
        row, col = 0, 0
        for text, cmd in self.commands.items():
            btn = tk.Button(btn_frame, text=text, width=17, command=lambda c=cmd: self.send_command(c),
                            bg="#BDD1FF", font=("Arial", 14, "bold"))
            btn.grid(row=row, column=col, padx=5, pady=5)
            btn.bind("<Enter>", self.funky_button_hover)
            btn.bind("<Leave>", self.funky_button_leave)
            
            col += 1
            if col >= cols:
                col = 0
                row += 1
    
    def create_extra_controls(self):
        tk.Button(self.root, text="Random Command", width=17, 
                  command=self.send_random_command, bg="#FF9800").grid(
                      row=3, column=0, padx=10, pady=10)

        self.idle_button = tk.Button(self.root, text="Enable Idle Mode", width=17,
                                     command=self.toggle_idle_mode, bg="#BDD1FF")
        self.idle_button.grid(row=4, column=0, padx=10, pady=10)

        custom_frame = tk.Frame(self.root, bg="#304166")
        custom_frame.grid(row=5, column=0, padx=10, pady=10)

        self.custom_command_var = tk.StringVar()
        tk.Entry(custom_frame, textvariable=self.custom_command_var, width=20).grid(
            row=0, column=0, padx=5, pady=5)

        tk.Button(custom_frame, text="Send", width=17,
                  command=self.send_custom_command, bg="#4CAF50").grid(
                      row=0, column=1, padx=5, pady=5)

        tk.Button(self.root, text="Send Test Message", width=17, 
                  command=self.send_test_message, bg="#FF5733").grid(
                      row=6, column=0, padx=10, pady=10)

    
    def refresh_com_ports(self):
        ports = serial.tools.list_ports.comports()
        port_list = [port.device for port in ports]
        self.port_dropdown['values'] = port_list
        self.second_port_dropdown['values'] = port_list
        self.port_var.set(port_list[0] if port_list else "")
        self.second_port_var.set(port_list[0] if port_list else "")
    
    def set_com_port(self):
        port = self.port_var.get()
        if not port:
            self.update_status("No COM port selected!", "#EE4932")
            return
        try:
            self.ser = serial.Serial(port, 9600, timeout=1)
            self.update_status(f"Connected to {port}", "#3DD37D")
        except Exception as e:
            self.update_status(f"Error: {e}", "#EE4932")
                
    def set_second_com_port(self):
        port = self.second_port_var.get()
        if not port:
            self.update_status("No second COM port selected!", "#EE4932")
            return
        try:
            self.second_serial_device = serial.Serial(port, 9600, timeout=1)
            self.update_status(f"Second device connected to {port}", "#3DD37D")
        except Exception as e:
            self.update_status(f"Error: {e}", "#EE4932")
    
    def send_command(self, command):
        if self.ser and self.ser.is_open:
            self.ser.write(command.encode())
        else:
            self.update_status("Serial port not open!", "#EE4932")
    
    def send_random_command(self):
        self.send_command(random.choice(list(self.commands.values())))
    
    def send_custom_command(self):
        command = self.custom_command_var.get().strip()
        if command:
            self.send_command(command)
            self.custom_command_var.set("")
    
    def send_test_message(self):
        if self.second_serial_device and self.second_serial_device.is_open:
            self.second_serial_device.write(b'd')
        else:
            self.update_status("Second serial device not connected!", "#EE4932")
    
    def toggle_idle_mode(self):
        self.idle_mode = not self.idle_mode
        self.send_command('i' if self.idle_mode else 'n')
        self.idle_button.config(text="Disable Idle Mode" if self.idle_mode else "Enable Idle Mode", bg="#FF5733" if self.idle_mode else "#BDD1FF")
    
    def update_status(self, message, color):
        self.status_label.config(text=message, fg=color)
    
    def funky_button_hover(self, event):
        event.widget.config(bg=random.choice(["#ff5733", "#33ff57", "#3357ff", "#f033ff", "#ffd700"]))
    
    def funky_button_leave(self, event):
        event.widget.config(bg="#BDD1FF")

if __name__ == "__main__":
    root = tk.Tk()
    app = SerialApp(root)
    root.mainloop()
