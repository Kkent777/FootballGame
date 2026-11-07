# FootballGame

A third-person football prototype built with C++ and Blueprints in Unreal Engine 5.5. Charge a dash, glide across the pitch and release a shot with a physics-driven ball. AI characters patrol the field. The goal volume logs each goal.

## Controls

| Action | Input |
| --- | --- |
| Move, look, jump | W/A/S/D, Q/E, Space |
| Dash | Hold and release Left Shift |
| Glide | Hold G |
| Shoot | Hold and release T |

The [player input bindings](Source/FootballGame/FootballGameCharacter.cpp) implement the player actions. Pass/intercept, tackle, adrenaline and low-gravity inputs currently write to the log.

## Run in Unreal Editor

Open `FootballGame.uproject` with Unreal Engine 5.5 and its C++ toolchain. The gameplay level needs local mannequin, ball and stadium assets. Follow the [scene setup guide](docs/scene-setup.md) for the required paths and Blueprint defaults, then open [the menu level](Content/ThirdPerson/Levels/MainMenu_L.umap) and use Play in Editor.
