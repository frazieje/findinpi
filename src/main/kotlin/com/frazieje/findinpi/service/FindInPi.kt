package com.frazieje.findinpi.service

import com.frazieje.findinpi.model.SearchResult
import kotlinx.coroutines.*
import org.slf4j.LoggerFactory

class FindInPi(private val piFinder: PiFinder) {
    private val logger = LoggerFactory.getLogger(javaClass)
    suspend fun find(searchText: String, maxResultCount: Int): SearchResult = withContext(Dispatchers.IO) {
        logger.debug(
            "find called - pattern: {}, maxResultCount: {}",
            searchText,
            maxResultCount
        )
        piFinder.search(searchText, if (maxResultCount > 101) 101 else maxResultCount)
    }
}