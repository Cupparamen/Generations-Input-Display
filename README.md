# Gens Input Display 4.43
(written by AI lol)
Based on TurtleMan64's Sonic Input Display v2.2.

Gens Input Display 4.4 is a dynamic controller input visualizer written in C++ using SDL2, SDL_image, and SDL_ttf. It captures real-time game controller inputs and converts them into a customizable visual overlay. 

## Overview

Gens Input Display renders a dynamic visual overlay that represents various controller inputs. It features:
- **Real-Time Input Visualization:** Continuously polls controller input (buttons, joysticks, triggers) and renders them smoothly.
- **Customizable Overlay Elements:** Includes a joystick indicator, decorative rings, crosshairs, dynamic glow effects, and customizable shapes.
- **Interactive Settings Dialog:** A separate SDL window allows users to adjust visual parameters (colors, sizes, pulse rates, etc.) in real time.
- **File-Based Configuration:** Visual parameters are loaded from and saved to a configuration file (`settings.ini`).

### Hotkeys
- **F1:** Toggle coordinate display.
- **F2:** Toggle an animated toon in the bottom right corner.
  - *Note:* Custom toon animations must have 8 frames and match the dimensions of the `toon.png` and `toonM.png` files in each folder under `./toons/`.
- **F3:** Toggle M-Speed light mode, lighting up the ring and pointer when the joystick’s X and Y values match the M-Speed equation. (Set stick sensitivity to 50 in-game.)
- **F4:** Toggle D-Pad Mode to allow D-Pad inputs to affect the ring.
- **F5:** Cycle through available ring shapes.
- **F6** Toggle-able button shadows
- **F7** Cycle through available button layouts.
  
- **Left/Right Arrow Keys:** Switch between button sets located in the `./sets/` folder. (New sets can be added by creating a folder with the same filenames for the button images.)
- **Up/Down Arrow Keys:** Cycle through available Toons
- **m** opens the menu

### Standard Features
- **Custom Button Sets:** Easily switch between button sets by placing images with the same filenames (e.g., `buttA.png`, `buttB.png`, etc.) in a new folder under `./sets/`.
- **OBS Compatibility:** Supports transparency, so you can capture the overlay without a background using Game Capture with “Allow Transparency.”
- **M-Speed Threshold Customization:** Adjust thresholds in the settings dialog to fine-tune M-Speed detection. The settings let you define:
  - The **Ceiling** (maximum value) and **Floor** (minimum value) for M-Speed activation per cardinal direction (North, West, East, South).
  - Corner ranges via minimum and maximum values for precise detection.

# Credits

Kyrrone: Original creator of the M/D Speed template
https://www.youtube.com/@Kyrrone

Starlight: The remaster button prompts were taken from the Remastered GUI Mod
https://www.youtube.com/@StarlightDX

Khroma: Creating the updated M/D Speed template and testing 100 stick sensitivity mode.
https://www.youtube.com/@SlavicSnowball

TurtleMan64: Original Sonic Input Display v2.2
https://github.com/TurtleMan64/SADX-SA2-Input-Display/releases/tag/v2.2

Ranbin: Unleashed Button Set
https://www.youtube.com/@R_A_N_B_I_N

 Avikaielef: Unleashed Button Sets
 https://www.youtube.com/@Avikaielef
 
#### How to compile:     

* 4.0 (C++)
   * Open the `SonicInputDisplay.sln` in Visual Studio 2017. Then hit build and it will produce the final `SonicInputDisplay.exe`.
   * Currently you may have to play around with the SDL libraries to get it to work properly.

