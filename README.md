# OCCF - Overly Complicated Configuration File
Simple c++ configuration file format that supports a ***strictly*** 2 level nesting format. An outter "Section" which contains key-value pairs, where each value can contain 1 of 4 datatypes (int, double, string, bool).

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
> - Every Key needs to have a Value pair.

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
> - Comment Lines mute the entire line, cannot declare anything within it.
> - Comment Blocks cannot interupt the flow of a key-value pair i.e. ```?Key? ../ Comment \.. !Value!```





