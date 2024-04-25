package com.frazieje.findinpi.plugins

import com.frazieje.findinpi.PiFinderType
import com.frazieje.findinpi.model.SearchRequest
import com.frazieje.findinpi.service.FindInPi
import com.frazieje.findinpi.service.KPiFinder
import com.frazieje.findinpi.service.PiFinder
import io.ktor.server.application.*
import io.ktor.server.request.*
import io.ktor.server.response.*
import io.ktor.server.routing.*
import kotlin.reflect.KClass
import kotlin.reflect.full.createInstance

fun Application.configureRouting(dataFilePath: String, finderType: PiFinderType) {

    val findInPi = FindInPi(dataFilePath, 1048576, 8)
    routing {
        post("/search") {
            val req = call.receive<SearchRequest>()
            call.respond(findInPi.find(finderType.finderClass.createInstance(), req.searchText))
        }
    }
}
