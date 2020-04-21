using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public struct SPHCollider
{
    public Vector3 position;
    public Vector3 right;
    public Vector3 up;
    public Vector2 scale;

    public void Init(Transform _transform)
    {
        position = _transform.position;
        right = _transform.right;
        up = _transform.up;
        scale = new Vector2(_transform.lossyScale.x / 2f, _transform.lossyScale.y / 2f);
    }
}
