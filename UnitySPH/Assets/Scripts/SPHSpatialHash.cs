using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SPHSpatialHash : MonoBehaviour
{
    // Consts
    private static Vector3 GRAVITY = new Vector3(0.0f, -9.81f, 0.0f);
    private const float GAS_CONST = 2000.0f;
    private const float DT = 0.0008f;
    private const float BOUND_DAMPING = -0.5f;

    [SerializeField] private GameObject Prefab = null;

    private int parameterID = 0;
    [SerializeField] private SPHParameters[] parameters = null;

    [SerializeField] private int amount = 250;
    [SerializeField] private int rowSize = 2;

    public int cellSize = 1;

    public int worldStartOffsetX = 0;
    public int worldStartOffsetY = 0;

    private SpatialHash<SPHParticle> hash;
    private bool clearing;

    // Data
    private SPHParticle[] particles;
    List<SPHParticle> neighbourParticles = new List<SPHParticle>();

    [System.Serializable]
    private struct SPHParameters
    {
#pragma warning disable 0649
        public float particleRadius;
        public float smoothingRadius;
        public float smoothingRadiusSq;
        public float restDensity;
        public float gravityMult;
        public float particleMass;
        public float particleViscosity;
        public float particleDrag;
#pragma warning restore 0649
    }

    private void Start()
    {
        InitSPH();
    }
    private void Update()
    {
        if (clearing)
        {
            return;
        }

        if(Input.GetKeyDown(KeyCode.Space))
        {
            Clear();
        }

        ComputeDensityPressure();
        ComputeForces();
        Integrate();
        ComputeColliders();
        ApplyPosition();
        hash.Clear();
    }

    public void Clear()
    {
        if (!clearing)
        {
            clearing = true;
        }

        foreach (SPHParticle p in particles)
        {
            Destroy(p.go);
        }
        InitSPH();
        clearing = false;
    }

    private void InitSPH()
    {
        particles = new SPHParticle[amount];
        hash = new SpatialHash<SPHParticle>(cellSize);

        for (int i = 0; i < amount; i++)
        {
            float jitter = (Random.value * 2f - 1f) * parameters[parameterID].particleRadius * 0.1f;
            float x = worldStartOffsetX + (i % rowSize) + Random.Range(-0.1f, 0.1f);
            float y = 2 + (float)((i / rowSize) / rowSize) * 1.1f;
            float z = worldStartOffsetY + ((i / rowSize) % rowSize) + Random.Range(-0.1f, 0.1f);

            GameObject go = Instantiate(Prefab);
            go.transform.localScale = Vector3.one * parameters[parameterID].particleRadius;
            go.transform.position = new Vector3(x + jitter, y, z + jitter);
            go.name = "Particle" + i.ToString();

            particles[i].Init(new Vector3(x, y, z), parameterID, go);
            particles[i].velocity = Vector3.zero;
            hash.Insert(particles[i].position, particles[i]);
        }
    }

    private void ComputeDensityPressure()
    {
        for (int i = 0; i < particles.Length; i++)
        {
            hash.Insert(particles[i].position, particles[i]);
            particles[i].density = 0.0f;

            //currentParticleBin = hash.GetBin(particles[i].position);
           // listNeighbours = hash.GetNeighbourBinsFromPosition(particles[i].position);
            neighbourParticles = hash.GetParticlesFromBins(hash.GetNeighbourBinsFromPosition(particles[i].position));
            //Debug.Log(neighbourParticles.Count);

            //look at neighbours
            for (int j = 0; j < neighbourParticles.Count; j++)
            {
                Vector3 dist = neighbourParticles[j].position - particles[i].position;
                float r2 = dist.sqrMagnitude;

                if (r2 < parameters[0].smoothingRadiusSq)
                {
                    particles[i].density += parameters[0].particleMass *
                        (315.0f / (64.0f * Mathf.PI * Mathf.Pow(parameters[0].smoothingRadius, 9.0f))) *
                        Mathf.Pow(parameters[0].smoothingRadiusSq - r2, 3.0f);
                }
            }

            particles[i].pressure = GAS_CONST * (particles[i].density - parameters[0].restDensity);
        }
    }

    private static bool Intersect(SPHCollider collider, Vector3 position, float radius,
    out Vector3 penetrationNormal, out Vector3 penetrationPosition, out float penetrationLength)
    {
        Vector3 colliderProjection = collider.position - position;

        penetrationNormal = Vector3.Cross(collider.right, collider.up);
        penetrationLength = Mathf.Abs(Vector3.Dot(colliderProjection, penetrationNormal)) - (radius / 2.0f);
        penetrationPosition = collider.position - colliderProjection;

        return penetrationLength < 0.0f
            && Mathf.Abs(Vector3.Dot(colliderProjection, collider.right)) < collider.scale.x
            && Mathf.Abs(Vector3.Dot(colliderProjection, collider.up)) < collider.scale.y;
    }



    private static Vector3 DampVelocity(SPHCollider collider, Vector3 velocity, Vector3 penetrationNormal, float drag)
    {
        Vector3 newVelocity = Vector3.Dot(velocity, penetrationNormal) * penetrationNormal * BOUND_DAMPING
                            + Vector3.Dot(velocity, collider.right) * collider.right * drag
                            + Vector3.Dot(velocity, collider.up) * collider.up * drag;
        newVelocity = Vector3.Dot(newVelocity, Vector3.forward) * Vector3.forward
                    + Vector3.Dot(newVelocity, Vector3.right) * Vector3.right
                    + Vector3.Dot(newVelocity, Vector3.up) * Vector3.up;
        return newVelocity;
    }



    private void ComputeColliders()
    {
        // Get colliders
        GameObject[] collidersGO = GameObject.FindGameObjectsWithTag("Collider");
        SPHCollider[] colliders = new SPHCollider[collidersGO.Length];
        for (int i = 0; i < colliders.Length; i++)
        {
            colliders[i].Init(collidersGO[i].transform);
        }

        if (clearing || colliders.Length == 0)
        {
            return;
        }

        for (int i = 0; i < particles.Length; i++)
        {
            for (int j = 0; j < colliders.Length; j++)
            {
                // Check collision
                Vector3 penetrationNormal;
                Vector3 penetrationPosition;
                float penetrationLength;
                if (Intersect(colliders[j], particles[i].position, parameters[particles[i].parameterID].particleRadius,
                    out penetrationNormal, out penetrationPosition, out penetrationLength))
                {
                    particles[i].velocity = DampVelocity(colliders[j], particles[i].velocity, penetrationNormal,
                        1.0f - parameters[particles[i].parameterID].particleDrag);
                    particles[i].position = penetrationPosition - penetrationNormal * Mathf.Abs(penetrationLength);
                }
            }
        }
    }



    private void Integrate()
    {
        for (int i = 0; i < particles.Length; i++)
        {
            // Cache
            Vector3 velocity = particles[i].velocity;
            Vector3 position = particles[i].position;

            // Process
            velocity += DT * particles[i].forceCombined / particles[i].density;
            position += DT * velocity;

            // Apply
            particles[i].velocity = velocity;
            particles[i].position = position;
        }
    }

    private void ComputeForces()
    {
        for (int i = 0; i < particles.Length; i++)
        {
            Vector3 forcePressure = Vector3.zero;
            Vector3 forceViscosity = Vector3.zero;

            //cache
            Vector3 position = particles[i].position;
            Vector3 velocity = particles[i].velocity;
            float pressure = particles[i].pressure;
            float density = particles[i].density;

            //listNeighbours = hash.GetBinNeighbourBinsFromPosition(particles[i].position);
            neighbourParticles = hash.GetParticlesFromBins(hash.GetNeighbourBinsFromPosition(particles[i].position));

            // Physics
            for (int j = 0; j < neighbourParticles.Count; j++)
            {
                if (i == j) continue;

                Vector3 rij = neighbourParticles[j].position - position;
                float r2 = rij.sqrMagnitude;
                float r = Mathf.Sqrt(r2);

                if (r < parameters[0].smoothingRadius)
                {
                    forcePressure += -rij.normalized * parameters[0].particleMass *
                        (2.0f * pressure) / (2.0f * density) *
                        (-45.0f / (Mathf.PI * Mathf.Pow(parameters[0].smoothingRadius, 6.0f))) *
                        Mathf.Pow(parameters[0].smoothingRadius - r, 2.0f);

                    forceViscosity += parameters[0].particleViscosity *
                        parameters[0].particleMass *
                        (neighbourParticles[j].velocity - velocity) / density *
                        (45.0f / (Mathf.PI * Mathf.Pow(parameters[0].smoothingRadius, 6.0f))) *
                        (parameters[0].smoothingRadius - r);
                }
            }

            Vector3 forceGravity = GRAVITY * density * parameters[0].gravityMult;

            // Apply
            particles[i].forceCombined = forcePressure + forceViscosity + forceGravity;
        }
    }

    private void ApplyPosition()
    {
        for (int i = 0; i < particles.Length; i++)
        {
            particles[i].go.transform.position = particles[i].position;
            // hash.UpdatePosition(particles[i].position, particles[i]);
        }
    }

}