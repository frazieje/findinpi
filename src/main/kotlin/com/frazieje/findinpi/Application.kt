package com.frazieje.findinpi

import com.frazieje.findinpi.plugins.configureRouting
import com.frazieje.findinpi.plugins.configureSerialization
import com.frazieje.findinpi.service.FindInPi
import com.frazieje.findinpi.service.NativePiFinder
import com.frazieje.findinpi.service.PiFinder
import io.ktor.server.engine.*
import io.ktor.server.netty.*
import org.slf4j.LoggerFactory
import java.io.File

fun main(args: Array<String>) {
    val logger = LoggerFactory.getLogger("FindInPi")

    fun getArg(argNum: Int, envVar: String) = if (args.isNotEmpty() && args[0].isNotBlank()) {
        args[argNum].trim()
    } else {
        val envPath = readEnv(envVar)
        logger.debug("$envVar value $envPath")
        envPath
    }

    val dataFilePath = getArg(0, "PI_DATA")

    val suffixArrayPath = getArg(1, "SUFFIX_ARRAY")

    val fmIndexPath = getArg(2, "FM_INDEX")

    val fullDataFilePath = getArg(3, "FULL_PI_DATA")

    fun checkFile(path: String, backup: (() -> String)? = null) = try {
        val file = File(path)
        file.reader().use { reader -> reader.read() }
        file.absolutePath
    } catch (e: Exception) {
        logger.warn("Could not read file $path")
        backup?.invoke()
    }!!

    val dataFile = checkFile(dataFilePath) {
        try {
            Thread.currentThread().contextClassLoader.getResource("Pi1M.txt")!!.file
        } catch (e2: Exception) {
            throw RuntimeException("Could not find pi data or read built-in pi data file")
        }
    }

    val suffixArrayFile = checkFile(suffixArrayPath)
    val fmIndexFile = checkFile(fmIndexPath)
    val fullDataFile = checkFile(fullDataFilePath)

    logger.info("Starting Application. Data file location: $dataFile, suffix array file: $suffixArrayFile, fm-index file: $fmIndexFile. Full data file: $fullDataFile. Begin loading...")

    val piFinder: PiFinder = NativePiFinder()
    piFinder.init(dataFilePath, suffixArrayFile, fmIndexFile, fullDataFile)

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
