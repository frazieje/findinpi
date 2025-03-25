val ktor_version: String by project
val kotlin_version: String by project
val logback_version: String by project

plugins {
    kotlin("jvm") version "1.9.23"
    id("io.ktor.plugin") version "2.3.9"
}

group = "com.frazieje"

application {
    mainClass.set("com.frazieje.findinpi.ApplicationKt")

    val isDevelopment: Boolean = project.ext.has("development")
    applicationDefaultJvmArgs = listOf("-Dio.ktor.development=$isDevelopment")
}

val nativeBuildDir = project.layout.buildDirectory.dir("native").get().asFile
val nativeSourceDir = project.layout.projectDirectory.dir("src/main/jni").asFile

val externalSourceDir = project.layout.projectDirectory.dir("external/femto").asFile

distributions {
    main {
        contents {
            from(File(nativeBuildDir, "libs"))
        }
    }
}

val generateExternalBuildSystemTask = tasks.create<Exec>("generateExternalBuildSystem") {
    dependsOn(tasks.getByName("compileJava"))
    workingDir = externalSourceDir
    commandLine = listOf("sh", "autogen.sh")
}

val configureExternalBuildSystemTask = tasks.create<Exec>("configureExternalBuildSystem") {
    dependsOn(generateExternalBuildSystemTask)
    workingDir = externalSourceDir
    commandLine = listOf("./configure")
}

val buildExternalTask = tasks.create<Exec>("buildExternal") {
    dependsOn(configureExternalBuildSystemTask)
    workingDir = externalSourceDir
    commandLine = listOf("make", "-j${Runtime.getRuntime().availableProcessors()}")
}

val copyExternalIncludes = tasks.create<Copy>("copyExternalIncludes") {
    dependsOn(buildExternalTask)
    from(file(File(externalSourceDir, "src/main/femto.h")))
    into(File(nativeBuildDir, "include"))
}

val copyExternalLibsTask = tasks.create<Copy>("copyExternalLibs") {
    dependsOn(copyExternalIncludes)
    from(
        fileTree(File(externalSourceDir, "src/main/.libs")).filter { it.extension == "a" },
        fileTree(File(externalSourceDir, "src/utils/.libs")).filter { it.extension == "a" }
    )
    into(File(nativeBuildDir, "libs"))
}

val generateNativeBuildSystemTask = tasks.create<Exec>("generateNativeBuildSystem") {
    dependsOn(tasks.getByName("compileJava"))
//    val platform = if (OperatingSystem.current().isLinux) {
//        "linux"
//    } else if (OperatingSystem.current().isMacOsX) {
//        "darwin"
//    } else {
//        "windows"
//    }
    workingDir = nativeSourceDir
    commandLine = listOf(
        "cmake",
        "-S",
        ".",
        "-B",
        nativeBuildDir.absolutePath,
        "-DNATIVE_BUILD_DIR=${nativeBuildDir.absolutePath}",
        "-DNATIVE_LIBS_DIR=${nativeBuildDir.absolutePath}/libs",
        "-DNATIVE_INCLUDE_DIR=${nativeBuildDir.absolutePath}/include"
    )
}

val buildNativeTask = tasks.create<Exec>("buildNative") {
    dependsOn(copyExternalLibsTask, generateNativeBuildSystemTask)
    workingDir = nativeSourceDir
    commandLine = listOf("cmake", "--build", nativeBuildDir.absolutePath, "--target", "all")
}

tasks.getByName("classes").dependsOn(buildNativeTask)

tasks.withType<JavaCompile> {
    val compilerArgs = options.compilerArgs
    compilerArgs.add("-h")
    compilerArgs.add(nativeBuildDir.absolutePath)
}

repositories {
    mavenCentral()
}

dependencies {
    implementation("io.ktor:ktor-server-core-jvm")
    implementation("io.ktor:ktor-server-netty-jvm")
    implementation("io.ktor:ktor-server-content-negotiation-jvm")
    implementation("io.ktor:ktor-serialization-gson-jvm")
    implementation("ch.qos.logback:logback-classic:$logback_version")
    testImplementation("io.ktor:ktor-server-tests-jvm")
    testImplementation("org.jetbrains.kotlin:kotlin-test-junit:$kotlin_version")
}
