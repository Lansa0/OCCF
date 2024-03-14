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
To initialize a new Section, create an arrow pointing right using the hypen ```-``` and greater-than ```>``` symbols with the name of your section in between. 