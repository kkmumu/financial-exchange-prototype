#include <stdexcept>

#include "engine.h"

void MatchingEngine::config(const std::string &config_file)
{
    // config tick size rule
    std::ifstream input_file(config_file, std::ifstream::in);
    nlohmann::json json_tick;

    try
    {
        input_file >> json_tick;
    }
    catch(const std::exception& err)
    {
        std::cout << "FAILED TO OPEN " << config_file << ".\n";
    }
    tick_size_rule_.FromJson(json_tick);
    
    // config lot size
    
}

std::vector<t_order> MatchingEngine::orderParser(const std::string &file_name)
{
    
    std::string line;
    nlohmann::json j;
    
    std::ifstream input_file(file_name, std::ifstream::in);
    std::vector<t_order> orders;
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
        
        t_order order;
        try
        {
            order.init(j); // parse the order and checks if the input arguments are valid
        }
        catch(const std::exception& err)
        {
            std::cout << "LINE " << cnt << " : INVALID ORDER INPUT..." << err.what() << std::endl;
        }
        orders.emplace_back(order);
    }
    
    return orders;
}
