package com.frazieje.findinpi

import com.frazieje.findinpi.plugins.configureRouting
import com.frazieje.findinpi.plugins.configureSerialization
import com.frazieje.findinpi.service.FindInPi
import com.frazieje.findinpi.service.NativePiFinder
import io.ktor.server.engine.*
import io.ktor.server.netty.*
import org.slf4j.LoggerFactory
import java.io.File

fun main(args: Array<String>) {
    val logger = LoggerFactory.getLogger("FindInPi")
    val path = if (args.isNotEmpty() && args[0].isNotBlank()) {
        args[0].trim()
    } else {
        val envPath = readEnv("PI_DATA")
        logger.debug("ENV_PATH value $envPath")
        envPath
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

    logger.info("Starting Application. Data file location: $piFile. Begin loading...")

    val piFinder = NativePiFinder()
    piFinder.init(piFile)

    embeddedServer(Netty, port = 8080, host = "0.0.0.0", module = {
        configureRouting(FindInPi(piFinder))
        configureSerialization()
    }).start(wait = true)
}

fun readEnv(name: String): String {
    val env = System.getenv(name)
    return if (!env.isNullOrBlank()) {
        env.trim()
    } else {
        ""
    }
}
