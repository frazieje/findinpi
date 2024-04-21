package com.frazieje.findinpi.model

data class SearchResult(
    val found: Boolean,
    val offset: Long,
    val searchTimeMs: Long,
    val message: String? = null
)
