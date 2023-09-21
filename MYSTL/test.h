//
// Created by 78172 on 2023/4/7.
//

#ifndef MEMORY_MANAGE_TEST_H
#define MEMORY_MANAGE_TEST_H

#include<string>
#include<iostream>

template<typename OutputIterator>
void MYSTL_print_range(const std::string& prompt, OutputIterator first,
                                                  OutputIterator last )
{
    std::cout << prompt << " ";
    while(first != last)
    {
        std::cout << *first << " ";
        ++first;
    }
    std::cout << std::endl;
}


#endif //MEMORY_MANAGE_TEST_H
