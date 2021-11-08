
#include <types/geometry.h>
#include <types/primitives.h>

void Geometry::addEdit(GeometryEdit edit)
{
    edits.push_back(edit);
    aabb = AABB::add(aabb, primitives::computeAABB(edit));
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
