using System.Linq;
using UnityEngine;
using System.Collections.Generic;

public class SpatialHash<T>
{
    private Dictionary<int, List<T>> dict;
    private Dictionary<T, int> objects;
    private int cellSize;

    private static int[] cellOffsetTable =
{
        1, 1, 1, 1, 1, 0, 1, 1, -1, 1, 0, 1, 1, 0, 0, 1, 0, -1, 1, -1, 1, 1, -1, 0, 1, -1, -1,
        0, 1, 1, 0, 1, 0, 0, 1, -1, 0, 0, 1, 0, 0, 0, 0, 0, -1, 0, -1, 1, 0, -1, 0, 0, -1, -1,
        -1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, 1, -1, 0, 0, -1, 0, -1, -1, -1, 1, -1, -1, 0, -1, -1, -1
    };

    public SpatialHash(int cellSize)
    {
        this.cellSize = cellSize;
        dict = new Dictionary<int, List<T>>();
        objects = new Dictionary<T, int>();
    }

    public void Insert(Vector3 vector, T obj)
    {
        var key = Key(vector);
        if (dict.ContainsKey(key))
        {
            dict[key].Add(obj);
        }
        else
        {
            dict[key] = new List<T> { obj };
        }
        objects[obj] = key;
    }

    public List<T> GetBin(Vector3 vector)
    {
        var key = Key(vector);
        if (dict.ContainsKey(key))
        {
            return dict[key];
        }
        else
        {
            //   Debug.LogError("The key" + key + "is not in the dictionary");
            return null;
        }
    }

    public List<List<T>> GetNeighbourBinsFromPosition(Vector3 vector)
    {
        // List<T> bin = GetBin(vector);
        List<List<T>> neighbours = new List<List<T>>();
        int i = 0;
        Vector3 gridOffset;

        for (int oi = 0; oi < 27; oi++)
        {
            i = oi * 3;
            gridOffset = new Vector3(cellOffsetTable[i], cellOffsetTable[i + 1], cellOffsetTable[i + 2]);
            Vector3 hash = vector + gridOffset;

            if (GetBin(hash) != null)
            {
                neighbours.Add(GetBin(hash));
            }
        }

        return neighbours;
    }

    public List<T> GetParticlesFromBins(List<List<T>> bins)
    {
        List<T> particles = new List<T>();

        foreach (List<T> list in bins)
        {
            particles.AddRange(list);
        }

        return particles;
    }

    public void UpdatePosition(Vector3 vector, T obj)
    {
        if (objects.ContainsKey(obj))
        {
            dict[objects[obj]].Remove(obj);
        }
        Insert(vector, obj);
    }

    public List<T> QueryPosition(Vector3 vector)
    {
        var key = Key(vector);
        return dict.ContainsKey(key) ? dict[key] : new List<T>();
    }

    public bool ContainsKey(Vector3 vector)
    {
        return dict.ContainsKey(Key(vector));
    }

    public void Clear()
    {
        var keys = dict.Keys.ToArray();
        for (var i = 0; i < keys.Length; i++)
            dict[keys[i]].Clear();
        objects.Clear();
    }

    public void Reset()
    {
        dict.Clear();
        objects.Clear();
    }

    private const int BIG_ENOUGH_INT = 16 * 1024;
    private const double BIG_ENOUGH_FLOOR = BIG_ENOUGH_INT + 0.0000;

    private static int FastFloor(float f)
    {
        return (int)(f + BIG_ENOUGH_FLOOR) - BIG_ENOUGH_INT;
    }

    private int Key(Vector3 v)
    {
        return ((FastFloor(v.x / cellSize) * 73856093) ^
                (FastFloor(v.y / cellSize) * 19349663) ^
                (FastFloor(v.z / cellSize) * 83492791));
    }
}
