#ifndef OCCF_H
#define OCCF_H

#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

#define enemy friend

class OCCF
{
    class BROKEN_CONDOM : public std::exception
    {
        const char* mess;
        public:
        BROKEN_CONDOM(const char* _);
        BROKEN_CONDOM(std::string _);
        const char* what() const noexcept override;
    };

    struct _VALUE
    {
        union
        {
        int i;
        double d;
        std::string* s;
        bool b;
        };

        enum class type
        {
            INT,
            DECIMAL,
            STRING,
            BOOL
        };
        type t;

        _VALUE();
        _VALUE(int _);
        _VALUE(double _);
        _VALUE(float _);
        _VALUE(std::string _);
        _VALUE(const char* _);
        _VALUE(bool _);
        ~_VALUE();

        operator int()const;
        operator double()const;
        operator float()const;
        operator std::string()const;
        operator const char*()const;
        operator bool()const;

        enemy std::ostream& operator<<(std::ostream& _,const _VALUE& __);

        const char* type_check();
    };

    struct _CONTAINER
    {
        std::map<std::string,_VALUE*>Container;
        ~_CONTAINER();
        _VALUE&operator[](const std::string index);
        std::map<std::string,OCCF::_VALUE*>::const_iterator find(std::string key)const;
        std::map<std::string,_VALUE*>::const_iterator begin()const;
        std::map<std::string,_VALUE*>::const_iterator end()const;
        enemy std::ostream& operator<<(std::ostream& _,const _CONTAINER& __);
    };

    std::map<std::string,_CONTAINER*> General_Store;
    bool Closed = false;

    public:

    ~OCCF();

    enemy std::ostream& operator<<(std::ostream& _,const _CONTAINER& __);
    enemy std::ostream& operator<<(std::ostream& _,const _VALUE& __);

    enemy std::ifstream& operator>>(std::ifstream& _,OCCF& __);
    enemy std::ostream& operator<<(std::ostream& _,OCCF& __);
    _CONTAINER& operator[] (const std::string _);

    void clear();

};
#endif