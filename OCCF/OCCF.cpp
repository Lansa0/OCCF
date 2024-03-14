#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include "OCCF.h"

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) OCCF (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

OCCF::~OCCF(){for(auto& pair : General_Store){delete pair.second;}}

// Parse OCCF file 
std::ifstream& operator>> (std::ifstream& Shopping_List,OCCF& _)
{
    if (_.Closed)
    {throw OCCF::BROKEN_CONDOM("Error : Clear current data before parsing new file");}

    enum class States 
    {
        //Character States
        Standby,
        Container_Start,
        Container_End,
        Key_Start,
        Standby_Value, 
        Value_Start,

        Force_End,

        Comment_Start,
        Comment_Middle,
        Comment_Line,

        Comment_Block,
        Comment_Block_Middle,
        Comment_Block_End,


        //Section States
        Container_Search,
        Key_Search,
        Value_Search,

        //Value States
        INT,
        FLOAT,
        STRING
    };

    States sectionState = States::Container_Search,cachedState;
    std::vector<std::string> ContainerList,keyList;

    bool Comment_Block = false;

    std::string bankContainer,bankKey,bankValue;
    while (Shopping_List)
    {
        States charState = (Comment_Block) ? States::Comment_Block : States::Standby;
        
        bool isString,isInt=true;

        bool isBool = false,searchBool;
        int indexBool;

        //bool numberedKeys = false;
        int keyNumber;

        std::string line;
        getline(Shopping_List,line);
        for (const char& c : line)
        {
            switch (charState)
            {
                case States::Standby:
                     if (c == '.'){charState = States::Comment_Start;}
                else if (c == '-' && sectionState == States::Container_Search){charState = States::Container_Start;}
                else if (c == '?' && sectionState == States::Key_Search){charState = States::Key_Start;}
                else if (c == '<' && sectionState == States::Key_Search){charState = States::Container_End;}
                else if (c != ' '){throw OCCF::BROKEN_CONDOM("Error : Parsing Failure");}
                    break;

                case States::Container_Start:
                    if (c == '>')
                    {
                        if (bankContainer.length() == 0)
                        {throw OCCF::BROKEN_CONDOM("Error : Cannot have empty Container Names");}
                        else if (std::find(ContainerList.begin(),ContainerList.end(),bankContainer) != ContainerList.end())
                        {throw OCCF::BROKEN_CONDOM("Error : Cannot have matching Container Names");}

                        sectionState = States::Key_Search;
                        charState = States::Standby;

                        ContainerList.push_back(bankContainer);
                        keyNumber = 1;
                    }
                    else
                    {bankContainer += c;}
                    break;

                case States::Key_Start:
                    if (c == '?')
                    {
                        if (bankKey.length() == 0)
                        {throw OCCF::BROKEN_CONDOM("Error : Cannot have Dmpty Key Names");}
                        else if (std::find(keyList.begin(),keyList.end(),bankKey) != keyList.end())
                        {throw OCCF::BROKEN_CONDOM("Error : Cannot have Duplicate Key Names");}

                        charState = States::Standby_Value;
                        sectionState = States::Value_Search;

                        keyList.push_back(bankKey);
                    }
                    else if (c == '#')
                    {
                        if (bankKey.length() != 0)
                        {throw OCCF::BROKEN_CONDOM("Error : WIPWIPWIPWIP");}

                        //numberedKeys = true;
                        std::string i = std::to_string(keyNumber);

                        bankKey += i;
                        keyNumber++;
                    }
                    else if (c != '#')
                    {bankKey += c;}
                    else 
                    {throw OCCF::BROKEN_CONDOM("Error : WIPWIPWIPWIPWIP");}
                    break;

                case States::Standby_Value:
                    if ((c == '!' || c == '#') && sectionState == States::Value_Search)
                    {
                        isString = (c == '#') ? false : true;
                        charState = States::Value_Start;
                    }
                    else if (c == 't' || c == 'f')
                    {
                        searchBool = (c == 't') ? true : false;
                        isBool = true;
                        indexBool = 1;
                        charState = States::Value_Start;
                    }
                    else if (!(c == ' '|| c == '!' || c == '#'))
                    {throw OCCF::BROKEN_CONDOM("Error : Invalid Value Declaration");}
                    break;

                case States::Value_Start:
                    if ((c == '!' && isString) || (c == '#' && !isString))
                    {
                        if (isString){_[bankContainer][bankKey] = bankValue;}
                        else if (isInt){_[bankContainer][bankKey] = int(std::stoi(bankValue));}
                        else {_[bankContainer][bankKey] = double(std::stod(bankValue));}

                        bankValue = "";
                        bankKey = "";
                        charState = States::Standby;
                        sectionState = States::Key_Search;

                        isInt = true;
                    }
                    else if ((c == '!' && !isString) || (c == '#' && isString))
                    {throw OCCF::BROKEN_CONDOM("Error : Value improperly declared");}
                    else if (isBool)
                    {
                        std::string check = (searchBool) ? "true" : "false";
                        if (searchBool)
                        {
                            if (check[indexBool] == c){indexBool++;}
                            else{throw OCCF::BROKEN_CONDOM("Error : bool 'true' failed to parse (Check Spelling)");}

                            if (indexBool == 4)
                            {
                                _[bankContainer][bankKey] = true;
                                charState = States::Standby;
                                sectionState = States::Key_Search;
                                isBool = false;
                                bankKey = "";
                            }
                        }
                        else if (!searchBool)
                        {
                            if (check[indexBool] == c){indexBool++;}
                            else{throw OCCF::BROKEN_CONDOM("Error : bool 'false' failed to parse (Check Spelling)");}

                            if (indexBool == 5)
                            {
                                _[bankContainer][bankKey] = false;
                                charState = States::Standby;
                                sectionState = States::Key_Search;
                                isBool = false;
                                bankKey = "";
                            }
                        }
                    }
                    else
                    {
                        if (!isString)
                        {
                            if (!(isdigit(c) || c =='.')){throw OCCF::BROKEN_CONDOM("Error : Number values cannot contain letters or symbols");}
                            if (c == '.' && !isInt){throw OCCF::BROKEN_CONDOM("Error : Invalid double");}
                            else if (c == '.'){isInt = false;}
                        }
                        bankValue += c;
                    }
                    break;

                case States::Container_End:
                    if (c == '-')
                    {

                        keyList.clear();

                        sectionState = States::Container_Search;
                        charState = States::Standby;

                        bankContainer = "";

                    }
                    else if (c != '-')
                    {throw OCCF::BROKEN_CONDOM("Error : Container was not closed correctly");}
                    else
                    {throw OCCF::BROKEN_CONDOM("Error : idk bro");}
                    break;

                case States::Comment_Start:
                    if (c == '.'){charState = States::Comment_Middle;}
                    else {throw OCCF::BROKEN_CONDOM("Error : Comment Line Improperly Declared (...)");}
                    break;

                case States::Comment_Middle:
                    if (c == '.'){charState = States::Comment_Line;}
                    else if (c =='/')
                    {
                        Comment_Block = true;
                        cachedState = charState;
                        charState = States::Comment_Block;
                    }
                    else {throw OCCF::BROKEN_CONDOM("Error : Comment Line Improperly Declared (...)");}
                    break;

                case States::Comment_Line:
                    break;

                case States::Comment_Block:
                    if (c == '\\'){charState = States::Comment_Block_Middle;}
                    break;

                case States::Comment_Block_Middle:
                    if (c == '.'){charState = States::Comment_Block_End;}
                    else {charState = States::Comment_Block;}
                    break;

                case States::Comment_Block_End:
                    if (c == '.')
                    {
                        Comment_Block = false;
                        charState = cachedState;
                    }
                    else {charState = States::Comment_Block;}
                    break;

                default:
                    break;
            }
        }
    }

    if (sectionState != States::Container_Search)
    {throw OCCF::BROKEN_CONDOM("Error : Container not Closed properly");}

    ContainerList.clear();
    _.Closed = true;
    return Shopping_List;
};

// Output Full Data in OCCF Format
std::ostream& operator<<(std::ostream& Gossip,OCCF& _)
{
    for(const auto& Inventory : _.General_Store)
    {
        Gossip << '-' << Inventory.first << ">\n"; 
        for(const auto& pair : *Inventory.second)
        {
            char t = pair.second->type_check();
            Gossip << "\t?" << pair.first << "? " << t << *pair.second << t << "\n";
        }
        Gossip << "<-\n\n";
    }
    return Gossip;
}

OCCF::_CONTAINER& OCCF::operator[] (const std::string Provision)
{
    if (General_Store.find(Provision) == General_Store.end()) 
    {General_Store[Provision] = new OCCF::_CONTAINER;}
    return *General_Store[Provision];
}

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) VALUE (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

OCCF::_VALUE::_VALUE(){}
OCCF::_VALUE::~_VALUE(){if (t == type::STRING){s = nullptr;}}

OCCF::_VALUE::_VALUE(int _i) : i(_i),t(type::INT){}
OCCF::_VALUE::_VALUE(double _d) : d(_d),t(type::DECIMAL){}
OCCF::_VALUE::_VALUE(float _d) : d(_d),t(type::DECIMAL){}
OCCF::_VALUE::_VALUE(std::string _s) : s(new std::string(_s)), t(type::STRING){} // CPP String Class
OCCF::_VALUE::_VALUE(const char* _s) : s(new std::string(_s)), t(type::STRING){} // C-Style String 
OCCF::_VALUE::_VALUE(bool _b) : b(_b), t(type::BOOL){}

OCCF::_VALUE::operator int() const
{
    switch (t)
    {
        case type::INT: return i;
        case type::DECIMAL: return int(d);
        case type::STRING:
            if ((s->find_first_not_of("0123456789")) == std::string::npos)
            {return std::stoi(*s);}
            else
            {return int(s->size());}
        case type::BOOL: return int(b);
        default: throw BROKEN_CONDOM("Error : Unable to convert to type int");
    }
}
OCCF::_VALUE::operator double() const
{
    switch (t)
    {
        case type::INT : return double(i);
        case type::DECIMAL: return double(d);
        case type::STRING: return double(s->size());// REDO THIS
        case type::BOOL: return double(b);
        default: throw BROKEN_CONDOM("Error : Unable to convert type to double");
    }
}
OCCF::_VALUE::operator float() const
{
    switch (t)
    {
        case type::INT : return float(i);
        case type::DECIMAL: return float(d);
        case type::STRING: return float(s->size());
        case type::BOOL: return float(b);
        default: throw BROKEN_CONDOM("Error : Unable to convert type to float");
    }
}
OCCF::_VALUE::operator std::string() const
{
    switch (t)
    {
        case type::INT: return std::to_string(i);
        case type::DECIMAL: return std::to_string(d);
        case type::STRING: return *s;
        case type::BOOL: return b ? "true" : "false";
        default: throw BROKEN_CONDOM("Error : Unable to convert to type std::string");
    }
}
OCCF::_VALUE::operator const char*() const
{
    switch (t)
    {
        case type::INT: return _VALUE(std::to_string(i).c_str());
        case type::DECIMAL: return _VALUE(std::to_string(d).c_str());
        case type::STRING: return s->c_str();
        case type::BOOL: return b ? "true" : "false";
        default: throw BROKEN_CONDOM("Error : Unable to convert to type const char*");
    }
}
OCCF::_VALUE::operator bool() const
{
    std::string cache;
    switch (t)
    {
        case type::INT: return i;
        case type::DECIMAL: return d;
        case type::STRING:
            if (s->size() > 5){throw BROKEN_CONDOM("Error : Unable to convert string to type bool");}
            for (const char& c : (*s))
            {cache += c;}
            if (cache == "true"){return true;}
            else if (cache == "false"){return false;}
            throw BROKEN_CONDOM("Error : Unable to convert string to type bool");
        case type::BOOL: return b;
        default: throw BROKEN_CONDOM("Error : Unable to convert to type bool");
    }
}

std::ostream& operator<<(std::ostream& output,const OCCF::_VALUE& self)
{
    switch (self.t)
    {
    case OCCF::_VALUE::type::INT: return output << self.i;
    case OCCF::_VALUE::type::DECIMAL: return output << self.d;
    case OCCF::_VALUE::type::STRING: return output << *self.s;
    case OCCF::_VALUE::type::BOOL: return (self.b) ? output << "true" : output << "false" ;
    default: return output;
    }
}

inline char OCCF::_VALUE::type_check()
{
    switch (t)
    {
    case type::INT: return '#';
    case type::DECIMAL: return '#';
    case type::STRING: return '!';
    case type::BOOL: return ' ';
    default: throw BROKEN_CONDOM("Error : idk");
    }
}

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) SECTION (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

OCCF::_CONTAINER::~_CONTAINER(){for(auto&__:_){delete __.second;}}

OCCF::_VALUE& OCCF::_CONTAINER::operator[](const std::string ___)
{
    if (_.find(___) == _.end()){_[___] = new _VALUE;}
    return *_[___];
}

std::map<std::string,OCCF::_VALUE*>::const_iterator OCCF::_CONTAINER::begin()const{return _.begin();}
std::map<std::string,OCCF::_VALUE*>::const_iterator OCCF::_CONTAINER::end()const{return _.end();}

std::ostream& operator<<(std::ostream& ____,const OCCF::_CONTAINER& _____)
{
    for(const auto& ______ : _____._)
    {
        char _______ = ______.second->type_check();
        ____ << char(63) << ______.first << char(63) << ' ' << _______ << *______.second << _______ << char(10);
    }
    return ____;
}

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) BROKEN CONDOM (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

OCCF::BROKEN_CONDOM::BROKEN_CONDOM(const char* message) : mess(message){}
const char* OCCF::BROKEN_CONDOM::what() const noexcept {return mess;}




// class OCCF
// {






//     public:

//     enemy std::ifstream& operator>>(std::ifstream& _,OCCF& __);
//     enemy std::ostream& operator<<(std::ostream& _,OCCF& __);

//     _CONTAINER& operator[] (const std::string Provision)
//     {
//         if (General_Store.find(Provision) == General_Store.end()) 
//         {General_Store[Provision] = new _CONTAINER;}
//         return *General_Store[Provision];
//     }
// };

// // Parse File


// // Output Full Data in OCCF Format
// std::ostream& operator<<(std::ostream& Gossip,OCCF& _)
// {
//     for(const auto& Inventory : _.General_Store)
//     {
//         Gossip << '-' << Inventory.first << ">\n"; 
//         for(const auto& pair : *Inventory.second)
//         {
//             char t = pair.second->type_check();
//             Gossip << "\t?" << pair.first << "? " << t << *pair.second << t << "\n";
//         }
//         Gossip << "<-\n\n";
//     }
//     return Gossip;
// }

