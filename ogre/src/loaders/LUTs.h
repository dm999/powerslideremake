#ifndef LUTS_H
#define LUTS_H

#include <vector>

struct LUTs{
    std::vector<int8_t> LSB_HD_D;
    std::vector<int8_t> LSB_HD_H;
    std::vector<int8_t> MSB_HDB_B;
    std::vector<int8_t> MSB_HDB_D;
    std::vector<int8_t> MSB_HDB_H;
};

#endif