# FootballGame

An Unreal Engine 5.5 third-person football prototype. The player charges a dash, glides through the air and releases a charged shot. A spawned ball receives a physics impulse, and a goal volume detects it crossing the line. The project began as a coursework prototype built on Unreal's Third Person template.

## Explore the project

| System | Implementation |
| --- | --- |
| Player movement and shooting | [`FootballGameCharacter.cpp`](Source/FootballGame/FootballGameCharacter.cpp) binds Enhanced Input actions to movement states. Dash and shot strength scale with charge time. The default maximum charge is 1.2 seconds for both. |
| Ball and goal | [`Ball.cpp`](Source/FootballGame/Ball.cpp) handles physics impulses. [`Goal.cpp`](Source/FootballGame/Goal.cpp) checks for a moving ball, logs the opposing team's goal and respawns a ball when a spawn class is set. |
| AI perception and patrol | [`FootballAIController.cpp`](Source/FootballGame/FootballAIController.cpp) writes perceived player and ball positions to a [Blackboard and Behavior Tree](Content/ThirdPerson/AI). [`BTTask_FindPatrolPoint.cpp`](Source/FootballGame/BTTask_FindPatrolPoint.cpp) searches the navigation mesh for a reachable patrol point. |
| Interface | The [main menu and HUD widgets](Content/ThirdPerson/UI) accompany the [menu level](Content/ThirdPerson/Levels/MainMenu_L.umap). [`FootballInGameHUDNew.cpp`](Source/FootballGame/FootballInGameHUDNew.cpp) updates the dash charge bar and adrenaline status text. |

Goal detection writes to the output log. The prototype has no persistent score tracker.

## Controls

The [original mapping context](Content/ThirdPerson/Input/IMC_Default.uasset) and [player Blueprint](Content/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.uasset) retain the game's input assignments and HUD reference.

| Input | Binding | Behavior in the prototype |
| --- | --- | --- |
| Move, look, jump | W/A/S/D, Q/E, Space | Third-person controls |
| Dash | Left Shift | Hold to charge and release to dash |
| Glide | G | Press to attempt a glide and release to stop |
| Shoot | T | Hold to charge and release to launch a ball |
| Kick | C | Defined in the mapping context. The player C++ input handler has no kick binding. |
| Pass/intercept, tackle | 2, 1 | Log the requested action |
| Toggle adrenaline, toggle low gravity | Z, N | Log the requested action |

## Open the original scene

The [gameplay map](Content/ThirdPerson/Maps/ThirdPersonMap.umap), its baked data and its World Partition packages are included. They preserve 1,110 external actor packages and two external object packages, including the original placements of field tiles, stands, goals and AI characters. A few external meshes and animations need local replacements.

1. Install Unreal Engine 5.5, its C++ toolchain, the Third Person template and Starter Content. Open `FootballGame.uproject` and build the `FootballGameEditor` target in Development Editor configuration. Migrate the matching mannequin, animation and ball assets from your local Epic installation, preserving the paths below or updating the Blueprints.
2. Supply your own stadium meshes at the six `/Game/Meshes/` asset paths listed below. Set collision on the field mesh. Using the original paths allows every placed instance to resolve its mesh together. You can start with simple blockout geometry for the field and stands.
3. In `BP_ThirdPersonCharacter`, inspect the `BallProjectileClass` default and set it to the included `BP_Ball`. The C++ shot handler requires that class. The player Blueprint already references the football action definitions, input context and in-game HUD.
4. Open `MainMenu_L` and use Play in Editor to enter `ThirdPersonMap`. You can also open `ThirdPersonMap` directly to inspect the scene and its World Partition cells.

### Asset paths to restore

| Used by | Original asset paths |
| --- | --- |
| Player mannequin and animation | `/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple`, `/Game/Characters/Mannequins/Animations/ABP_Quinn` |
| AI mannequin and animation | `/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin`, `/Game/Characters/Mannequins/Animations/ABP_Manny` |
| Ball mesh | `/Game/StarterContent/Props/MaterialSphere` |
| Field, stands and goal geometry | `/Game/Meshes/SM_Floor`, `/Game/Meshes/SM_Stands`, `/Game/Meshes/SM_StandStair`, `/Game/Meshes/SM_Corner`, `/Game/Meshes/SM_Straight`, `/Game/Meshes/SM_Goal` |

## Distribution and verification

The source retains its original copyright notices. The player Blueprint, mapping context and gameplay level derive from Unreal's Third Person template and contain football-specific changes. Unreal Engine, mannequin artwork and Starter Content can be obtained from Epic. Venue meshes of unverified origin are omitted. Epic's terms for distributing modified Examples appear in section 5(b) of the [Unreal Engine EULA](https://www.unrealengine.com/eula/unreal).

The public snapshot was checked against the original Git objects. The project descriptor, Unreal package headers, input bindings and scene asset references were inspected. Compilation and Play in Editor were not run for this release. Build and play behavior still need verification in Unreal Engine 5.5.

[中文项目简介（PDF）](docs/portfolio/Project-Brief-ZH.pdf)
