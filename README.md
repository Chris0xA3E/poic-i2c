# SSD1306 I2C Communication via PIOC on WCH Microcontroller

This project demonstrates how to use **PIOC (Programmable IO Controller)** to implement **I2C communication** with an **SSD1306 OLED display** on a **WCH microcontroller** (e.g., CH32X series).

It provides a minimal, low-level example of bypassing hardware I2C by leveraging PIOC to manually control SDA and SCL lines for communicating with an I2C device.

---

## 📌 Project Goals

- Implement I2C protocol using PIOC (bit-banging through programmable IO)
- Communicate with and display data on an **SSD1306** OLED screen
- Showcase low-level peripheral manipulation on **WCH** devices

---

## 💡 Why Use PIOC Instead of Hardware I2C?

- Useful when hardware I2C is already in use or unavailable
- Demonstrates versatility of WCH's PIOC interface
- Some WCH controllers batches have a damaged I2C subsystem so it is the only way without using soft I2C

---

## 🔧 Hardware Requirements

- WCH microcontroller with PIOC (ex wch32x035)
- SSD1306 OLED display

---
