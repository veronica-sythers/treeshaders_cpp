#include "utils.hpp"


double normalize(double value, double min_original, double max_original) {
    return (value - min_original) / (max_original - min_original);
}


double remap(double value, double old_min, double old_max, double new_min, double new_max) {
    return new_min + ((value - old_min) / (old_max - old_min)) * (new_max - new_min);
}