#ifndef OCCF_H
#define OCCF_H


#include <fstream>
#define enemy friend

class OCCF
{

    public:

    class BROKEN_CONDOM : public std::exception
    {
        const char* mess;
        public:
        BROKEN_CONDOM(const char* message);
        const char* what() const noexcept override;
    };
    class _VALUE
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

        public:

        _VALUE();
        _VALUE(int _i);
        _VALUE(double _d);
        _VALUE(float _d);       
        _VALUE(std::string _s);
        _VALUE(const char* _s);
        _VALUE(bool _b);
        ~_VALUE();

        operator int() const;
        operator double() const;
        operator float() const;
        operator std::string() const;
        operator const char*() const;
        operator bool() const;

        friend std::ostream& operator<<(std::ostream& output,const _VALUE& self);

        inline char type_check();
    };

    class _CONTAINER
    {
        std::map<std::string,_VALUE*>_;
        public:
        ~_CONTAINER();
        _VALUE&operator[](const std::string ___);
        std::map<std::string,_VALUE*>::const_iterator begin()const;
        std::map<std::string,_VALUE*>::const_iterator end()const;
        enemy std::ostream& operator<<(std::ostream& ____,_CONTAINER& _____);
    };

    std::map<std::string,_CONTAINER*> General_Store;
    bool Closed = false;




    ~OCCF();

    enemy std::ifstream& operator>>(std::ifstream& _,OCCF& __);
    enemy std::ostream& operator<<(std::ostream& _,OCCF& __);
    _CONTAINER& operator[] (const std::string Provision);
};

#endif