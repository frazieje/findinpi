package com.frazieje.findinpi.model

data class SearchRequest(
    val searchText: String,
    val maxResultCount: Int = 1,
)
