# Si5351 Clock Generator for Arduino Projects

This project showcases how to create a clock generator using the **Si5351 module** with an Arduino Nano. It includes an OLED display for visual feedback and supports frequency adjustments through an encoder. The project is designed for electronics enthusiasts and engineers looking to generate precise clock signals for various applications.

## Features
- **Si5351 Module**: Generates stable clock signals.
- **OLED Display (128x64)**: Displays frequency and other parameters.
- **Rotary Encoder**: Adjusts frequency with tactile feedback.
- **Compact Design**: Fits seamlessly with Arduino Nano.
- **Custom PCB Layout**: Designed for ease of assembly.

---

## Circuit Schematic

![Circuit Schematic](https://raw.githubusercontent.com/TeknoTrek/Si5351-Clock-Generator-for-Arduino-Projects/refs/heads/main/Si5351-Signal-Generator/images/circuit-sc.jpg)

The schematic includes:
- **Power Input**: DC 5V via terminal connector (J2).
- **Arduino Nano**: Acts as the main controller.
- **OLED Display**: Connected via I2C (SDA, SCL).
- **Si5351 Module**: Outputs clock signals.
- **Rotary Encoder**: For user interaction.
- **Signal Output**: Filtered via capacitor and resistor.

---

## PCB Layout

![PCB Layout](https://raw.githubusercontent.com/TeknoTrek/Si5351-Clock-Generator-for-Arduino-Projects/refs/heads/main/Si5351-Signal-Generator/images/f_cu_screen.jpg)

![PCB Layout](https://raw.githubusercontent.com/TeknoTrek/Si5351-Clock-Generator-for-Arduino-Projects/refs/heads/main/Si5351-Signal-Generator/images/pcb.jpg)

The PCB is designed to accommodate:
- **Si5351 Module**
- **OLED Display**
- **Arduino Nano**
- **Rotary Encoder**
- **Signal Output Terminal**
- **Decoupling Capacitors for Noise Reduction**

### Key Design Highlights:
- Labels for each component for easy assembly.
- Signal integrity ensured with decoupling capacitors.
- Clear silkscreen layout for user-friendly assembly.

---

## Components Needed
- **Si5351 Module**
- **Arduino Nano (v3.x)**
- **OLED Display (128x64)**
- **Rotary Encoder with Button**
- **Decoupling Capacitors**: 10µF, 100nF
- **Resistor**: 1kΩ
- **PCB**: Custom-designed layout provided

---

## Software
### Arduino Code
The Arduino sketch for this project initializes the Si5351 module and OLED display, processes encoder inputs, and outputs clock signals.

> Code will be provided in the GitHub repository.

---

## Assembly Instructions
1. Solder all the components onto the custom PCB as per the silkscreen.
2. Connect the DC power supply (5V) to the terminal connector.
3. Upload the provided Arduino sketch to the Nano.
4. Power on the setup and adjust the frequency using the encoder.
5. Observe the output frequency on the OLED display.

---

## Applications
- RF signal generation
- Frequency synthesis
- Test equipment

---

## Demo
Check out the video demonstration of the project on our YouTube channel:

📺 **[Teknotrek YouTube Channel](https://youtu.be/GfB7mTY67xE)**

---

## License
This project is licensed under the **MIT License**. Feel free to use, modify, and share it!

---

## Contribution
Feel free to open issues or submit pull requests to improve the project.

---

## Contact
For any inquiries, reach out via:
- YouTube: [Teknotrek](https://www.youtube.com/@Teknotrek)
- GitHub Issues

