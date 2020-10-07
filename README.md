# polarbear
A C++ compiler for a fresh new mathematical language

## Types

Types in polarbear can be defined in two ways:
- Simple types

  polarbear allows simple types to be defined concisely, with just a set of values, all of which are publicly accessible. Simple types have no member methods or **explicit** supertypes.

  - **Enum types**
    
    polarbear enums are defined by instantiating a type variable with a set of undeclared identifiers. Once an enum type has been defined, the composing elements can be referred to by name, within scope, and the type variable itself functions in the same way as a regular set; polarbear enums are mutable and iterable.

    ```javascript
    type bearfood = {roots, berries, seals, insects};
    type colour = {black, white, red, blue};
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
            string getName() {
                return this.name;
            }
            void setName(string name) {
                this.name = name;
            }
            posint getAge() {
                return this.age;
            }
            void setAge(posint age) {
                this.age = age;
            }
            
    }
    ```
    With complex types, the value set is the set of all possible instances, which is determined by the set of all possible combinations of stateful member variables. The value set for each complex type is calculated and stored implicitly. The value set of any given complex type can be obtained through the ***vset()*** method.

    ```javascript
    > bear.vset();
    > {bear(a,b) : for a in string for b in posint}
    ```

    In contrast to simple types, type inheritance must be **explicit**, and is done like so:

    ```javascript
    type polarbear : bear {
        private:
            set diet = {seals};
            colour furColour = white;
        public:
            polarbear(string name, posint age) {
                this.setName(name);
                this.setAge(age);
            }
            set getDiet() {
                return this.diet;
            }
            colour getFurColour() {
                return this.furColour;
            }
    }
    ```
    Any instance of the polarbear type inherits the member functions of the bear method, since it is a subtype. Moreover, the type definition of polarbear can extend the bear type with more functions and variables. Consequently, since a polarbear is still a bear, the value set of a bear must also be extended.

    ```javascript
    > bear.vset();
    > {
        bear(a,b) : for a in string for b in posint,
        polarbear(a,b) : for a in string for b in posint
      }
    ```


### Built in types

There are few built-in types in polarbear, it is encouraged that users define their own as needed.

 - ``` int ```
 - ``` real ```
 - ``` bool ```
 - ``` char ```
 - ``` string ```
 - ``` set ```
 - ``` array ```


#### Sets

Set-theory forms the basis for most theoretical computational mathematics. Some of the most famous and important algorithms are implemented first and foremost using sets. Sets have three main properties:

 - Elements are unordered.
 - There are no duplicate elements.
 - Elements do not have to be of the same type.

Here are some examples of different ways to define sets in polarbear:

```javascript
set emptySet = {};
set learningAlphabet = {'a', "ab", "abc"};
set rationals = { a/b : for a in int for b in int};
```

As is shown, sets can be intialised by specifying each individual element, or, by using a production and a set of conditions.

Conveniently, there are also a load of built-in set operators in polarbear!

 - Intersection: /\\
 - Union: \\/
 - Difference: -
 - Cartesian product: *

Here are some examples!

```javascript
> {'a', 'b', 'c'} /\ {'b', 'c', 'd'};
> {'b', 'c'}
```

```javascript
> {'a', 'b', 'c'} \/ {'b', 'c', 'd'};
> {'a', 'b', 'c', 'd'}
```

```javascript
> {'a', 'b', 'c'} - {'b', 'c', 'd'};
> {'a'}
```

```javascript
> {'a', 'b'} * {'b', 'c'};
> {('a', 'b'), ('a', 'c'), ('b', 'b'), ('b', 'c')}
```

This makes it easier for us to define even more sets!

```javascript
set irrationals = real - rationals;
```
Note: in polarbear, all types can be used as sets!

Sets can also be iterated over:

```javascript
for (x : posint) {
    out(x);
}
```
This for-loop would output all integers greater than 0.


### Custom
## Variables
## Functions