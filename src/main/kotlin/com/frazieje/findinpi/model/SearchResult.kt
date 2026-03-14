package com.frazieje.findinpi.model

data class SearchResult(
    val count: Long,
    val offset: Long,
    val excerpt: String,
    val excerptOffset: Int,
    val searchTimeMs: Long,
    val message: String? = null
)
