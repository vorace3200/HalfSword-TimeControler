# HalfSword-TimeControler

An easy-to-use software that allows you to control time in Half Sword. Adjust time dilation dynamically with a simple slider or hotkey, offering seamless control for slow-motion effects or hyper speed.

> **UPDATE:** This tool has been updated to support the latest version of Half Sword (Early Access). I will look into continuing to update it as the game evolves.

## New Features
- **Player Stats:** God Mode, Infinite Stamina, No Pain, Max Consciousness
- **Movement:** Super Speed, Super Jump, Gravity Scale, Mass Control
- **Combat:** Punch Force, Kick Force, Muscle Power, Damage Multiplier

## How to use?
```bash
   1 - Install the TimeControl.exe
   2 - Open it before or after launching Half Sword
   3 - You can now press the assigned key to change the time.
```

## Questions
   - When I download it or check on VirusTotal, it shows virus detections. Why?
         This happens because the mod includes functions like WriteMemory and ReadMemory
         to interact directly with the game’s memory. These functions are commonly flagged by antivirus
﻿﻿         software as potentially harmful, even though they used here solely to change specific in-game values,
         such as altering the time for the slow-motion effect.

   - I still want to play with the mod, but I'm unsure because of these issues.
﻿  ﻿       Since the project is open source, you can check out the code on GitHub and even compile it yourself for added assurance.
         The part of the code that modifies the values is primarily located in `src/game/PlayerManagerSDK.hpp` and `src/ui/UIManager.cpp`.


## Video

https://github.com/user-attachments/assets/98cf6bc2-daf4-4d37-be87-c2ce1968e1bf
    
## Screenshots

![App Screenshot](https://staticdelivery.nexusmods.com/mods/6067/images/headers/16_1733111311.jpg)
![App Screenshot](https://staticdelivery.nexusmods.com/mods/6067/images/16/16-1733111339-1993519505.png)

## Credit

ImGui Loader Base from notaila: https://github.com/notaila/ImGui-Loader-Base
