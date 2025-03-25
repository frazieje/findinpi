package com.frazieje.findinpi.service

import com.google.gson.JsonDeserializationContext
import com.google.gson.JsonDeserializer
import com.google.gson.JsonElement
import java.lang.reflect.Type

class FemtoCountResultDeserializer : JsonDeserializer<FemtoCountResult> {
    override fun deserialize(
        json: JsonElement,
        typeOfT: Type,
        context: JsonDeserializationContext
    ): FemtoCountResult {
        val matchesArray = json.asJsonObject.get("matches").asJsonArray
        return FemtoCountResult(
            if (matchesArray.size() > 0) {
                val rangeArray = matchesArray.get(0).asJsonObject.get("range").asJsonArray
                rangeArray.get(1).asLong - rangeArray.get(0).asLong + 1
            } else 0
        )
    }
}