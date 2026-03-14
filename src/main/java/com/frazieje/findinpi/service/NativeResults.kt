package com.frazieje.findinpi.service

data class NativeSearchResult(
    val offset: Long,
    val excerpt: String,
    val excerptOffset: Int,
)

data class NativeCountResult(
    val count: Long
)