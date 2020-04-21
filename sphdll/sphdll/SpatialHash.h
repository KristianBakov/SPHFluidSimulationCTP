#pragma once
#ifndef SPATIAL_HASH
#define SPATIAL_HASH

#include <map>
#include <vector>

//template<typename TK, typename TV>
//std::vector<TV> extract_values(std::map<TK, TV> const& input_map) {
//    std::vector<TV> retval;
//    for (auto const& element : input_map) {
//        retval.push_back(element.second);
//    }
//    return retval;
//}


class SpatialHash
{
private:
    std::map<int, std::vector<Particle>> dict;
    std::map<Particle, int> objects;
    int cellSize;

    const int cellOffsetTable[81] =
    {
            1, 1, 1, 1, 1, 0, 1, 1, -1, 1, 0, 1, 1, 0, 0, 1, 0, -1, 1, -1, 1, 1, -1, 0, 1, -1, -1,
            0, 1, 1, 0, 1, 0, 0, 1, -1, 0, 0, 1, 0, 0, 0, 0, 0, -1, 0, -1, 1, 0, -1, 0, 0, -1, -1,
            -1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, 1, -1, 0, 0, -1, 0, -1, -1, -1, 1, -1, -1, 0, -1, -1, -1
    };

    const int BIG_ENOUGH_INT = 16 * 1024;
    const double BIG_ENOUGH_FLOOR = BIG_ENOUGH_INT + 0.0000;
    int Key(Vector3 v);

public:
    SpatialHash() = default;
    SpatialHash(int cellSize) { this->cellSize = cellSize; };

    std::vector<int> extract_keys(std::map<int, std::vector<Particle>> const& input_map);

    void Insert(Vector3 vector, Particle obj);
    std::vector<Particle> GetBin(Vector3 vector);
    bool Bin(Vector3 vector);
    std::vector<std::vector<Particle>> GetNeighbourBinsFromPosition(Vector3 vector);
    std::vector<Particle> GetParticlesFromBins(std::vector<std::vector<Particle>> bins);
    std::vector<Particle> QueryPosition(Vector3 vector);
    bool ContainsKey(Vector3 vector);
    void Clear();
    void Reset();
    int FastFloor(float f)
    {
        return (int)(f + BIG_ENOUGH_FLOOR) - BIG_ENOUGH_INT;
    }
};

void SpatialHash::Insert(Vector3 vector, Particle obj)
{
    auto key = Key(vector);
    if (dict.count(key))
    {
        dict[key].push_back(obj);
    }
    else
    {
        dict[key] = std::vector<Particle>{ obj };
    }
    objects[obj] = key;
}

std::vector<Particle> SpatialHash::GetBin(Vector3 vector)
{
    auto key = Key(vector);
    if (dict.count(key))
    {
        return dict[key];
    }
    else
    {
        //This should not return
        // return NULL;
    }
}

bool SpatialHash::Bin(Vector3 vector)
{
    auto key = Key(vector);
    if (dict.count(key))
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<std::vector<Particle>> SpatialHash::GetNeighbourBinsFromPosition(Vector3 vector)
{
    std::vector<std::vector<Particle>> neighbours;
    int i = 0;
    Vector3 gridOffset;

    for (int oi = 0; oi < 27; oi++)
    {
        i = oi * 3;
        gridOffset = Vector3(cellOffsetTable[i], cellOffsetTable[i + 1], cellOffsetTable[i + 2]);
        Vector3 hash = vector + gridOffset;

        if (Bin(hash))
        {
            neighbours.push_back(GetBin(hash));
        }
    }

    return neighbours;
}

std::vector<Particle> SpatialHash::GetParticlesFromBins(std::vector<std::vector<Particle>> bins)
{
    std::vector <Particle> particles;

    for (std::vector<Particle>& list : bins)
    {
        particles.insert(particles.end(), list.begin(), list.end());
    }

    return particles;
}

std::vector<Particle> SpatialHash::QueryPosition(Vector3 vector)
{
    auto key = Key(vector);
    return dict.count(key) ? dict[key] : std::vector<Particle>();
}

bool SpatialHash::ContainsKey(Vector3 vector)
{
    return dict.count(Key(vector));
}

void SpatialHash::Clear()
{
    auto keys = extract_keys(dict);
    for (auto i = 0; i < keys.size(); i++)
    {
        dict.erase(keys[i]);
    }
    objects.clear();
}


void SpatialHash::Reset()
{
    dict.clear();
    objects.clear();
}

int SpatialHash::Key(Vector3 v)
{
    return ((FastFloor(v.x / cellSize) * 73856093) ^
        (FastFloor(v.y / cellSize) * 19349663) ^
        (FastFloor(v.z / cellSize) * 83492791));
}

std::vector<int> SpatialHash::extract_keys(std::map<int, std::vector<Particle>> const& input_map) {
    std::vector<int> retval;
    for (auto const& element : input_map) {
        retval.push_back(element.first);
    }
    return retval;
}

#endif // !SPATIAL_HASH