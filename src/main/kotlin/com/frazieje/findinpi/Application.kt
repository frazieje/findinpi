package com.frazieje.findinpi

import com.frazieje.findinpi.plugins.configureRouting
import com.frazieje.findinpi.plugins.configureSerialization
import com.frazieje.findinpi.service.KPiFinder
import com.frazieje.findinpi.service.NativePiFinder
import com.frazieje.findinpi.service.PiFinder
import io.ktor.server.application.*
import io.ktor.server.engine.*
import io.ktor.server.netty.*
import org.slf4j.LoggerFactory
import java.io.File
import kotlin.reflect.KClass

fun main(args: Array<String>) {
    val logger = LoggerFactory.getLogger("FindInPi")
    val path = if (args.isNotEmpty() && args[0].isNotBlank()) {
        args[0].trim()
    } else {
        readEnv("PI_DATA")
    }

    val piFile = try {
        val file = File(path)
        file.reader().use { reader -> reader.read() }
        file.absolutePath
    } catch (e: Exception) {
        logger.warn("Could not read data file, using builtin 1M pi data")
        try {
            Thread.currentThread().contextClassLoader.getResource("Pi1M.txt")!!.file
        } catch (e2: Exception) {
            throw RuntimeException("Could not find pi data or read built-in pi data file")
        }
    }

    val finder = try {
        PiFinderType.valueOf(readEnv("PI_FINDER").uppercase())
    } catch (e: Exception) {
        PiFinderType.KOTLIN
    }

    logger.info("Starting Application. Data file location: $piFile. Finder: $finder")
    embeddedServer(Netty, port = 8080, host = "0.0.0.0", module = {
        configureRouting(piFile, finder)
        configureSerialization()
    })
        .start(wait = true)
}

enum class PiFinderType(val finderClass: KClass<out PiFinder>) {
    KOTLIN(KPiFinder::class),
    NATIVE(NativePiFinder::class)
}

fun readEnv(name: String): String {
    val env = System.getenv(name)
    return if (!env.isNullOrBlank()) {
        env.trim()
    } else {
        ""
    }
}
