package com.frazieje.findinpi.service

import com.frazieje.findinpi.model.SearchResult
import io.ktor.util.*
import kotlinx.coroutines.*
import kotlinx.coroutines.selects.select
import org.slf4j.LoggerFactory
import java.io.*
import java.util.concurrent.CopyOnWriteArrayList
import kotlin.math.ceil

class FindInPi(
    private val dataFilePath: String,
    private val bufferSizeBytes: Long,
    private val numWorkers: Int
) {
    private val logger = LoggerFactory.getLogger(javaClass)

    private val scope = CoroutineScope(Dispatchers.Default)

    private val ranges by lazy {
        val dataFile = File(dataFilePath)
        val lastChunkSize = dataFile.length() % bufferSizeBytes
        val minChunkSize = (bufferSizeBytes * 0.25).toLong()
        val rawChunkSize = dataFile.length() / bufferSizeBytes.toDouble()
        val totalChunks = rawChunkSize.apply { if (lastChunkSize >= minChunkSize) ceil(this) }.toLong()
        val numJobs = if (totalChunks > numWorkers) numWorkers.toLong() else totalChunks
        val chunkCountPerJob = ceil(totalChunks.toDouble() / numJobs).toLong()
        val ranges = (0 until numJobs).map { num ->
            (num * chunkCountPerJob * bufferSizeBytes)..
            if (num == numJobs - 1) {
                dataFile.length() - 1
            } else {
                ((num + 1) * chunkCountPerJob * bufferSizeBytes) - 1
            }
        }
        logger.debug("datafile length {} numJobs {} totalChunks {} ", dataFile.length(), numJobs, totalChunks)
        ranges.withIndex().forEach { (index, range) ->
            logger.debug("Job $index: processing bytes ${range.first} .. ${range.last}")
        }
        ranges
    }

    suspend fun find(piFinder: PiFinder, searchText: String): SearchResult {
        logger.debug(
            "find called with {} and {}",
            piFinder,
            searchText,
        )
        return if (File(dataFilePath).length() == 0L) {
            SearchResult(false, -1, -1, "No data file")
        } else {
            select {
                ranges.withIndex().map { (index, byteRange) ->
                    scope.async {
                        logger.debug("launched a child search job {}, searching bytes {}", index, byteRange)
                        withContext(Dispatchers.IO) {
                            piFinder.search(dataFilePath, searchText, bufferSizeBytes, byteRange.first, byteRange.last)
                        }
                    }.onAwait {
                        logger.debug("finished child search job {}", index)
                        it
                    }
                }
            }
        }
    }
}