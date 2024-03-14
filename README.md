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
To initialize a new Section, create an arrow pointing right using the hypen ```-``` and greater-than ```>``` symbols with the name of your section in between. To close a section simply create an arrow facing left ```<-```. 

***Note**** _Section names cannot be empty or contain the character ```>```. Duplicate Section names will also cause error when parsing._

#### Keys
To initialize a new Key, begin with a ```?``` followed by the name and close with another ```?```. Keys are stored as a String when indexing.

***Note**** _Key names cannot be empty or contain the character ```#```. Duplicate Key names within the same Section will cause error._ 

#### Values


