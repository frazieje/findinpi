package com.frazieje.findinpi.plugins

import com.frazieje.findinpi.model.SearchRequest
import io.ktor.serialization.gson.*
import io.ktor.server.application.*
import io.ktor.server.plugins.contentnegotiation.*

fun Application.configureSerialization() {
    install(ContentNegotiation) {
        gson {
            registerTypeAdapter(SearchRequest::class.java, SearchRequestDeserializer())
        }
    }
}