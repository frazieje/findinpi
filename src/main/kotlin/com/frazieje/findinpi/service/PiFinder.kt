package com.frazieje.findinpi.service

import com.frazieje.findinpi.model.SearchResult

interface PiFinder {
    fun init(dataFilePath: String)
    fun search(searchText: String, maxResultCount: Int): SearchResult
}