package com.frazieje.findinpi.service

import com.frazieje.findinpi.model.SearchResult

interface PiFinder {
    fun init(dataFilePath: String, readBufferSize: Long)
    fun search(searchText: String): SearchResult
}