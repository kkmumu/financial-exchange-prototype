/// @file price4.h
/// @brief This is a file to implement a class to store the unscaled price of the financial instrument as an integer with scale 4.
/// @author Shangwen Sun
/// @date 03/08/2022

#pragma once

#include <functional>
#include <string>

#include "types.h"

namespace lib
{

class Price4
{
private:
    t_price unscaled_; // the default scale is 4 eg. 139.96 stored as 1399600
    
public:
    Price4() = default;
    
    explicit Price4(t_price unscaled) : unscaled_(unscaled) {}
    
    /// @brief Convert string stored price to unscaled
    explicit Price4(const std::string& str)
    {
        unscaled_ = std::stold(str) * 10000;
    };
    
    t_price unscaled() const { return unscaled_; }
        
    /// @brief Convert price to string for output
    std::string to_str() const
    {
        return std::to_string(1.0 * unscaled_ / 10000); // # would it be faster if using boost::lexical_cast?
    };
    
    // operator overloading
    /// @brief Overload == operator to check if two prices are equal
    bool operator == (const Price4& price_) const
    {
        return (unscaled_ == price_.unscaled_);
    }
    
    /// @brief Overload != operator to check if two prices are equal
    bool operator != (const Price4& price_) const
    {
        return (unscaled_ != price_.unscaled_);
    }
    
    /// @brief Overload < operator to check if two prices are equal
    bool operator < (const Price4& price_) const
    {
        return (unscaled_ < price_.unscaled_);
    }
    
    /// @brief Overload <= operator to check if two prices are equal
    bool operator <= (const Price4& price_) const
    {
        return (unscaled_ <= price_.unscaled_);
    }
    
    /// @brief Overload > operator check if two prices are equal
    bool operator > (const Price4& price_) const
    {
        return (unscaled_ > price_.unscaled_);
    }
    
    /// @brief Overload >= operator check if two prices are equal
    bool operator >= (const Price4& price_) const
    {
        return (unscaled_ >= price_.unscaled_);
    }
    
};

}
