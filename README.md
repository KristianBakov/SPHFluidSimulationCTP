# Smoothed Particle Hydrodynamics in Unity
Showcase video available at: The video will be made available in May 2020.
Project instructions available at the bottom of the read me.

This is an implementation of the SPH method inside of Unity, suitable for use in game scenes.
Builds can be found inside the Builds folder.

![Showcase Image](images/showcase.png)


The implementation uses 3 methods, differences between which is shown in the image below.
Left - C# SPH without spatial hashing. Number of particles at 60FPS - 300.
Middle - C# SPH with spatial hashing. Number of particles at 60FPS - 450.
Right - C++ DLL integration with C#. Number of particles at 60FPS - 1000.

![SPH Comparison Image](images/3comparison.png)

Project instructions:
The Unity project contains a gameplay environemnt scene - the one in the showcase image and a SPHShowcase scene where the 3 benchmarks are compared. Each implementation requires a different script, while the C++ implementation requires a script and the DLL. A project to build the DLL is also provided under the sphdll folder. 

NOTE: FPS counter is not fully accurate.

<b> Contents: </b>
The builds folder contains builds of:

Showcase - the gameplay scene;
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


