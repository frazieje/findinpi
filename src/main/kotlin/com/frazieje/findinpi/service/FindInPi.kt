package com.frazieje.findinpi.service

import com.frazieje.findinpi.model.SearchResult
import io.ktor.util.*
import kotlinx.coroutines.*
import kotlinx.coroutines.channels.produce
import kotlinx.coroutines.selects.select
import org.slf4j.LoggerFactory
import java.io.*
import java.util.concurrent.CopyOnWriteArrayList
import kotlin.coroutines.Continuation
import kotlin.coroutines.resume
import kotlin.math.ceil

class FindInPi(private val piFinder: PiFinder) {
    private val logger = LoggerFactory.getLogger(javaClass)
    suspend fun find(searchText: String): SearchResult = withContext(Dispatchers.IO) {
        logger.debug(
            "find called with {} and {}",
            piFinder,
            searchText,
        )
        piFinder.search(searchText)
    }
}