package com.frazieje.findinpi.service

import com.google.gson.JsonDeserializationContext
import com.google.gson.JsonDeserializer
import com.google.gson.JsonElement
import java.lang.reflect.Type

class FemtoSearchResultDeserializer : JsonDeserializer<FemtoSearchResult> {
    override fun deserialize(
        json: JsonElement, typeOfT: Type, context: JsonDeserializationContext
    ): FemtoSearchResult = FemtoSearchResult(
        json.asJsonObject.get("results").asJsonArray.get(0).asJsonObject.get("offsets").asJsonArray.map { it.asLong })
}