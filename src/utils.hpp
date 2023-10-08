#ifndef UTILS_H
#define UTILS_H


double normalize(double value, double min_original, double max_original);

double remap(double value, double old_min, double old_max, double new_min, double new_max);

#endif