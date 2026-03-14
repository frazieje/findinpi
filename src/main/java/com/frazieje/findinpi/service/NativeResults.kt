package com.frazieje.findinpi.service

data class NativeSearchResult(
    val offsets: List<Long>
)

data class NativeCountResult(
    val count: Long
)