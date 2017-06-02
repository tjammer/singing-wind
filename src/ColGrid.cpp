//
// Created by jammer on 04/04/17.
//

#include "ColGrid.h"
#include "ColShape.h"

const int P1 = 73856093;
const int P2 = 19349663;

StaticGrid::StaticGrid() {
    m_cell_size = static_cast<unsigned int>(c_line_triangulate_split * 1.5f);
}

int StaticGrid::get_id(const WVec &v) const {
    int x = static_cast<int>(v.x / m_cell_size);
    int y = static_cast<int>(v.y / m_cell_size);
    return x * P1 ^ y * P2;
}

std::set<int> StaticGrid::get_ids_for_object(const std::shared_ptr<ColShape> &shape) const {
    std::set<int> ids;
;
    int times = static_cast<int>(2.f * shape->get_radius() / m_cell_size) + 1;
    for (int i = 0; i <= times; ++i) {
        for (int j = 0; j <= times; ++j) {
            int id = get_id(shape->m_center + WVec(static_cast<float>(i * m_cell_size) - shape->get_radius(),
                                                   static_cast<float>(j * m_cell_size) - shape->get_radius()));
            ids.insert(id);
        }
    }
    return ids;
}

void StaticGrid::clear() {
    m_buckets = std::unordered_map<int, std::vector<size_t>>();
    m_objects.clear();
}

void StaticGrid::add_object(std::shared_ptr<ColShape> shape) {
    size_t pos = m_objects.size();
    auto ids = get_ids_for_object(shape);
    m_objects.push_back(std::move(shape));
    for (auto id : ids)
        m_buckets[id].push_back(pos);
}

std::vector<std::shared_ptr<ColShape>> StaticGrid::find_colliders(const std::shared_ptr<ColShape> &shape) {
    std::vector<std::shared_ptr<ColShape>> colliders;
    std::set<size_t> indices;
    for (const auto &id : get_ids_for_object(shape)) {
        for (size_t i : m_buckets[id]) {
            indices.insert(i);
        }
    }
    for (auto i : indices) {
        colliders.push_back(m_objects[i]);
    }
    return colliders;
}

std::vector<std::shared_ptr<ColShape>> StaticGrid::get_colliders_of_point(const WVec &p) {
    std::vector<std::shared_ptr<ColShape>> colliders;
    auto id = get_id(p);
    for (auto i : m_buckets[id]) {
        colliders.push_back(m_objects[i]);
    }
    return colliders;
}
