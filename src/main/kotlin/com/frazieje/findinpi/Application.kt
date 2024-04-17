package com.frazieje.findinpi

import com.frazieje.findinpi.plugins.configureRouting
import com.frazieje.findinpi.plugins.configureSerialization
import io.ktor.server.application.*
import io.ktor.server.engine.*
import io.ktor.server.netty.*
import org.slf4j.LoggerFactory
import java.io.File

fun main(args: Array<String>) {
    val logger = LoggerFactory.getLogger("FindInPi")
    val path = if (args.isNotEmpty() && args[0].isNotBlank()) {
        args[0].trim()
    } else {
        val env = System.getenv("PI_DATA")
        if (!env.isNullOrBlank()) {
            env.trim()
        } else {
            ""
        }
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
    embeddedServer(Netty, port = 8080, host = "0.0.0.0", module = {
        configureRouting(piFile)
        configureSerialization()
    })
        .start(wait = true)
}
