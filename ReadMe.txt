[Gens Input Display 3.2]
Built off of TurtleMan64's Sonic Input Display v2.2.

[Features]
Changes to the original:
- De-coupled the D-Pad from the joystick and added D-Pad buttons. Coupling can be toggled (F5).
- De-coupled the triggers and shoulder buttons and added trigger buttons. 
- Fixed issue where the thinner line on the ring was drawing outside the boundary

New Features:
- Added a toggle-able coordinate display (F1).
- Added a toggle-able animated toon (F2) to the bottom right corner.
	* For those that want to create custom toons, the animation must be 8 frames and should match the
	  image size of the toon.png and toonM.png files found in each folder
- Added a toggle-able M-Speed light (F3) that makes the ring and pointer light up when the joystick's X and Y
  match the known M-Speed equation. Stick sensitivity should be set to 50 in-game.
- Added a toggle-able "100 Stick Sensitivity" mode (F4) that adjusted the M-Speed coordinate set to match
  values found with the stick sensitivity set to 100.

Standard Features (from Sonic Input Display 2.2):
- You can switch between skins by using left/right arrow keys on your keyboard. New skins can be added by simply creating a new folder with the same filenames in them.
- The background color can be defined in `BackgroundColor.ini`, (RGB values from 0-255)
- The stick color can be defined in `StickColor.ini`, (RGB values from 0-255)
- As of version 2.2, capturing in OBS supports transparency. This means you will not have to color/chroma key out the background. Just use Game Capture, and select the "Allow Transparency" checkbox.
- The stick line width can be defined in `StickLineWidth.ini`, (0 for thin 1 for thick)

Commands:
F1 - Toggle Coordinates
F2 - Toggle Toon
F3 - Toggle M-Speed Lights
F4 - Toggle 100 Stick Sensitivity Mode (under construction)
F5 - Toggle D-Pad + Joystick Coupling

[Notes]
Extra special thanks to Khroma for testing 100 stick sensitivity mode! I was able to use that information to come up with a fairly accurate M-Speed Equation to test against instead of individual coordinates!

[Change Log]
<3.2> 	-Fixed crash with Coordinates switch
	-Capped framerate to help with CPU usage
	-Adjusted MSpeed equations to include edge cases
<3.1> 	-Fixed massive memory leak 
	-Added MSpeed equations for both regular and 100 Stick Sensitivity Mode

	

[Credits]
Kyrrone: Original creator of the M/D Speed template
https://www.youtube.com/@Kyrrone
Starlight: The remaster button prompts were taken from the Remastered GUI Mod
https://www.youtube.com/@StarlightDX
Khroma: Creating the updated M/D Speed template and testing 100 stick sensitivity mode.
https://www.youtube.com/@SlavicSnowball
TurtleMan64: Original Sonic Input Display v2.2
https://github.com/TurtleMan64/SADX-SA2-Input-Display/releases/tag/v2.2
