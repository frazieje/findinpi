package com.frazieje.findinpi.model

data class SearchResult(
    val found: Boolean,
    val offset: Long,
    val timeMs: Long
)
