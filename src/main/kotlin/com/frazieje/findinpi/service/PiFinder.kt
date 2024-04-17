package com.frazieje.findinpi.service

import com.frazieje.findinpi.model.SearchResult

interface PiFinder {
    fun search(dataFilePath: String, searchText: String, bufferSize: Long, offset: Long, limit: Long): SearchResult
}