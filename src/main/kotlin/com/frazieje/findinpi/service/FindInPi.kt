package com.frazieje.findinpi.service

import com.frazieje.findinpi.model.SearchResult
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.*

class FindInPi(
    private val dataFile: File,
    private val bufferSizeBytes: Int,
) {
    suspend fun find(piFinder: PiFinder, searchText: String): SearchResult = withContext(Dispatchers.IO) {
        piFinder.search(dataFile.absolutePath, searchText, bufferSizeBytes)
    }
}