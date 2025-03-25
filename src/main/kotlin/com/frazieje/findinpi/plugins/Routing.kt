package com.frazieje.findinpi.plugins

import com.frazieje.findinpi.model.SearchRequest
import com.frazieje.findinpi.service.FindInPi
import io.ktor.server.application.*
import io.ktor.server.request.*
import io.ktor.server.response.*
import io.ktor.server.routing.*

fun Application.configureRouting(findInPi: FindInPi) {
    routing {
        post("/api/search") {
            val req = call.receive<SearchRequest>()
            call.respond(findInPi.find(req.searchText, req.maxResultCount))
        }
    }
}
