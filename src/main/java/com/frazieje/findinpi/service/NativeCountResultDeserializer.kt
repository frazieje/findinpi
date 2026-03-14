package com.frazieje.findinpi.service

import com.google.gson.JsonDeserializationContext
import com.google.gson.JsonDeserializer
import com.google.gson.JsonElement
import java.lang.reflect.Type

class NativeCountResultDeserializer : JsonDeserializer<NativeCountResult> {
    override fun deserialize(
        json: JsonElement,
        typeOfT: Type,
        context: JsonDeserializationContext
    ): NativeCountResult {
        val matchesArray = json.asJsonObject.get("matches").asJsonArray
        return NativeCountResult(
            if (matchesArray.size() > 0) {
                val rangeArray = matchesArray.get(0).asJsonObject.get("range").asJsonArray
                rangeArray.get(1).asLong - rangeArray.get(0).asLong + 1
            } else 0
        )
    }
}