//
// Created by 78172 on 2023/3/11.
//

#include<iostream>
#include<memory>

namespace MySTL
{
    struct file_closer
    {
        void operator()(FILE* file) const
        {
            fclose(file);
        }
    };

    using cfile = std::unique_ptr<FILE, file_closer>;

    cfile make_cfile(const char* filename, const char* mode)
    {
        FILE* stream = fopen(filename, mode);
        if(not stream)
            throw std::runtime_error("Filed to open file");
        return cfile(stream);
    }
}
