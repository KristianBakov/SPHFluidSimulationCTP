using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public class sphdll : MonoBehaviour
{
    //local parameters
    public int cellSize = 1;
    public float particleRadius;
    public float smoothingRadius;
    public float smoothingRadiusSq;
    public float restDensity;
    public float gravityMult;
    public float particleMass;
    public float particleViscosity;
    public float particleDrag;
    private int Amount = 1000;
    public GameObject Prefab;
    public int per_row = 3;
    public GameObject[] Colliders;
    //Imported Functions
    GameObject[] Particles;
    [DllImport("sphdll")]
    public static extern void Initialize();
    [DllImport("sphdll")]
    public static extern void InitColliders();
    [DllImport("sphdll")]
    public static extern void Clear();
    [DllImport("sphdll")]
    public static extern void CalculateDensityAndPressure();
    [DllImport("sphdll")]
    public static extern void CalculateForces();
    [DllImport("sphdll")]
    public static extern void CalculateCollisions();
    [DllImport("sphdll")]
    public static extern void SetParamsDefault();
    [DllImport("sphdll")]
    public static extern void Integrate();
    [DllImport("sphdll")]
    public static extern Vector3 V3abs(Vector3 v_in);
    [DllImport("sphdll")]
    public static extern Vector3 GetParticleScale(int num_in);
    [DllImport("sphdll")]
    public static extern Vector3 GetparticlePosition(int num_in);
    [DllImport("sphdll")]
    public static extern Vector3 GetColliderRealPos(int num_in);
    [DllImport("sphdll")]
    public static extern Vector3 GetColliderRealRot(int num_in);
    [DllImport("sphdll")]
    public static extern Vector3 GetColliderRealScale(int num_in);
    [DllImport("sphdll")]
    public static extern bool GetClearing();
    [DllImport("sphdll")]
    public static extern int SetPerRow(int i_in);
    [DllImport("sphdll")]
    public static extern void GetPerRow();
    [DllImport("sphdll")]
    public static extern float GetParticleRadius();
    [DllImport("sphdll")]
    public static extern float GetSmoothingRadius();
    [DllImport("sphdll")]
    public static extern float GetSmoothingRadiusSq();
    [DllImport("sphdll")]
    public static extern float GetRestDensity();
    [DllImport("sphdll")]
    public static extern float GetGravityMultiplier();
    [DllImport("sphdll")]
    public static extern float GetParticleMass();
    [DllImport("sphdll")]
    public static extern float GetParticleViscosity();
    [DllImport("sphdll")]
    public static extern float GetParticleDrag();
    [DllImport("sphdll")]
    public static extern void SetParticleRadius(float val_in);
    [DllImport("sphdll")]
    public static extern void SetSmoothingRadius(float val_in);
    [DllImport("sphdll")]
    public static extern void SetSmoothingRadiusSq(float val_in);
    [DllImport("sphdll")]
    public static extern void SetRestDensity(float val_in);
    [DllImport("sphdll")]
    public static extern void SetGravityMultiplier(float val_in);
    [DllImport("sphdll")]
    public static extern void SetParticleMass(float val_in);
    [DllImport("sphdll")]
    public static extern void SetParticleViscosity(float val_in);
        [DllImport("sphdll")]
    public static extern void SetParticleDrag(float val_in);
    [DllImport("sphdll")]
    public static extern void ClearHash();
    [DllImport("sphdll")]
    public static extern void SetCellSize(int i_in);
    [DllImport("sphdll")]
    public static extern int GetCellSize();


    void Start()
    {
        //set initial parameters for the C++
        SetCellSize(cellSize);
        SetParticleRadius(particleRadius);
        SetSmoothingRadius(smoothingRadius);
        SetSmoothingRadiusSq(smoothingRadiusSq);
        SetRestDensity(restDensity);
        SetGravityMultiplier(gravityMult);
        SetParticleMass(particleMass);
        SetParticleViscosity(particleViscosity);
        SetParticleDrag(particleDrag);
        SetPerRow(per_row);

        Particles = new GameObject[Amount];
        //initial setup for particles
        Initialize();
        for (int i = 0; i < Amount; i++)
        {
            GameObject particle = Instantiate(Prefab);
            particle.transform.localScale = GetParticleScale(i);
            particle.transform.position = GetparticlePosition(i);
            particle.name = "Particle" + i.ToString();
            Particles[i] = particle;
        }
        //initial setup for the walls
        InitColliders();
        for (int i = 0; i < Colliders.Length; i++)
        {
            Colliders[i].transform.position = GetColliderRealPos(i);
            Colliders[i].transform.eulerAngles = GetColliderRealRot(i);
            Colliders[i].transform.localScale = GetColliderRealScale(i);
        }        
    }

    void Update()
    {
        //Execute when cleared
        if (Input.GetKeyUp(KeyCode.Space))
        {
            Clear();
            SetPerRow(per_row);
            SetParticleRadius(particleRadius);
            SetSmoothingRadius(smoothingRadius);
            SetSmoothingRadiusSq(smoothingRadiusSq);
            SetRestDensity(restDensity);
            SetGravityMultiplier(gravityMult);
            SetParticleMass(particleMass);
            SetParticleViscosity(particleViscosity);
            SetParticleDrag(particleDrag);
        }

        if (GetClearing())
        {
            return;
        }
        //SPH Solver
        CalculateDensityAndPressure();
        CalculateForces();
        Integrate();
        CalculateCollisions();
        //assign position to particle game object
        for (int i = 0; i < Amount; i++)
        {
            Particles[i].transform.position = GetparticlePosition(i);
        }
        ClearHash();
    }


}
