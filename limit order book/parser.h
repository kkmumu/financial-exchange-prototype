/// @file parser.h
/// @brief This is a file to parse an request file consists of JSON orders.
/// @author Shangwen Sun
/// @date 04/08/2022

#pragma once

#include <vector>

#include "types.h"
#include "ticks.h"
#include "order.h"

namespace lob
{

class OrderParser
{
public:
    OrderParser() = default;
    std::vector<lib::t_order> load(const lib::FILE& file_name, lib::TickSizeRule& tick_size_rule_, lib::t_lot lot_size_);
    
    ~OrderParser() = default;
};

} // namespace lob

