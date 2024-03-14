# OCCF    Overly Complicated Configuration File
Simple c++ configuration file format that supports a ***strictly*** 2 level nesting format. An outter "Section" containing key-value pairs.

## Format
```
-Section>
    ?KEY STRING? !Value!
    ?KEY INT? #1#
    ?KEY DOUBLE? #1.0#
    ?KEY BOOL? true
<-
```