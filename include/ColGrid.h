//
// Created by jammer on 04/04/17.
//

#ifndef SINGING_WIND_COLGRID_H
#define SINGING_WIND_COLGRID_H

#include "WindDefs.h"
#include <unordered_map>
#include <set>
#include <memory>
#include <vector>

class ColShape;

class HashGrid {
public:
    void add_object(std::shared_ptr<ColShape> shape);
    void clear();
    std::vector<std::shared_ptr<ColShape>> find_colliders(const std::shared_ptr<ColShape> &shape);
    std::vector<std::shared_ptr<ColShape>> get_objects() const {return m_objects;}
    std::vector<std::shared_ptr<ColShape>> get_colliders_of_point(const WVec &p);

    unsigned int get_size() const {return m_cell_size;}

    HashGrid();
    ~HashGrid() = default;

private:
    unsigned int m_cell_size;
    std::vector<std::shared_ptr<ColShape>> m_objects;
    std::unordered_map<int, std::vector<size_t>> m_buckets;
    int get_id(const WVec &v) const;
    std::set<int> get_ids_for_object(const std::shared_ptr<ColShape> &shape) const;
};


#endif //SINGING_WIND_COLGRID_H
