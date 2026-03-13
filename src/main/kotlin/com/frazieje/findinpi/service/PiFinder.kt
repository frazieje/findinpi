package com.frazieje.findinpi.service

import com.frazieje.findinpi.model.SearchResult

interface PiFinder {
    fun init(dataFilePath: String, suffixArrayFilePath: String, fmIndexFilePath: String)
    fun search(searchText: String): SearchResult
}