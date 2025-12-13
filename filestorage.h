#pragma once
#include <contacts.h>
#include <string>
#include <vector>

class filestorage{
    std::string filename;
public:
    explicit filestorage(std::string fn);// предотвращает неявное преобразования одного типа в другой
    std::vector <contacts> loadall();
    void saveall(const std::vector<contacts>& data);// типо прочитай все файлы, а потом типо отдай в виде списка
};
