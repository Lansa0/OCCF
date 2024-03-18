#include "OCCF.hpp"

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) OCCF (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

OCCF::~OCCF(){for(auto& pair : General_Store){delete pair.second;}}

std::string trim(const std::string& line)
{
    const char* WhiteSpace = " \t\v\r\n";
    std::size_t start = line.find_first_not_of(WhiteSpace);
    std::size_t end = line.find_last_not_of(WhiteSpace);
    return start == end ? std::string() : line.substr(start, end - start + 1);
}
#define PARSING_ERROR(line_num,line) ("\nERROR : Parsing Failure\nLINE : " + std::to_string(line_num) + " , '" + trim(line) + "'\nWHAT : ")

// Parse OCCF file
std::ifstream& operator>> (std::ifstream& Shopping_List,OCCF& _)
{
    if (_.Closed)
    {throw OCCF::BROKEN_CONDOM("ERROR : Data Override\nWHAT : Clear current data before parsing new file");}

    enum class States
    {
        //Character States
        Standby,
        Container_Start,
        Container_End,
        Key_Start,
        Standby_Value, 
        Value_Start,

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
    };

    States sectionState = States::Container_Search,cachedState;

    bool Comment_Block = false;
    int line_num = 0;

    std::string bankContainer,bankKey,bankValue;
    while (Shopping_List)
    {
        States charState = (Comment_Block) ? States::Comment_Block : States::Standby;
        
        bool isString,isInt=true;

        bool isBool = false,searchBool;
        int indexBool;

        line_num++;
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
                else if (c != ' '){throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "General syntax error"));}
                    break;

                case States::Container_Start:
                    if (c == '>')
                    {
                        if (bankContainer.length() == 0)
                        {throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "Cannot have empty container Names"));}
                        else if (_.General_Store.find(bankContainer) != _.General_Store.end())
                        {throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "Cannot have duplicate container Names"));}

                        sectionState = States::Key_Search;
                        charState = States::Standby;
                    }
                    else
                    {bankContainer += c;}
                    break;

                case States::Key_Start:
                    if (c == '?')
                    {
                        if (bankKey.length() == 0)
                        {throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "Cannot have empty key names"));}
                        else if (_[bankContainer].find(bankKey) != _[bankContainer].end())
                        {throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "Cannot have duplicate key names"));}

                        charState = States::Standby_Value;
                        sectionState = States::Value_Search;
                    }
                    else if (c != '#')
                    {bankKey += c;}
                    else 
                    {throw OCCF::BROKEN_CONDOM("Error : Don't Do That");}
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
                    else if (c == '.')
                    {
                        cachedState = charState;
                        charState = States::Comment_Start;
                    }
                    else if (!(c == ' '|| c == '!' || c == '#'))
                    {throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "Invalid value declaration"));}
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
                    {throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "Invalid value declaration"));}
                    else if (isBool)
                    {
                        std::string check = (searchBool) ? "true" : "false";
                        if (searchBool)
                        {
                            if (check[indexBool] == c){indexBool++;}
                            else{throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "bool 'true' failed to parse (Check Spelling)"));}

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
                            else{throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "bool 'false' failed to parse (Check Spelling)"));}

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
                            if (!(isdigit(c) || c =='.')){throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "Number values cannot contain letters or symbols"));}
                            if (c == '.' && !isInt){throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "Invalid double"));}
                            else if (c == '.'){isInt = false;}
                        }
                        bankValue += c;
                    }
                    break;

                case States::Container_End:
                    if (c == '-')
                    {
                        sectionState = States::Container_Search;
                        charState = States::Standby;

                        bankContainer = "";
                    }
                    else if (c != '-')
                    {throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "Container was not closed correctly"));}
                    else
                    {throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "idk bro"));}
                    break;

                case States::Comment_Start:
                    if (c == '.'){charState = States::Comment_Middle;}
                    else {throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "Comment Line Improperly Declared (...)"));}
                    break;

                case States::Comment_Middle:
                    if (c == '.'){charState = States::Comment_Line;}
                    else if (c =='/')
                    {
                        Comment_Block = true;
                        charState = States::Comment_Block;
                    }
                    else {throw OCCF::BROKEN_CONDOM(std::string(PARSING_ERROR(line_num,line) + "Comment Line Improperly Declared (...)"));}
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
                        charState = States::Standby;
                    }
                    else {charState = States::Comment_Block;}
                    break;

                default:
                    break;
            }
        }
    }

    if (sectionState != States::Container_Search)
    {throw OCCF::BROKEN_CONDOM(std::string("ERROR : Parsing Failure\nWHAT : Container not Closed properly"));}

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
            const char* t = pair.second->type_check();
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

void OCCF::clear()
{
    General_Store.clear();
    Closed = false;
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

inline const char* OCCF::_VALUE::type_check()
{
    switch (t)
    {
    case type::INT: return "#";
    case type::DECIMAL: return "#";
    case type::STRING: return "!";
    case type::BOOL: return "";
    default: throw BROKEN_CONDOM("\nERROR : idk\n");
    }
}

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) SECTION (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

OCCF::_CONTAINER::~_CONTAINER(){for(auto& pair:Container){pair.second = nullptr;}}

OCCF::_VALUE& OCCF::_CONTAINER::operator[](const std::string index)
{
    if (Container.find(index) == Container.end()){Container[index] = new _VALUE;}
    return *Container[index];
}

std::map<std::string,OCCF::_VALUE*>::const_iterator OCCF::_CONTAINER::find(std::string key)const{return Container.find(key);}
std::map<std::string,OCCF::_VALUE*>::const_iterator OCCF::_CONTAINER::begin()const{return Container.begin();}
std::map<std::string,OCCF::_VALUE*>::const_iterator OCCF::_CONTAINER::end()const{return Container.end();}

std::ostream& operator<<(std::ostream& output,const OCCF::_CONTAINER& self)
{
    for(const auto& pair : self.Container)
    {
        const char* affix = pair.second->type_check();
        output << char(63) << pair.first << char(63) << ' ' << affix << *pair.second << affix << char(10);
    }
    return output;
}

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) BROKEN CONDOM (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

OCCF::BROKEN_CONDOM::BROKEN_CONDOM(const char* message) : mess(message){}
OCCF::BROKEN_CONDOM::BROKEN_CONDOM(std::string message) : mess(message.c_str()){}
const char* OCCF::BROKEN_CONDOM::what() const noexcept {return mess;}
