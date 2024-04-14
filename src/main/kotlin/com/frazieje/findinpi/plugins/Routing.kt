package com.frazieje.findinpi.plugins

import com.frazieje.findinpi.model.SearchRequest
import com.frazieje.findinpi.model.SearchResult
import com.frazieje.findinpi.service.FindInPi
import com.frazieje.findinpi.service.KPiFinder
import com.frazieje.findinpi.service.NativePiFinder
import io.ktor.server.application.*
import io.ktor.server.request.*
import io.ktor.server.response.*
import io.ktor.server.routing.*
import java.io.File

fun Application.configureRouting() {
    val findInPi = FindInPi(File("/home/frazieje/Pi25B.txt"), 1048576)
    routing {
        post("/search") {
            val req = call.receive<SearchRequest>()
            call.respond(findInPi.find(NativePiFinder(), req.searchText))
        }
    }
}
