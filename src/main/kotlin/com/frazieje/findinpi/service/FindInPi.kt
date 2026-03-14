package com.frazieje.findinpi.service

import com.frazieje.findinpi.model.SearchResult
import kotlinx.coroutines.*
import org.slf4j.LoggerFactory

class FindInPi(private val piFinder: PiFinder) {
    private val logger = LoggerFactory.getLogger(javaClass)
    private val regex = Regex("""^[.0-9]{1,100}$""")
    suspend fun find(searchText: String): SearchResult = withContext(Dispatchers.IO) {
        logger.debug(
            "find called - pattern: {}",
            searchText,
        )
        if (!regex.matches(searchText)) {
            SearchResult(0, -1, "", -1, -1,"Not a valid search term")
        } else {
            piFinder.search(searchText)
        }
    }
}