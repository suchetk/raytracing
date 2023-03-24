//
// Created by Suchet Kumar on 3/18/23.
//

#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
private:
    std::vector<shared_ptr<hittable>> objects;
public:
    hittable_list() = default;
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) { objects.push_back(object); }

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        hit_record temp;
        bool hit_anything = false;
        auto closest_so_far = t_max;

        for (const auto& object : objects) {
            if (object->hit(r, t_min, closest_so_far, temp)) {
                hit_anything = true;
                closest_so_far = temp.t;
                rec = temp;
            }
        }

        return hit_anything;
    }
};

#endif
