# OCCF - Overly Complicated Configuration File
Simple c++ configuration file format that supports a ***strictly*** 2 level nesting format. An outter "Section" which contains key-value pairs, where each value can contain 1 of 4 datatypes (int, double, string, bool). Compiles with c++11.
***

## Format
```
-Section>
    ?KEY STRING? !Value!
    ?KEY INT? #1#
    ?KEY DOUBLE? #1.0#
    ?KEY BOOL? true
<-
```
#### Sections
To declare a new Section, create an arrow pointing right using the hypen `-` and greater-than `>` symbols with the name of your section in between. To close a section simply create an arrow facing left `<-`. 

> [!Note] 
> - Section names cannot be empty or contain the character `>`. 
> - Duplicate Section names will also cause error when parsing.

#### Keys
To declare a new Key, begin with a `?` followed by the name and close with another `?`. Keys are stored as a String when indexing.

> [!Note] 
> - Key names cannot be empty or contain the character `#`. 
> - Duplicate Key names within the same Section will cause error when parsing.

#### Values
There are 3 ways to declare a new Value. Strings should be prefixed and suffixed with a `!`, ints and doubles with a `#`, and omit any affixes for bools.

> [!Note] 
> - String Values cannot contain the `!` symbol.
> - Each value must begin and end with the same symbol. 
> - An int Value can only hold numbers in the range of an int datatype (+-2,147,483,647)

> [!Important]
> Each Key is required to have a Value pair. Additionally, every Key-Value pair must share the same line. 

#### Comments
Declare Comment Lines with ```...```
```
... Comment Line
```
Declare Comment Blocks with the prefix `../` and end with the suffix `\..`, works multi line.

```
../ Comment Block \..
```

> [!Note]
> - Comment Lines mute the entire line following `...`, attempts to declare anything will be ignored.

***

## Examples
`Source.txt`
```
-Section>
    ?STRING? !Value!
    ?INT? #100#
    ?DOUBLE? #1.1#
    ?BOOL? true
<-
```

### Reading from source file
```c++
#include <iostream>
#include "OCCF.hpp"

int main()
{
    OCCF Data;
        
    std::ifstream file("Source.txt");
    if (!file.is_open())
    {
        std::cout << "File Failed to Open\n";
        return 1;
    }

    // Parse contents of the Source.txt file
    file >> Data;

    // Store the parsed data into variables
    int INT = Data["Section"]["INT"]; 
    double DOUBLE = Data["Section"]["DOUBLE"];
    std::string STRING = Data["Section"]["STRING"]; // const char* works too
    bool BOOL = Data["Section"]["BOOL"];

    // Output data
    std::cout 
    << INT << '\n'    // 100
    << DOUBLE << '\n' // 1.1
    << STRING << '\n' // VALUE
    << BOOL << '\n';  // true : 1
}
```
### Modifying data  
```c++
int main()
{
    OCCF Data;
        
    std::ifstream file("Source.txt");
    if (!file.is_open())
    {
        std::cout << "File Failed to Open\n";
        return 1;
    }

    file >> Data;
    Data["Section"]["INT"] = 200;
    Data["Section"]["DOUBLE"] = 1.2;
    Data["Section"]["STRING"] = "NEW VALUE";
    Data["Section"]["BOOL"] = false;

    int INT =  Data["Section"]["INT"]; 
    double DOUBLE =  Data["Section"]["DOUBLE"];
    std::string STRING =  Data["Section"]["STRING"];
    bool BOOL =  Data["Section"]["BOOL"];

    std::cout 
    << INT << '\n'    // 200
    << DOUBLE << '\n' // 1.1
    << STRING << '\n' // NEW VALUE
    << BOOL << '\n';  // false : 0
}
```
> [!Note]
> Values are not locked to the type they originated with, `Section["INT"] = true` will reassign the key `INT` to value `true`.
### Adding new data
```c++
int main()
{
    OCCF Data;
        
    std::ifstream file("Source.txt");
    if (!file.is_open())
    {
        std::cout << "File Failed to Open\n";
        return 1;
    }

    file >> Data;
    Data["Section"]["NEW STRING"] = "NEW STRING";
    std::string NEW_STRING = Data["Section"]["STRING"];
    
    std::cout << NEW_STRING; 
}
```
### Writing to file
```c++
int main()
{
    OCCF Data;
        
    std::ifstream file("Source.txt");
    if (!file.is_open())
    {
        std::cout << "File Failed to Open\n";
        return 1;
    }

    file >> Data;

    // Create new section
    Data["Section2"]["KEY"] = "VALUE";

    std::ofstream file2("Target.txt");
    if (!file2.is_open())
    {
        std::cout << "File Failed to Open\n";
        return 1;
    }

    // Dump data into target file
    file2 << Data; 
}
```
`Target.txt`
```
-Section>
    ?STRING? !Value!
    ?INT? #100#
    ?DOUBLE? #1.1#
    ?BOOL? true
<-

-Section2>
    ?KEY? !VALUE!
<-
```
