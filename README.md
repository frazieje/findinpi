# FindInPi
A Kotlin HTTP API for searching Pi for specific series of numbers. Includes both pure Kotlin and C/JNI/Kotlin impelemntations.

## Why?
Pi is the world's most famous irrational number. As an irrational number, Pi is a never ending, never permanently repeating, seemingly random stream of numbers when represented as a decimal.

Pi has long been a popular reference number by which to calculate numbers to many digits using computers, with the world record at the time of this writing being over 100 trillion digits.

A random number this long contains many smaller number series, such as your social securty number, phone number, birthday, etc. These numbers occur by chance, but it can still be interesting to see exactly where in Pi some of these numbers might occur.

Let's use phone numbers as an example. US phone numbers are 10 digits long, so there are 10<sup>10</sup> possible phone numbers. If we were to randomly generate 10<sup>10</sup> 10-digit numbers, the resulting set should contain nearly all possible values. For example, in any random string of digits, if we want to find any decimal number of length n, our string of digits must be 10<sup>n</sup> * n digits long. So in our example, we would need 10<sup>10</sup> * 10 digits of Pi, or 100 billion digits.

This program was written for Pi day 2024 and can be used to search large Pi data files for any string of digits. 
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
Before running, you'll need to provide a Pi data file. The file should be a **SINGLE LINE** text file containg the
digits of pi. 

NOTE: The [file](src/main/resources/Pi1M.txt) provided with this repository is only 1M digits long, which isn't very 
interesting to search. 

#### Getting a data file
To get Pi data, you can do the following: 
- Download 1B+ digit calculations of Pi from [here](https://stuff.mit.edu/afs/sipb/contrib/pi/)
- Calculate your own Pi data file with [y-cruncher](http://www.numberworld.org/y-cruncher/)
- Perform the following steps:
  
1. Set the location of the file as an environment variable:
```shell
$ export PI_DATA=[location of data file]
```

2. Run the program:
```shell
$ ./gradlew run
```
Optional: Set the location of the data file as a program argument:
```shell
$ ./gradlew run -PpiData=[location of data file]
```

### Usage
After you run the program, use the HTTP API:
```shell
$ curl --request POST \
     --header 'Content-Type: application/json' \
     --url 'http://localhost:8080/search' \
     --data '{"searchText":"62643"}'
```
