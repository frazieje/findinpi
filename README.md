# FindInPi
A Kotlin HTTP API to searching pi for specific strings of numbers. Includes a pure kotlin and C/JNI/kotlin impelemntation.

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
You download some 1B+ digit calculations of pi from [here](https://stuff.mit.edu/afs/sipb/contrib/pi/), or you can calculate your own pi data file with [y-cruncher](http://www.numberworld.org/y-cruncher/), or you

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