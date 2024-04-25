package com.frazieje.findinpi.service

import com.frazieje.findinpi.model.SearchResult
import org.slf4j.LoggerFactory
import java.io.File
import java.io.FileInputStream
import java.io.IOException
import java.io.Reader
import kotlin.math.min


class KPiFinder : PiFinder {

    private val logger = LoggerFactory.getLogger(javaClass)

    override fun search(
        dataFilePath: String,
        searchText: String,
        bufferSize: Long,
        offset: Long,
        length: Long,
        isActive: () -> Boolean
    ): SearchResult {
        logger.debug("searching $dataFilePath for $searchText. buffersize $bufferSize offset $offset, length $length")
        val time = System.currentTimeMillis()
        return File(dataFilePath).inputStream().apply { skipNBytes(offset) }.bufferedReader().use {
            logger.debug("got stream $dataFilePath for $searchText. buffersize $bufferSize offset $offset, length $length")
            var result: SearchResult? = null
            var offsetCount = 0
            logger.debug("entering loop $dataFilePath for $searchText. buffersize $bufferSize offset $offset, length $length")
            while (isActive() && length - offsetCount >= searchText.length) {
                val buffer = runCatching { readExactly(it, min(length - offsetCount, bufferSize).toInt()) }
                if (buffer.isSuccess) {
                    val loc = buffer.getOrNull()?.indexOf(searchText)
                    val len = buffer.getOrNull()?.length ?: 0
                    if (loc != null && loc >= 0) {
                        result = SearchResult(true, offset + loc.toLong() + offsetCount, -1)
                        break
                    }
                    offsetCount+=len
                }
            }
            result ?: SearchResult(false, -1, -1)
        }.copy(searchTimeMs = System.currentTimeMillis() - time)
    }

    @Throws(IOException::class)
    fun readExactly(reader: Reader, length: Int): String {
        val chars = CharArray(length)
        var readOffset = 0
        while (readOffset < length) {
            val charsRead: Int = reader.read(chars, 0, length - readOffset)
            if (charsRead <= 0) {
                throw IOException("Stream terminated early")
            }
            readOffset += charsRead
        }
        return String(chars)
    }


}