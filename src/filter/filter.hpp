
#ifndef _462_FILTER_HPP_
#define _462_FILTER_HPP_

#include "math/math.hpp"

namespace _462 {

class Filter {
public:
    Filter(float w_x, float w_y) :
	width_x(w_x), width_y(w_y), filter_table(NULL),
<<<<<<< HEAD
	table_edge(16) { }
=======
	table_edge(32) { }
>>>>>>> 9612b61bec1ef47036192ed0a454ddc67da31fc3
    ~Filter() {
	delete filter_table;
    }

    virtual float evaluate(float x, float y) = 0;
    virtual void populate();

    float width_x, width_y;
    uint32_t table_edge;
    float *filter_table;
};

}

#endif
