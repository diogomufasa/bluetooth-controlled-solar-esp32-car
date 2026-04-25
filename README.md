# Solar ESP32 Motor Car

A small car powered by an ESP32 and a single 18650 battery, with a custom 3D‑printed chassis, Bluetooth control, and a solar panel to help charge the battery.

<img width="1264" height="843" alt="final_design" src="https://github.com/user-attachments/assets/421f803e-053f-4962-9c76-ca3943ccd9be" />


## Project idea

- ESP32 DevKit V1 handles Bluetooth and car logic  
- L298N drives two DC gear motors  
- 18650 + TP4056 + MT3608 power the ESP32 and motors  
- 6V 1W solar panel planned for charging  
- Custom 3D‑printed chassis designed for this layout  

## Components

- ESP32 DevKit V1  
- L298N motor driver  
- 2x DC gear motors  
- 18650 Li‑ion battery + holder  
- TP4056 charger module  
- MT3608 boost converter  
- 6V 1W solar panel  
- 3D‑printed chassis  

```text
bluetooth-controlled-solar-esp32-car/
├── firmware/
├── cad/
├── images/
└── README.md
```

## Next steps
- Print and test the chassis  
- Write Bluetooth control firmware  
- Hook up and test the solar charging
