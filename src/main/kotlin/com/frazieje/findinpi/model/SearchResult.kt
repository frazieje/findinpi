package com.frazieje.findinpi.model

data class SearchResult(
    val count: Long,
    val offsets: List<Long>,
    val searchTimeMs: Long,
    val message: String? = null
)
