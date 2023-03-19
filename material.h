//
// Created by Suchet Kumar on 3/19/23.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"

class material {
public:
    virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};



#endif