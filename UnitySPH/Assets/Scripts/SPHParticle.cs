using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public struct SPHParticle
{
    public Vector3 position;

    public Vector3 velocity;
    public Vector3 forceCombined;

    public float density;
    public float pressure;

    public int parameterID;
    public Vector3 gridPos;

    public GameObject go;


    public void Init(Vector3 _position, int _parameterID, GameObject _go)
    {
        position = _position;
        parameterID = _parameterID;
        go = _go;

        velocity = Vector3.zero;
        forceCombined = Vector3.zero;
        density = 0.0f;
        pressure = 0.0f;
    }
}
