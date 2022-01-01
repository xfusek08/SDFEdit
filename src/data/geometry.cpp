
#include <data/geometry.h>
#include <data/primitives.h>

void Geometry::addEdit(GeometryEdit edit)
{
    edits.push_back(edit);
    auto toadd =  primitives::computeAABB(edit);
    aabb = AABB::add(aabb, toadd);
}

void Geometry::addEdits(const std::vector<GeometryEdit>& edits)
{
    for (auto edit : edits) {
        addEdit(edit);
    }
}

void Geometry::clearEdits()
{
    edits.clear();
}
