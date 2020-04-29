The builds folder contains builds of:

Showcase - the gameplay scene
Additional builds of the 3 implementations

The project contains:

Game environment scene:
- Game environment SPH using the C# with SH implementation

ShowcaseSPH scene:
In this scene the 3 implementations are on the Manager script. By default it will run the C++ implementation.
To change it, activate the single script for one of the following:

- C++ SPH (1000 particles at 60FPS) with sphdll script
- C# with SH (450 particles at 60FPS) with SPH Spatial Hash script
- C# without SH (450 particles at 60FPS) with NoHashSPH script

