//
//  parser.cpp
//  financial_exchange_prototype
//
//  Created by Sun Shangwen on 4/8/22.
//

#include "parser.h"

using namespace lob;



std::vector<lib::t_order> OrderParser::load(const lib::FILE& file_name, lib::TickSizeRule& tick_size_rule_, lib::t_lot lot_size_)
{
    
    std::string line;
    nlohmann::json j;
    
    std::ifstream input_file(file_name, std::ifstream::in);
    std::vector<lib::t_order> orders;
    int cnt = 0; // count line numbers
    
    if (!input_file.is_open())
    {
        std::cout << "FAILED TO OPEN " << file_name << ".\n";
        throw std::exception();
    }
    
    // if comes across error (invalid order input or parsing errors)
    // print error information and skip that line/object
    while (std::getline(input_file, line))
    {
        cnt += 1;
        try
        {
            j = nlohmann::json::parse(line);
            
        }
        catch(const std::exception& err)
        {
            std::cout << "LINE " << cnt << " : PARSING ERROR..." << err.what() << std::endl;
        }
        
        try
        {
            lib::t_order order(j, tick_size_rule_, lot_size_); // parse the order and checks if the input arguments are valid
            orders.emplace_back(order);
        }
        catch(const std::exception& err)
        {
            std::cout << "LINE " << cnt << " : INVALID ORDER INPUT..." << err.what() << std::endl;
        }
    }
    return orders;
}

