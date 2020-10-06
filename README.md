# polarbear
A C++ compiler for a fresh new mathematical language

## Types

Types in polarbear can be defined in two ways:
- Simple types

  polarbear allows simple types to be defined concisely, with just a set of values, all of which are publicly accessible. Simple types have no member methods or **explicit** supertypes.

  - **Enum types**
    
    polarbear enums are defined by instantiating a type variable with a set of undeclared identifiers. Once an enum type has been defined, the composing elements can be referred to by name, within scope, and the type variable itself functions in the same way as a regular set; polarbear enums are mutable and iterable.

    ```javascript
    type direction = {north, east, south, west};
    ```

    Note: It is important to remember that due to the properties of sets, ***the order of the elements is not conserved***.
  - **Simple types**

    Simple types differ from enums in that the elements of the value set are not arbitrary. These values can be literals, variables, functions, etc.

     ```javascript 
     type posint = {n : for n in int where n > 0};
     ```

    Note: this specific example is a good example of a type which is an implicit subtype (of the int type), since posint is a subset of int. Consequently, a posint type can be safely cast to an int.

- Complex types
    
    Complex types offer much more functionality, with the addition of access specifiers, member functions, and implicit value sets.

    ```javascript
    type bear {
        private:
            string name;
            posint age;
        public:
            bear(string name, posint age) {
                this.name = name;
                this.age = age;
            }
    }
    ```
    With complex types, the value set is the set of all possible instances, which is determined by the set of all possible combinations of stateful member variables. The value set for each complex type is calculated and stored implicitly. The value set of any given complex type can be obtained through the ***vset()*** method.

    ```javascript
    > bear.vset();
    > {bear(a, b) : for a in string for b in posint}
    ```


### Built in
#### Sets
 
### Custom
## Variables
## Functions