
import serial
import serial.tools.list_ports
import customtkinter as ctk
import random


class SerialApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Funky Arduino Arcade")
        self.root.iconbitmap("icon.ico")
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("blue")
        self.root.geometry("600x600")
        self.root.minsize(480, 480)
        self.root.grid_rowconfigure(2, weight=1)
        self.root.grid_columnconfigure(0, weight=1)

        self.ser = None
        self.idle_mode = False

        self.commands = {
            "Red": "r",
            "Green": "g",
            "Yellow": "y",
            "White": "w",
            "Cycle": "c",
            "Swirl": "s",
            "Pulse": "p",
            "Random Twinkle": "t",
            "Color Fade": "f",
            "VJ": "v",
            "Pinball": "m",
            "Blank": "b"
        }

        self.setup_ui()
        self.refresh_com_ports()
    

    def setup_ui(self):
        self.create_com_frame()
        self.create_status_label()
        self.create_command_buttons()
        self.create_extra_controls()
    

    def create_com_frame(self):
        com_frame = ctk.CTkFrame(self.root, corner_radius=10)
        com_frame.grid(row=0, column=0, padx=16, pady=(16, 8), sticky="ew")
        com_frame.grid_columnconfigure((0,1,2,3), weight=1)

        ctk.CTkLabel(com_frame, text="Select COM Port:").grid(row=0, column=0, padx=5, pady=5, sticky="ew")
        self.port_var = ctk.StringVar()
        self.port_dropdown = ctk.CTkComboBox(com_frame, variable=self.port_var, width=140, state="readonly")
        self.port_dropdown.grid(row=0, column=1, padx=5, pady=5, sticky="ew")
        ctk.CTkButton(com_frame, text="Refresh", command=self.refresh_com_ports, width=80).grid(row=0, column=2, padx=5, pady=5, sticky="ew")
        ctk.CTkButton(com_frame, text="Connect", command=self.set_com_port, width=80).grid(row=0, column=3, padx=5, pady=5, sticky="ew")
    

    def create_status_label(self):
        self.status_label = ctk.CTkLabel(self.root, text="Not connected", text_color="#EE4932", font=("Arial", 14, "bold"))
        self.status_label.grid(row=1, column=0, padx=16, pady=(0, 8), sticky="ew")
    

    def create_command_buttons(self):
        btn_frame = ctk.CTkFrame(self.root, corner_radius=10)
        btn_frame.grid(row=2, column=0, padx=16, pady=8, sticky="nsew")
        btn_frame.grid_columnconfigure((0,1,2,3), weight=1)
        btn_frame.grid_rowconfigure(tuple(range((len(self.commands)+3)//4)), weight=1)
        cols = 4
        row, col = 0, 0
        self.cmd_buttons = []
        for text, cmd in self.commands.items():
            btn = ctk.CTkButton(btn_frame, text=text, width=120, height=36, command=lambda c=cmd: self.send_command(c), font=("Arial", 13, "bold"))
            btn.grid(row=row, column=col, padx=6, pady=6, sticky="nsew")
            self.cmd_buttons.append(btn)
            col += 1
            if col >= cols:
                col = 0
                row += 1
    

    def create_extra_controls(self):
        controls_frame = ctk.CTkFrame(self.root, corner_radius=10)
        controls_frame.grid(row=3, column=0, padx=16, pady=(8, 0), sticky="ew")
        controls_frame.grid_columnconfigure((0,1,2), weight=1)

        ctk.CTkButton(controls_frame, text="Random Command", width=160, command=self.send_random_command, fg_color="#FF9800").grid(row=0, column=0, padx=8, pady=8, sticky="ew")
        self.idle_button = ctk.CTkButton(controls_frame, text="Enable Idle Mode", width=160, command=self.toggle_idle_mode, fg_color="#BDD1FF", text_color="#000000")
        self.idle_button.grid(row=0, column=1, padx=8, pady=8, sticky="ew")
        ctk.CTkButton(controls_frame, text="Send Test Message", width=160, command=self.send_test_message, fg_color="#FF5733").grid(row=0, column=2, padx=8, pady=8, sticky="ew")

        custom_frame = ctk.CTkFrame(self.root, corner_radius=10)
        custom_frame.grid(row=4, column=0, padx=16, pady=(8, 16), sticky="ew")
        custom_frame.grid_columnconfigure((0,1), weight=1)
        self.custom_command_var = ctk.StringVar()
        ctk.CTkEntry(custom_frame, textvariable=self.custom_command_var, width=180).grid(row=0, column=0, padx=8, pady=8, sticky="ew")
        ctk.CTkButton(custom_frame, text="Send", width=80, command=self.send_custom_command, fg_color="#4CAF50").grid(row=0, column=1, padx=8, pady=8, sticky="ew")

    

    def refresh_com_ports(self):
        ports = serial.tools.list_ports.comports()
        port_list = [port.device for port in ports]
        self.port_dropdown.configure(values=port_list)
        if port_list:
            self.port_var.set(port_list[0])
        else:
            self.port_var.set("")
    

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
        self.update_status("Test message feature removed.", "#EE4932")
    

    def toggle_idle_mode(self):
        self.idle_mode = not self.idle_mode
        self.send_command('i' if self.idle_mode else 'n')
        self.idle_button.configure(text="Disable Idle Mode" if self.idle_mode else "Enable Idle Mode", fg_color="#FF5733" if self.idle_mode else "#BDD1FF", text_color="#000000")
    

    def update_status(self, message, color):
        self.status_label.configure(text=message, text_color=color)
    

# No hover effect in customtkinter, but could be added with advanced event binding if needed

if __name__ == "__main__":
    root = ctk.CTk()
    app = SerialApp(root)
    root.mainloop()
