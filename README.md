# FindInPi
A Kotlin HTTP API to searching pi for specific strings of numbers. Includes a pure kotlin and C/JNI/kotlin impelemntation.

## Why?
Pi is probably the most famous irrational number. As an irrational number, when pi is represented as a decimal it is a never ending, never permanently repeating, seemingly random stream of numbers.

Pi has long been popular to calculate to many digits using computers, with the world record at the time of this writing being over 100 trillion digits.

A random number number this long contains many other smaller numbers, such as your social securty number, phone number, birthday, etc. These numbers occur by chance, but it can still be interesting to see exactly where in pi some of these numbers might occur.

Let's use phone numbers as an example. US phone numbers are 10 digits long, so there are 10<sup>10</sup> possible phone numbers. If we were to randomly generate 10<sup>10</sup> 10-digit numbers, the resulting set should contain nearly all possible values. Following this we can see that in any random string of digits, if we want to find any decimal number of length n, our string of digits must be 10<sup>n</sup> * n digits long. So in our example we'd need 10<sup>10</sup> * 10 digits of pi, or 100 billion digits.

This program was written for pi day 2024, and it can be used to search large pi data files for any string of digits. 
## Building
### Prerequisites
- Java 11+
- CMake 3.x+

You can build the project with gradle:
```shell
$ ./gradlew build
```

To build only the native library:
```shell
./gradlew nativeBuild
```

## Running
### Prerequisites
Before running, you'll need to provide a pi data file. The file should be a **SINGLE LINE** text file containg the
digits of pi. 

The [file](src/main/resources/Pi1M.txt) provided with this repository is only 1M digits long, which isn't very 
interesting to search. 

#### Getting a data file
You download some 1B+ digit calculations of pi from [here](https://stuff.mit.edu/afs/sipb/contrib/pi/), 
or you can calculate your own pi data file with [y-cruncher](http://www.numberworld.org/y-cruncher/), or you

Set the location of the file as an environment variable:
```shell
$ export PI_DATA=[location of data file]
```

Run the program:
```shell
$ ./gradlew run
```
You can also set the location of the data file as a program argument:
```shell
$ ./gradlew run -PpiData=[location of data file]
```

### Usage
To use the HTTP API once running:
```shell
$ curl --request POST \
     --header 'Content-Type: application/json' \
     --url 'http://localhost:8080/search' \
     --data '{"searchText":"62643"}'
```