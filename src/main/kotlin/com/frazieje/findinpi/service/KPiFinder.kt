package com.frazieje.findinpi.service

import com.frazieje.findinpi.model.SearchResult
import java.io.File
import java.io.IOException
import java.io.Reader


class KPiFinder : PiFinder {

    override fun search(dataFilePath: String, searchText: String, bufferSize: Int): SearchResult {
        val time = System.currentTimeMillis()
        return File(dataFilePath).inputStream().bufferedReader().use {
            var result: SearchResult? = null
            var offsetCount = 0
            while (result == null) {
                val buffer = runCatching { readExactly(it, bufferSize) }
                if (buffer.isSuccess) {
                    val loc = buffer.getOrNull()?.indexOf(searchText)
                    if (loc != null && loc >= 0) {
                        result = SearchResult(true, loc.toLong() + offsetCount, -1)
                    } else {
                        offsetCount+=bufferSize
                    }
                } else {
                    result = SearchResult(false, -1, -1)
                }
            }
            result
        }.copy(timeMs = System.currentTimeMillis() - time)
    }

    @Throws(IOException::class)
    fun readExactly(reader: Reader, length: Int): String {
        val chars = CharArray(length)
        var offset = 0
        while (offset < length) {
            val charsRead: Int = reader.read(chars, offset, length - offset)
            if (charsRead <= 0) {
                throw IOException("Stream terminated early")
            }
            offset += charsRead
        }
        return String(chars)
    }


}