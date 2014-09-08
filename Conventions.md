# Code formatting
To prevent git conflicts and ugly histories, please always format your code with the eclipse formatter ```ctrl+shift+f```. Unfortunately, unlike for Java projects, eclipse does not provide an automatic formatting at saving a file. Therefore please try to get used to always press a combination of ```ctrl+s``` and ```ctrl+shift+f```!

# File structure
The L1 and L2 specific algorithms should be implemented within the folders ```l1``` and ```l2```. Code that is shared by L1 and L2 should be stored in ```common```. Within all these three folders code related to one detector should always be stored in a detector specific folder as in ```l1/straw/``` unless the code is supposed to be reused by other detectors.

# Code conventions
Here's a rough list of conventions I try to stick to:
 - CamelCase (don't use '_' in Methode/Variable names expect in following point)
 - Instance variables are lower case and end with ```_``` e.g. ```int someVar_;```
 - Class names are capitalized as in ```class SomeClass {```
 - Method names are in lower case e.g. ```void doSomething() {```
