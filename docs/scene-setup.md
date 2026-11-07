# Set up the gameplay scene

The [gameplay map](../Content/ThirdPerson/Maps/ThirdPersonMap.umap), its baked data and its World Partition packages are included. They preserve 1,110 external actor packages and two external object packages, including the original placements of field tiles, stands, goals and AI characters. A few external meshes and animations need local replacements.

1. Install Unreal Engine 5.5, its C++ toolchain, the Third Person template and Starter Content. Open `FootballGame.uproject` and build the `FootballGameEditor` target in Development Editor configuration. Migrate the matching mannequin, animation and ball assets from your local Epic installation, preserving the paths below or updating the Blueprints.
2. Supply your own stadium meshes at the six `/Game/Meshes/` asset paths listed below. Set collision on the field mesh. Using the original paths allows every placed instance to resolve its mesh together. You can start with simple blockout geometry for the field and stands.
3. In `BP_ThirdPersonCharacter`, inspect the `BallProjectileClass` default and set it to the included `BP_Ball`. The C++ shot handler requires that class. The player Blueprint already references the football action definitions, input context and in-game HUD.
4. Open `MainMenu_L` and use Play in Editor to enter `ThirdPersonMap`. You can also open `ThirdPersonMap` directly to inspect the scene and its World Partition cells.

## Asset paths to restore

| Used by | Original asset paths |
| --- | --- |
| Player mannequin and animation | `/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple`, `/Game/Characters/Mannequins/Animations/ABP_Quinn` |
| AI mannequin and animation | `/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin`, `/Game/Characters/Mannequins/Animations/ABP_Manny` |
| Ball mesh | `/Game/StarterContent/Props/MaterialSphere` |
| Field, stands and goal geometry | `/Game/Meshes/SM_Floor`, `/Game/Meshes/SM_Stands`, `/Game/Meshes/SM_StandStair`, `/Game/Meshes/SM_Corner`, `/Game/Meshes/SM_Straight`, `/Game/Meshes/SM_Goal` |

## Asset provenance

The source retains its original copyright notices. The player Blueprint, mapping context and gameplay level derive from Unreal's Third Person template and contain football-specific changes. Unreal Engine, mannequin artwork and Starter Content can be obtained from Epic. Venue meshes of unverified origin are omitted. Epic's terms for distributing modified Examples appear in section 5(b) of the [Unreal Engine EULA](https://www.unrealengine.com/eula/unreal).

The published source and asset references were checked against the original project. An editor build and Play in Editor run have yet to be verified for this snapshot.
